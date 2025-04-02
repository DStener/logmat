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


using namespace API;


// URL: http://localhost:5555/api/auth/register
// URL: http://localhost:5555/register.html
void Auth::Register(const HttpRequestPtr& req, callback_func &&callback)
{
    // Convert request body to and register
    auto info = DTO::RequestBody::To<RegisterDTO>(req->getBody());
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
