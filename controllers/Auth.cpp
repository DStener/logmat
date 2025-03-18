#include "Auth.h"
#include "DTO.hpp"
#include "registerrequest.h"
#include "struct_declaration.hpp"
#include "loginrequest.h"
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
    // Convert request body to RegisterDTO
    auto info = \
        DTO::CreateFromRequestBody<RegisterDTO>(req->getBody());

    // Create new user if all conditions are met
    auto ret = API::RegisterRequest::newUser(info);

    auto response = \
        HttpResponse::newHttpJsonResponse(DTO::ToJson(ret));

    response->setStatusCode(ret.code);

    callback(response);
}


// URL: http://localhost:5555/api/auth/login
// URL: http://localhost:5555/login.html
void Auth::Login(const HttpRequestPtr& req, callback_func &&callback)
    
{
    // Convert request body to LoginDTO
    auto info = \
        DTO::CreateFromRequestBody<LoginDTO>(req->getBody());

    // Login new user if is try
    auto ret = API::LoginRequest::loginUser(info);
    
    // Create token cookie
    Cookie cookie("token", "invalid");

    if (ret.code == drogon::k200OK) {
        cookie.setValue(ret.message);
        ret.message = "";
    }

    // Create response
    auto response = \
        HttpResponse::newHttpJsonResponse(DTO::ToJson(ret));
    
    response->addCookie(cookie);
    response->setStatusCode(ret.code);

    callback(response);
}


// URL: http://localhost:5555/api/auth/me
void Auth::Info(const HttpRequestPtr& req, callback_func &&callback)
{
    HttpResponsePtr response;
    ReturnDTO ret {drogon::k200OK};

    auto user_id = LoginRequest::getUserByToken(req->getCookie("token"), ret);

    if(ret.code == drogon::k200OK) {
        auto userinfo = DB::get()->Select<User>(std::format("id == {}", user_id))[0].second;
        response = HttpResponse::newHttpJsonResponse(DTO::ToJson(userinfo));
    } else {
        response = HttpResponse::newHttpJsonResponse(DTO::ToJson(ret));
    }

    response->setStatusCode(ret.code);
    callback(response);
}


// URL: http://localhost:5555/api/auth/out
void Auth::Logout(const HttpRequestPtr& req, callback_func &&callback)
{
    HttpResponsePtr response;
    ReturnDTO ret {drogon::k200OK};

    LoginRequest::getUserByToken(req->getCookie("token"), ret);

    if(ret.code == drogon::k200OK) {
        DB::get()->Delete<Token>(std::format("tokens == {}", req->getCookie("token")));
    }

    response = HttpResponse::newHttpJsonResponse(DTO::ToJson(ret));
    response->setStatusCode(ret.code);

    callback(response);
}

// URL: http://localhost:5555/api/auth/out_all
void Auth::LogoutAll(const HttpRequestPtr& req, callback_func &&callback)
{
    HttpResponsePtr response;
    ReturnDTO ret {drogon::k200OK};

    auto user_id = LoginRequest::getUserByToken(req->getCookie("token"), ret);

    if(ret.code == drogon::k200OK) {
        DB::get()->Delete<Token>(std::format("user == {}", user_id));
    }

    response = HttpResponse::newHttpJsonResponse(DTO::ToJson(ret));
    response->setStatusCode(ret.code);

    callback(response);
}

// URL: http://localhost:5555/api/auth/tokens
void Auth::getTokens(const HttpRequestPtr& req, callback_func &&callback)
{
    HttpResponsePtr response;
    ReturnDTO ret {drogon::k200OK};

    auto user_id = LoginRequest::getUserByToken(req->getCookie("token"), ret);

    if(ret.code == drogon::k200OK) {
        Json::Value json;

        for(auto& row : DB::get()->Select<Token>(std::format("user == {}", user_id))) {
            json.append(DTO::ToJson(row.second));
        }

        response = HttpResponse::newHttpJsonResponse(json);
    } else {
        response = HttpResponse::newHttpJsonResponse(DTO::ToJson(ret));
    }

    response->setStatusCode(ret.code);
    callback(response);
}
