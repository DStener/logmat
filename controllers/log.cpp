#include "log.h"

#include "Request/login.hpp"
#include "Request/log.hpp"


void API::Log::GetList(const HttpRequestPtr& req, callback_func &&callback)
{
    Json::Value json;

    for (auto& row : DB::get()->Select<::ChangeLog>())
    {
        json.append(Request::Log::GetJSONComplianceRules(row));
    }

    if(!json.size()) { json = "Нет записей"; }

    auto response = HttpResponse::newHttpJsonResponse(json);
    callback(response);
}

void API::Log::Restore(const HttpRequestPtr& req, callback_func &&callback,
                       id_t id_log)
{
    // Login and check Permission
    auto login = Request::Login(req, callback);
    if(!login.id || !login.hasPermission<::User>("get-story")) { return; }

    if(Request::Log::Restore(id_log))
    {
        auto response = HttpResponse::newHttpResponse();
        response->setStatusCode(drogon::k200OK);
        callback(response);
    }
    else
    {
        Json::Value json("Невозможно восстановить row");

        auto response = HttpResponse::newHttpJsonResponse(json);
        response->setStatusCode(drogon::k406NotAcceptable);
        callback(response);
    }
}
