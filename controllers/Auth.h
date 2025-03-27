#pragma once

#include <drogon/HttpController.h>
#include <drogon/HttpController.h>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <drogon/HttpTypes.h>

#include "System/DTO.hpp"
#include "System/database.hpp"

using namespace drogon;

using callback_func = std::function<void (const HttpResponsePtr &)>;

namespace API {
class Auth : public drogon::HttpController<API::Auth>
{
public:
    //Connect methods to URL
    METHOD_LIST_BEGIN
        // METHOD_ADD
        METHOD_ADD(Auth::Login, "/login", Post); // open
        METHOD_ADD(Auth::Register, "/register", Post); // no authorized
        METHOD_ADD(Auth::Info, "/me", Get); // authorized
        METHOD_ADD(Auth::Logout, "/out", Post); // authorized
        METHOD_ADD(Auth::getTokens, "/tokens", Get); // authorized
        METHOD_ADD(Auth::LogoutAll, "/out_all", Post); // authorized
        METHOD_ADD(Auth::Switch2FA, "/2fa/switch", Post); // authorized
        METHOD_ADD(Auth::Get2FAKey, "/2fa/key", Get); // authorized
    METHOD_LIST_END

    //Declaring methods
    void Login(const HttpRequestPtr& req, callback_func &&callback);
    void Register(const HttpRequestPtr& req, callback_func &&callback);
    void Info(const HttpRequestPtr& req, callback_func &&callback);
    void Logout(const HttpRequestPtr& req, callback_func &&callback);
    void LogoutAll(const HttpRequestPtr& req, callback_func &&callback);
    void getTokens(const HttpRequestPtr& req, callback_func &&callback);
    void Switch2FA(const HttpRequestPtr& req, callback_func &&callback);
    void Get2FAKey(const HttpRequestPtr& req, callback_func &&callback);

    // token
};
}
