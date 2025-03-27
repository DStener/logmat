#pragma once

#include <drogon/drogon.h>

#include "System/database.hpp"
#include "System/utils.h"

#include <openssl/hmac.h>
#include <openssl/evp.h>

using namespace drogon;
using callback_func = std::function<void (const HttpResponsePtr &)>;

#define MAX_TOKEN 32
#undef CHECK_AND_CALLBACK
#define CHECK_AND_CALLBACK(condition, txt)                                      \
if(condition)                                                                   \
{                                                                               \
    if(info.asJSON == "true")                                                   \
    {                                                                           \
        Json::Value json(txt);                                                  \
                                                                                \
        auto response = HttpResponse::newHttpJsonResponse(json);                \
        response->setStatusCode(drogon::k415UnsupportedMediaType);              \
        callback(response);                                                     \
    }                                                                           \
    else                                                                        \
    {                                                                           \
        HttpViewData data;                                                      \
        data.insert("message_visibility", "\"visibility: visible;\"");          \
        data.insert("message_txt", txt);                                        \
                                                                                \
        auto response = HttpResponse::newHttpViewResponse("signin.csp", data);  \
        response->setStatusCode(drogon::k415UnsupportedMediaType);              \
        callback(response);                                                     \
    }                                                                           \
    return;                                                                     \
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
    std::string message;

private:
    LoginDTO info;
    HttpRequestPtr reqest;
    callback_func& callback;

public:

    Login(const HttpRequestPtr& req, callback_func& callback)
        : id(0), reqest(req), callback(callback)
    {
        info = DTO::CreateFromRequestBody<LoginDTO>(req->getBody());
        auto token = reqest->getCookie("token");

        // If there is no cookie with the token
        if(!token.size())
        {
            message = "Пожалуйста, войдите в систему";
            return;
        }

        // Get vector of struct Token where field token is qual to the sample
        auto tokens = DB::get()->Select<::Token>(std::format("token == \"{}\"", token));
        if(!tokens.size())
        {
            message = "Пожалуйста, войдите в систему";
            return;
        }

        // Checking for expired token lifetime
        if(tokens[0].second.time < std::chrono::system_clock::now())
        {
            message = "Пожалуйста, войдите в систему";
            return;
        }

        id = tokens[0].second.user.id;
    }

    Login(LoginDTO& info, const HttpRequestPtr& req, callback_func& callback)
        : id(0), info(info), reqest(req), callback(callback)
    {
        auto arr = DB::get()->Select<::User>(std::format("username == \"{}\"", info.username));

        if(arr.size() == 0)
        {
            message = "Некорректный логин или пароль.";
            return;
        }

        // by 2FA
        if(!DTO::SQL::CheckField::isNull(arr[0].second.time2FA))
        {
            auto open_key = std::format("{}{}",
                                        SQL::REMOVE_ATTRIB(arr[0].second.username),
                                        SQL::REMOVE_ATTRIB(arr[0].second.time2FA));

            if(info.code != TOTP(open_key))
            {
                message = "Некорректный логин или ключ.";
                return;
            }
        }
        // by Password
        else
        {
            // Calculate hash
            std::stringstream hash{};
            hash << std::hash<std::string>{}(info.password);

            // Check correct data
            if(arr.size() == 0 || arr[0].second.password != hash.str())
            {
                message = "Некорректный логин или пароль.";
                return;
            }

        }

        // If username and password are correct,
        // AND the token limit has not been exceeded
        // then create token
        auto tokens = DB::get()->Select<Token>(std::format("user == {}", arr[0].first));

        if(tokens.size() >= MAX_TOKEN)
        {
            message = "Достигнут лимит по активным сессиям.";
            return;
        }

        // Set correct id of User
        Login::id = arr[0].first;

        // Generate tokens
        Token t = Login::generateToken();
        t.user = arr[0].first;

        // Add token to DB
        DB::get()->Insert(t);

        // Convert token to std::string
        std::stringstream stream;
        stream << t.token;

        message = stream.str();
    }

    bool hasPermission(const std::string& permission)
    {
        // Initial login check
        if(!id) { return 0; }

        auto perm = DB::get()->Select<::Permission>(std::format("name == \"{}\"", permission))[0];

        // Check soft delete
        CHECK_SOFT_DELETE(perm) { FORBIDEN_CALLBACK }

        // Check if the user has a concrete permission
        auto role_perm = DB::get()->Select<::RoleAndPermission>(std::format("permission == {}", perm.first));
        for(auto& link : role_perm)
        {
            CHECK_SOFT_DELETE(link) { continue; };
            auto user_role = DB::get()->Select<::UserAndRole>(std::format("role == {}", link.second.role.id));

            for(auto& link2 : user_role)
            {
                CHECK_SOFT_DELETE(link2) { continue; }

                if(link2.second.user.id == Login::id)
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
        return hasPermission(std::format("{}-{}", permission, DTO::GetName<T>()));
    }

    std::string Get2FAKey()
    {
        auto user = DB::get()->Select<::User>(std::format("id == {}", id))[0].second;

        // Change time2FA
        auto time = std::chrono::utc_clock::now();
        DB::get()->Update<::User>(id, std::format("time2FA = \"{}\"", time));

        std::string open_key = std::format("{}{}",
                                           SQL::REMOVE_ATTRIB(user.username), time);

        return API::Utils::base32Encode(open_key);
    }

    void Switch2FA()
    {
        auto info = DTO::CreateFromRequestBody<::LoginDTO>(reqest->getBody());
        auto user = DB::get()->Select<::User>(std::format("id == {}", id))[0].second;

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

        auto time = (DTO::SQL::CheckField::isNull(user.time2FA))?
                    std::chrono::utc_clock::now() :
                    std::chrono::utc_clock::time_point();

        // Update flag
        DB::get()->Update<::User>(id, std::format("time2FA = \"{}\"", time));

        auto response = HttpResponse::newHttpResponse();
        response->setStatusCode(drogon::k200OK);
        callback(response);
    }



private:
    Token generateToken()
    {
        ::Token t{};
        t.user = Login::id;
        t.time = std::chrono::system_clock::now() + std::chrono::months(18);
        const std::string characters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");

        std::srand(std::time(0));
        for(int i = 0; i < 32; ++i ) {
            t.token.value.value += characters[std::rand() % characters.size()];
        }

        return t;
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

