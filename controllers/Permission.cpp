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
        json.append(DTO::ToJson(row));
    }

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
    json = DTO::ToJson(DB::get()->Select<::Permission>(std::format("id == {}", id))[0]);

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
        CHECK_SOFT_DELETE(log) { continue; }
        json.append(DTO::ToJson(log));
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

    auto permission = DTO::CreateFromRequestBody<::Permission>(req->getBody());
    permission.created_by.id = login.id;
    permission.created_at = std::chrono::system_clock::now();
    permission.deleted_by.id = 0;
    permission.deleted_at = time_p();

    // Сhecking that the fields with the NOTNULL attribute are filled in
    if(!DTO::SQL::CheckField::NotNull(permission))
    {
        auto response = HttpResponse::newHttpResponse();
        response->setStatusCode(drogon::k406NotAcceptable);
        callback(response);
        return;
    }

    // Сhecking that the fields with the attribute UNIQUE are unique
    auto result = DB::get()->Select<::Permission>(DTO::SQL::CheckField::UniqueSQL(permission));
    if(result.size() != 0)
    {
        Json::Value json;
        json["message"] = "Не уникальные данные";

        auto response = HttpResponse::newHttpJsonResponse(json);
        response->setStatusCode(drogon::k406NotAcceptable);
        callback(response);
        return;
    }

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

    auto permission = DTO::CreateFromRequestBody<::Permission>(req->getBody());
    permission.created_by.id = 0;
    permission.created_at = time_p();
    permission.deleted_by.id = 0;
    permission.deleted_at = time_p();

    auto result = DB::get()->Select<::Permission>(DTO::SQL::CheckField::UniqueSQL(permission));
    if(result.size() != 0)
    {
        Json::Value json;
        json["message"] = "Не уникальные данные";

        auto response = HttpResponse::newHttpJsonResponse(json);
        response->setStatusCode(drogon::k406NotAcceptable);
        callback(response);
        return;
    }

    // Update row Permission in DB
    DB::get()->Update<::Permission>(id, DTO::SQL::CheckField::UpdateNotNull(permission));

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

    auto set = std::format("deleted_at = \"{}\", deleted_by = {}",
                           std::chrono::system_clock::now(), login.id);
    DB::get()->Update<::Permission>(id, set);

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

    auto set = std::format("deleted_at = \"{}\", deleted_by = 0",
                           std::chrono::system_clock::time_point());
    DB::get()->Update<::Permission>(id, set);

    auto response = HttpResponse::newHttpResponse();
    response->setStatusCode(drogon::k200OK);
    callback(response);
}

