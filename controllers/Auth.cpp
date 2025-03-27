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


using namespace API;


// URL: http://localhost:5555/api/auth/register
// URL: http://localhost:5555/register.html
void Auth::Register(const HttpRequestPtr& req, callback_func &&callback)
{
    auto reg = Request::Register(req, callback);

    // If there is any error
    if(reg.message.size())
    {
        Json::Value json(reg.message);

        auto response = HttpResponse::newHttpJsonResponse(json);
        response->setStatusCode(drogon::k415UnsupportedMediaType);
        callback(response);

        return;
    }

    // If everything is fine
    auto response = HttpResponse::newHttpResponse();
    response->setStatusCode(drogon::k201Created);
    callback(response);
}

// URL: http://localhost:5555/api/auth/login
// URL: http://localhost:5555/login.html
void Auth::Login(const HttpRequestPtr& req, callback_func &&callback)
{
    // Convert request body to LoginDTO and login
    auto info = DTO::CreateFromRequestBody<LoginDTO>(req->getBody());
    auto login = Request::Login(info, req, callback);

    // If there is any error
    if(!login.id)
    {
        Json::Value json(login.message);

        auto response = HttpResponse::newHttpJsonResponse(json);
        response->setStatusCode(drogon::k415UnsupportedMediaType);
        callback(response);

        return;
    }

    // If everything is fine
    // Create token cookie
    Cookie cookie("token", login.message);
    cookie.setPath("/");

    // Create response
    auto response = HttpResponse::newHttpResponse();
    response->setStatusCode(drogon::k200OK);
    response->addCookie(cookie);

    callback(response);
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

    auto response = HttpResponse::newHttpJsonResponse(DTO::ToJson(userinfo));
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
        json.append(DTO::ToJson(row));
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

    Json::Value json(login.Get2FAKey());

    auto response = HttpResponse::newHttpJsonResponse(json);
    response->setStatusCode(drogon::k200OK);
    callback(response);
}
