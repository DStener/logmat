#include "Permission.h"

#include "Request/login.hpp"
#include "Request/log.hpp"

using namespace API;
using namespace API::Ref;


void Policy::Permission::GetList(const HttpRequestPtr& req, callback_func &&callback)
{
    // Login and check Permission
    auto login = Request::Login(req, callback);
    if(!login.id || !login.hasPermission<::Permission>("get-list")) { return; }

    Json::Value json;
    for(auto& row : DB::get()->Select<::Permission>())
    {
        CHECK_SOFT_DELETE(row) { continue; }
        json.append(DTO::JSON::From(row));
    }

    if(!json.size()) { json = "Нет записей"; }

    auto response = HttpResponse::newHttpJsonResponse(json);
    response->setStatusCode(drogon::k200OK);
    callback(response);
}

void Policy::Permission::Get(const HttpRequestPtr& req, callback_func &&callback,
         id_t id)
{
    // Login and check Permission
    auto login = Request::Login(req, callback);
    if(!login.id || !login.hasPermission<::Permission>("read")) { return; }

    Json::Value json;
    json = DTO::JSON::From(DB::get()->Select<::Permission>(std::format("id == {}", id))[0]);

    auto response = HttpResponse::newHttpJsonResponse(json);
    response->setStatusCode(drogon::k200OK);
    callback(response);
}

void Policy::Permission::Story(const HttpRequestPtr& req, callback_func &&callback,
            id_t id_perm)
{
    // Login and check Permission
    auto login = Request::Login(req, callback);
    if(!login.id || !login.hasPermission<::Permission>("get-story")) { return; }

    Json::Value json;
    for(auto& log : Request::Log::Get<::Permission>(id_perm))
    {
        json.append(Request::Log::GetJSONComplianceRules(log));
    }

    auto response = HttpResponse::newHttpJsonResponse(json);
    response->setStatusCode(drogon::k200OK);
    callback(response);
}

void Policy::Permission::Create(const HttpRequestPtr& req, callback_func &&callback)
{
    // Login and check Permission
    auto login = Request::Login(req, callback);
    if(!login.id || !login.hasPermission<::Permission>("create")) { return; }

    auto permission = DTO::ConvertTo<::Permission>(req);
    permission.created_by = login.id;
    permission.created_at = std::chrono::system_clock::now();
    permission.deleted_by = 0;
    permission.deleted_at = time_p();

    // Add Permission to DB
    DB::get()->Insert(permission);

    auto response = HttpResponse::newHttpResponse();
    response->setStatusCode(drogon::k200OK);
    callback(response);
}



void Policy::Permission::Update(const HttpRequestPtr& req, callback_func &&callback,
                                id_t id)
{
    // Login and check Permission
    auto login = Request::Login(req, callback);
    if(!login.id || !login.hasPermission<::Permission>("update")) { return; }

    auto permission = DTO::ConvertTo<::Permission>(req);
    permission.created_by = 0;
    permission.created_at = time_p();
    permission.deleted_by = 0;
    permission.deleted_at = time_p();

    DB::get()->Update<::Permission>(id, permission);

    auto response = HttpResponse::newHttpResponse();
    response->setStatusCode(drogon::k200OK);
    callback(response);
}

void Policy::Permission::Delete(const HttpRequestPtr& req, callback_func &&callback,
                                id_t id)
{
    // Login and check Permission
    auto login = Request::Login(req, callback);
    if(!login.id || !login.hasPermission<::Permission>("delete")) { return; }

    DB::get()->Delete<::Permission>(id);

    auto response = HttpResponse::newHttpResponse();
    response->setStatusCode(drogon::k200OK);
    callback(response);
}

void Policy::Permission::SoftDelete(const HttpRequestPtr& req, callback_func &&callback,
                id_t id)
{
    // Login and check Permission
    auto login = Request::Login(req, callback);
    if(!login.id || !login.hasPermission<::Permission>("delete")) { return; }

    auto permission = DB::get()->Select<::Permission>(std::format("id == {}",
                                                      login.id))[0].second;
    permission.deleted_at = std::chrono::system_clock::now();
    permission.deleted_by = login.id;

    DB::get()->Update(id, permission);

    auto response = HttpResponse::newHttpResponse();
    response->setStatusCode(drogon::k200OK);
    callback(response);
}

void Policy::Permission::Restore(const HttpRequestPtr& req, callback_func &&callback,
             id_t id)
{
    // Login and check Permission
    auto login = Request::Login(req, callback);
    if(!login.id || !login.hasPermission<::Permission>("restore")) { return; }

    auto permission = DB::get()->Select<::Permission>(std::format("id == {}", id))[0].second;
    permission.deleted_at = time_p();
    permission.deleted_by = 0;

    DB::get()->Update(id, permission);

    auto response = HttpResponse::newHttpResponse();
    response->setStatusCode(drogon::k200OK);
    callback(response);
}

void Policy::Permission::Form(const HttpRequestPtr& req, callback_func &&callback)
{
    Json::Value form;

    Json::Value name;
    Json::Value description;
    Json::Value code;

    name["type"] = "text";
    name["required"] = "required";

    description["type"] = "text" ;
    description["required"] = "required";

    code["type"] = "text";
    code[""] = "required";

    form["name"] = name;
    form["description"] = description;
    form["code"] = code;


    auto response = HttpResponse::newHttpJsonResponse(form);
    response->setStatusCode(drogon::k200OK);
    callback(response);
}

