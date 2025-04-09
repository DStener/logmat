#pragma once

#include <drogon/drogon.h>

#include "System/database.hpp"
#include "System/utils.h"

#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <qrencode.h>

#include <qrencode.h>
#include <boost/gil.hpp>
#include <boost/gil/extension/io/jpeg.hpp>
#include <boost/gil/extension/numeric/resample.hpp>
#include <boost/gil/extension/numeric/sampler.hpp>
using namespace boost::gil;


using namespace drogon;
using callback_func = std::function<void (const HttpResponsePtr &)>;

#define MAX_TOKEN 32
#undef CHECK_AND_CALLBACK
#define CHECK_AND_CALLBACK(condition)                           \
if(condition) {                                                 \
    Json::Value json("Пожалуйста, войдите в систему");          \
                                                                \
    auto response = HttpResponse::newHttpJsonResponse(json);    \
    response->setStatusCode(drogon::k406NotAcceptable);         \
    callback(response);                                         \
                                                                \
    return;                                                     \
}
#undef FORBIDEN_CALLBACK
#define FORBIDEN_CALLBACK                                       \
    Json::Value json(permission);                               \
    auto response = HttpResponse::newHttpJsonResponse(json);    \
    response->setStatusCode(drogon::k403Forbidden);             \
    callback(response);                                         \
    return false;

namespace Request{
class Login
{
public:
    id_t id;
    ::User user;

private:
    HttpRequestPtr reqest;
    callback_func& callback;

public:

    static ::User GetUser(const HttpRequestPtr& req)
    {
        ::User user{};

        // Get vector of struct Token where field token is qual to the sample
        auto tokens = DB::get()->Select<::Token>(std::format("token == \"{}\"", req->session()->sessionId()));
        if (tokens.empty()) { return user; }

        // Get user DTO
        auto users = DB::get()->Select<::User>(std::format("id == {}", tokens[0].second.user));
        if (users.empty()) { return user; }

        return users[0].second;
    }

    Login(const HttpRequestPtr& req, callback_func& callback)
        : id(0), reqest(req), callback(callback)
    {
        // Get session id
        auto token = reqest->session()->sessionId();

        // Get vector of struct Token where field token is qual to the sample
        auto tokens = DB::get()->Select<::Token>(std::format("token == \"{}\"", token));
        CHECK_AND_CALLBACK(!tokens.size());

        Login::id = tokens[0].second.user;
        Login::user = DB::get()->Select<::User>(std::format("id == {}", id))[0].second;
    }

    Login(LoginDTO& info, const HttpRequestPtr& req, callback_func& callback)
        : id(0), reqest(req), callback(callback)
    {
        auto arr = DB::get()->Select<::User>(std::format("username == \"{}\"", info.username));

        if(arr.size() == 0)
        {
            Json::Value json("Некорректный логин или пароль.");

            auto response = HttpResponse::newHttpJsonResponse(json);
            response->setStatusCode(drogon::k415UnsupportedMediaType);
            callback(response);
            return;
        }

        // by 2FA
        if(!DTO::SQL::CheckField::isNull(arr[0].second.time2FA))
        {
            auto open_key = getOpenKey(arr[0].second.username, arr[0].second.time2FA);

            if(info.code != TOTP(open_key))
            {
                Json::Value json("Некорректный логин или ключ.");

                auto response = HttpResponse::newHttpJsonResponse(json);
                response->setStatusCode(drogon::k415UnsupportedMediaType);
                callback(response);
                return;
            }
        }
        // by Password
        else
        {
            // Check correct data
            if(arr.size() == 0 || arr[0].second.password != utils::getMd5(info.password))
            {
                Json::Value json("Некорректный логин или пароль.");

                auto response = HttpResponse::newHttpJsonResponse(json);
                response->setStatusCode(drogon::k415UnsupportedMediaType);
                callback(response);
                return;
            }
        }

        // If username and password are correct,
        // AND the token limit has not been exceeded
        // then create token
        auto tokens = DB::get()->Select<Token>(std::format("user == {}", arr[0].first));
        if(tokens.size() >= MAX_TOKEN)
        {
            Json::Value json("Достигнут лимит по активным сессиям.");

            auto response = HttpResponse::newHttpJsonResponse(json);
            response->setStatusCode(drogon::k415UnsupportedMediaType);
            callback(response);
            return;
        }

        // Set correct id of User
        Login::id = arr[0].first;
        Login::user = arr[0].second;

        // Generate tokens
        Token t;
        t.token = req->session()->sessionId();
        t.user = arr[0].first;

        // Add token to DB
        DB::get()->Insert(t);

        // Convert token to std::string
        std::stringstream stream;
        stream << t.token;

        // Create response
        auto response = HttpResponse::newHttpResponse();
        response->setStatusCode(drogon::k200OK);

        callback(response);
    }

    bool hasPermission(const std::string& permission)
    {
        // Initial login check
        if(!id) { return false; }

        auto _cash = DB::get()->Select<::Permission>(std::format("name == \"{}\"", permission));
        if (!_cash.size()) { return false; }

        auto perm = _cash[0];

        // Check soft delete
        CHECK_SOFT_DELETE(perm) { FORBIDEN_CALLBACK }

        // Check if the user has a concrete permission
        auto role_perm = DB::get()->Select<::RoleAndPermission>(std::format("permission == {}", perm.first));
        for(auto& link : role_perm)
        {
            CHECK_SOFT_DELETE(link) { continue; };

            auto user_role = DB::get()->Select<::UserAndRole>(std::format("role == {}", link.second.role));

            for(auto& link2 : user_role)
            {
                CHECK_SOFT_DELETE(link2) { continue; }

                if(link2.second.user == Login::id)
                {
                    return true;
                }
            }
        }
        FORBIDEN_CALLBACK
    }

    template <typename T>
    bool hasPermission(const std::string& permission)
    { 
        // Get name T and convert to lower case
        std::string name = DTO::GetName<T>();
        for (auto& c : name)
        {
            c = std::tolower(c);
        }

        return hasPermission(std::format("{}-{}", permission, name));
    }

    bool isAdmin()
    {
        auto roles = DB::get()->Select<::Role>("code == \"ADMIN\"");
        if (roles.empty()) { return false; }

        auto user_role = DB::get()->Select<::UserAndRole>(std::format("user == {} AND role = {}", id, roles[0].first));
        if (roles.empty()) { return false; }

        return true;
    }

    static bool isAdmin(const HttpRequestPtr& req)
    {
        auto tokens = DB::get()->Select<::Token>(std::format("token == \"{}\"", req->session()->sessionId()));
        if (tokens.empty()) { return false; }

        auto roles = DB::get()->Select<::Role>("code == \"ADMIN\"");
        if (roles.empty()) { return false; }

        auto user_role = DB::get()->Select<::UserAndRole>(std::format("user == {} AND role == {}", tokens[0].second.user, roles[0].first));
        if (user_role.empty()) { return false; }

        return true;
    }

    void Get2FAKey()
    {
        if(DTO::SQL::CheckField::isNull(Login::user.time2FA))
        {
            auto response = HttpResponse::newHttpResponse();
            response->setStatusCode(drogon::k403Forbidden);
            callback(response);

            return;
        }

        std::string open_key = getOpenKey(true);
        std::string message = std::format(
                                "otpauth://totp/logMat?secret={}&issure=logMat",
                                API::Utils::base32Encode(open_key));


        QRcode *qr = QRcode_encodeString(message.c_str(), 0, QR_ECLEVEL_H, QR_MODE_8, 1);

        std::stringstream out_buffer(std::ios_base::out | std::ios_base::binary );
        rgb8_image_t img(qr->width, qr->width);

        auto* pSourceData = qr->data;
        for(auto& pixel : boost::gil::view(img))
        {
            pixel = rgb8_pixel_t(!(*pSourceData & 1) * 255);
            pSourceData++;
        }

        rgb8_image_t img_resize(qr->width * 4, qr->width * 4); // rgb_image is 136x98
        resize_view(const_view(img), view(img_resize), nearest_neighbor_sampler());
        write_view(out_buffer, const_view(img_resize), jpeg_tag{});

        QRcode_free(qr);

        auto response = HttpResponse::newFileResponse(
                    reinterpret_cast<unsigned char *>(out_buffer.str().data()),
                    out_buffer.str().size(), "", CT_IMAGE_JPG);
        callback(response);
    }


    void Switch2FA()
    {
        auto info = DTO::ConvertTo<::LoginDTO>(reqest);

        std::stringstream hash{};
        hash << std::hash<std::string>{}(info.password);

        // Check correct data
        if(user.password != hash.str())
        {
            Json::Value json("Для смены способа аутентификации необходимо "
                             "указать корректный пароль");

            auto response = HttpResponse::newHttpJsonResponse(json);
            response->setStatusCode(drogon::k415UnsupportedMediaType);
            callback(response);
            return;
        }

        // Switch value
        Json::Value json;
        if(DTO::SQL::CheckField::isNull(user.time2FA))
        {
            Login::user.time2FA = std::chrono::system_clock::now();
            json["enable"] = true;
        }
        else
        {
            Login::user.time2FA = std::chrono::system_clock::time_point();
            json["enable"] = false;
        }

        DB::get()->Update<::User>(id, Login::user);

        auto response = HttpResponse::newHttpJsonResponse(json);
        response->setStatusCode(drogon::k200OK);
        callback(response);
    }



private:
    std::string getOpenKey(std::string& username, time_p& time)
    {
        return std::format("{}{}",
                           username,
                           time.time_since_epoch().count());
    }


    std::string getOpenKey(bool override = false)
    {
        if(override)
        {
            Login::user.time2FA = std::chrono::system_clock::now();
            DB::get()->Update<::User>(id, Login::user);
        }

        return getOpenKey(Login::user.username, Login::user.time2FA);
    }

    std::string TOTP(std::string key)
    {
        using namespace std::chrono;

        std::vector<uint8_t> md(20);
        uint64_t time =\
            std::byteswap(system_clock::now().time_since_epoch() / seconds(30));

        // Calculate HMAC_SHA1
        HMAC(EVP_sha1(),
             key.data(), key.size(),
             reinterpret_cast<uint8_t*>(&time), sizeof(time),
             reinterpret_cast<uint8_t*>(md.data()), nullptr);
        OPENSSL_cleanse(key.data(), key.size());

        // Get the number from the last 4 bits
        int offset = md[19] & 0b1111;
        // Magic
        int bin_code = (md[offset] & 0x7f) << 24
            | (md[offset+1] & 0xff) << 16
            | (md[offset+2] & 0xff) << 8
            | (md[offset+3] & 0xff);
        bin_code = bin_code % 1000000;

        return std::to_string(bin_code);
    }
};
}

