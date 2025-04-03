#include "Auth.h"
#include "System/DTO.hpp"
#include "Request/register.hpp"
#include "struct_declaration.hpp"
#include "Request/login.hpp"
// #include "include/System.hpp"

#include <drogon/Cookie.h>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <drogon/HttpTypes.h>
#include <drogon/utils/Utilities.h>
#include <json/value.h>
#include <string>
#include <trantor/utils/Logger.h>

#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/regex.hpp>

#include "math.hpp"
#include <algorithm>


using namespace API;


// URL: http://localhost:5555/api/auth/register
// URL: http://localhost:5555/register.html
void Auth::Register(const HttpRequestPtr& req, callback_func &&callback)
{
    // Convert request body to and register
    auto info = DTO::RequestBody::To<RegisterDTO>(req->getBody());
    auto captcha = DTO::RequestBody::To<::Captcha>(req->getBody());
    auto answers = DB::get()->Select<::Captcha>(std::format("token = \"{}\"", req->getCookie("captcha_token")));

    std::string message;

    // If the record has not been found or the recording period has expired
    if(!answers.size() || answers[0].second.time < std::chrono::system_clock::now())
    {
        // Set message
        message = "Срок CAPTCHA истёк. Перезагрузите страницу";
        // Delete captcha 
        DB::get()->Delete<::Captcha>(answers[0].first);
    }
    // Check correct answer
    if(std::abs(answers[0].second.answer - captcha.answer) > 0.00001)
    {
        // Set message
        message = "Неверное решение CAPTCHA";
    }

    // Callback if the captcha failed
    if (message.size())
    {
        auto response = HttpResponse::newHttpJsonResponse(Json::Value(message));
        response->setStatusCode(drogon::k406NotAcceptable);
        callback(response);

        return;
    }

    // Delete captcha 
    DB::get()->Delete<::Captcha>(answers[0].first);
    auto ret = Request::Register(info, req, callback);
}

// URL: http://localhost:5555/api/auth/login
// URL: http://localhost:5555/login.html
void Auth::Login(const HttpRequestPtr& req, callback_func &&callback)
{
    // Convert request body to LoginDTO and login
    auto info = DTO::RequestBody::To<LoginDTO>(req->getBody());
    Request::Login(info, req, callback);
}

// URL: http://localhost:5555/api/auth/me
void Auth::Info(const HttpRequestPtr& req, callback_func &&callback)
{
    // Login and check Permission
    auto login = Request::Login(req, callback);
    if(!login.id) { return; }

    // Get info about current user
    auto userinfo = DB::get()->Select<User>(std::format("id == {}", login.id))[0].second;
    userinfo.password = "X";

    auto response = HttpResponse::newHttpJsonResponse(DTO::JSON::From(userinfo));
    response->setStatusCode(drogon::k200OK);
    callback(response);
}

// URL: http://localhost:5555/api/auth/out
void Auth::Logout(const HttpRequestPtr& req, callback_func &&callback)
{
    // Login and check Permission
    auto login = Request::Login(req, callback);
    if(!login.id) { return; }

    auto id = DB::get()->Select<::Token>(
        std::format("token == \"{}\"", req->getCookie("token")))[0].first;
    // Delete current token
    DB::get()->Delete<::Token>(id);

    auto response = HttpResponse::newHttpResponse();
    response->setStatusCode(drogon::k200OK);
    callback(response);
}

// URL: http://localhost:5555/api/auth/out_all
void Auth::LogoutAll(const HttpRequestPtr& req, callback_func &&callback)
{
    // Login and check Permission
    auto login = Request::Login(req, callback);
    if(!login.id) { return; }

    // Delete all tokens by the user
    auto tokens = DB::get()->Select<::Token>(std::format("user == {}", login.id));
    for(auto& row : tokens)
    {
        DB::get()->Delete<Token>(row.first);
    }

    auto response = HttpResponse::newHttpResponse();
    response->setStatusCode(drogon::k200OK);
    callback(response);
}

// URL: http://localhost:5555/api/auth/tokens
void Auth::getTokens(const HttpRequestPtr& req, callback_func &&callback)
{
    // Login and check Permission
    auto login = Request::Login(req, callback);
    if(!login.id) { return; }

    Json::Value json;
    for(auto& row : DB::get()->Select<Token>(std::format("user == {}", login.id)))
    {
        json.append(DTO::JSON::From(row));
    }

    auto response = HttpResponse::newHttpJsonResponse(json);
    response->setStatusCode(drogon::k200OK);
    callback(response);
}

///////////////////////////////// 2FA //////////////////////////////////////////

void Auth::Switch2FA(const HttpRequestPtr& req, callback_func &&callback)
{
    // Login and check Permission
    auto login = Request::Login(req, callback);
    if(!login.id) { return; }

    login.Switch2FA();
}

void Auth::Get2FAKey(const HttpRequestPtr& req, callback_func &&callback)
{
    auto login = Request::Login(req, callback);
    if(!login.id) { return; }

    login.Get2FAKey();
}

void Auth::Is2FAEnbaled(const HttpRequestPtr& req, callback_func &&callback,
                        std::string username)
{
    Json::Value json;
    auto users = DB::get()->Select<::User>(std::format("username == \"{}\"", username));

    json["enable"] = (users.size() && !DTO::SQL::CheckField::isNull(users[0].second.time2FA));

    auto response = HttpResponse::newHttpJsonResponse(json);
    response->setStatusCode(drogon::k200OK);
    callback(response);
}


/////////////////////////////// Captcha ////////////////////////////////////////

void Auth::GetCaptcha(const HttpRequestPtr& req, callback_func&& callback)
{
    std::srand(std::time({}));
    const int operation_id = std::rand() % 6;

    double a = std::rand() % 1000 + 2;
    double b = std::rand() % (static_cast<int>(a) - 1) + 1;

    std::string equation;

    ::Captcha captcha{};
    captcha.time = std::chrono::system_clock::now() + std::chrono::minutes(5);
    

    if (operation_id == 0) 
    {
        equation = std::format("{} + {}", a, b);
        math::add(a, b, captcha.answer);
    }
    else if (operation_id == 1)
    {
        equation = std::format("{} - {}", a, b);
        math::sub(a, b, captcha.answer);
    }
    else if (operation_id == 2)
    {
        equation = std::format("{} * {}", a, b);
        math::prod(a, b, captcha.answer);
    }
    else if (operation_id == 3)
    {
        equation = std::format("{} / {}", a, b);
        math::quot(a, b, captcha.answer);
    }
    else if (operation_id == 4)
    {
        a = static_cast<int>(a) % 3 + 1;
        b = static_cast<int>(b) % 1 + 1;

        equation = std::format("A({},{})", a, b);
        captcha.answer =  math::A(a, b);
    }
    else if (operation_id == 5)
    {
        a = static_cast<int>(a) % 3 + 1;
        b = static_cast<int>(b) % 1 + 1;

        equation = std::format("C({},{})", a, b);
        captcha.answer = math::C(a, b);
    }

    captcha.token = utils::base64Encode(std::format("{}{}", equation, 
                            captcha.time.time_since_epoch().count()));

    DB::get()->Insert(captcha);

    Cookie cookie("captcha_token", captcha.token);
    cookie.setPath("/");
    
    
    auto response = HttpResponse::newHttpJsonResponse(Json::Value(equation));
    response->setStatusCode(drogon::k200OK);
    response->addCookie(cookie);

    callback(response);

}