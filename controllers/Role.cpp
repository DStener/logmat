#include "Role.h"

#include "Request/login.hpp"
#include "Request/log.hpp"

using namespace API;
using namespace API::Ref;

void Policy::Role::Get(const HttpRequestPtr& req, callback_func &&callback,
          id_t id)
{
    // Login and check Permission
    auto login = Request::Login(req, callback);
    if(!login.id || !login.hasPermission<::Role>("read")) { return; }

    Json::Value json;
    json = DTO::ToJson(DB::get()->Select<::Role>(std::format("id == {}", id))[0]);

    auto response = HttpResponse::newHttpJsonResponse(json);
    response->setStatusCode(drogon::k200OK);
    callback(response);
}

void Policy::Role::Story(const HttpRequestPtr& req, callback_func &&callback,
            id_t id_role)
{
    // Login and check Permission
    auto login = Request::Login(req, callback);
    if(!login.id || !login.hasPermission<::Role>("get-story")) { return; }

    Json::Value json;
    for(auto& log : Request::Log::Get<::Role>(id_role))
    {
        CHECK_SOFT_DELETE(log) { continue; }
        json.append(DTO::ToJson(log));
    }

    auto response = HttpResponse::newHttpJsonResponse(json);
    response->setStatusCode(drogon::k200OK);
    callback(response);
}


void Policy::Role::GetList(const HttpRequestPtr& req, callback_func &&callback)
{
    // Login and check Permission
    auto login = Request::Login(req, callback);
    if(!login.id || !login.hasPermission<::Role>("get-list")) { return; }

    Json::Value json;
    for(auto& row : DB::get()->Select<::Role>())
    {
        CHECK_SOFT_DELETE(row) { continue; }
        json.append(DTO::ToJson(row));
    }

    auto response = HttpResponse::newHttpJsonResponse(json);
    response->setStatusCode(drogon::k200OK);
    callback(response);
}



void Policy::Role::Create(const HttpRequestPtr& req, callback_func &&callback)
{
    // Login and check Permission
    auto login = Request::Login(req, callback);
    if(!login.id || !login.hasPermission<::Role>("create")) { return; }

    auto role = DTO::CreateFromRequestBody<::Role>(req->getBody());
    role.created_by.id = 0;
    role.created_at = std::chrono::system_clock::now();
    role.deleted_by.id = 0;
    role.deleted_at = time_p();

    // Сhecking that the fields with the NOTNULL attribute are filled in
    if(!DTO::SQL::CheckField::NotNull(role))
    {
        auto response = HttpResponse::newHttpResponse();
        response->setStatusCode(drogon::k406NotAcceptable);
        callback(response);
        return;
    }

    // Сhecking that the fields with the attribute UNIQUE are unique
    auto result = DB::get()->Select<::Role>(DTO::SQL::CheckField::UniqueSQL(role));
    if(result.size() != 0)
    {
        Json::Value json;
        json["message"] = "Не уникальные данные";

        auto response = HttpResponse::newHttpJsonResponse(json);
        response->setStatusCode(drogon::k406NotAcceptable);
        callback(response);
        return;
    }

    // Add Role to DB
    DB::get()->Insert(role);

    auto response = HttpResponse::newHttpResponse();
    response->setStatusCode(drogon::k200OK);
    callback(response);
}

void Policy::Role::Update(const HttpRequestPtr& req, callback_func &&callback,
            id_t id)
{
    // Login and check Permission
    auto login = Request::Login(req, callback);
    if(!login.id || !login.hasPermission<::Role>("update")) { return; }

    auto role = DTO::CreateFromRequestBody<::Role>(req->getBody());
    role.created_by.id = 0;
    role.created_at = time_p();
    role.deleted_by.id = 0;
    role.deleted_at = time_p();

    auto result = DB::get()->Select<::Role>(DTO::SQL::CheckField::UniqueSQL(role));
    if(result.size() != 0)
    {
        Json::Value json;
        json["message"] = "Не уникальные данные";

        auto response = HttpResponse::newHttpJsonResponse(json);
        response->setStatusCode(drogon::k406NotAcceptable);
        callback(response);
        return;
    }

    // Update row Role in DB
    DB::get()->Update<::Role>(id, DTO::SQL::CheckField::UpdateNotNull(role));

    auto response = HttpResponse::newHttpResponse();
    response->setStatusCode(drogon::k200OK);
    callback(response);
}


void Policy::Role::Delete(const HttpRequestPtr& req, callback_func &&callback,
                id_t id)
{
    // Login and check Permission
    auto login = Request::Login(req, callback);
    if(!login.id || !login.hasPermission<::Role>("delete")) { return; }

    DB::get()->Delete<::Role>(id);

    auto response = HttpResponse::newHttpResponse();
    response->setStatusCode(drogon::k200OK);
    callback(response);
}


void Policy::Role::SoftDelete(const HttpRequestPtr& req, callback_func &&callback,
                id_t id)
{
    // Login and check Permission
    auto login = Request::Login(req, callback);
    if(!login.id || !login.hasPermission<::Role>("delete")) { return; }

    auto set = std::format("deleted_at = \"{}\", deleted_by = {}",
                           std::chrono::system_clock::now(), login.id);
    DB::get()->Update<::Role>(id, set);

    auto response = HttpResponse::newHttpResponse();
    response->setStatusCode(drogon::k200OK);
    callback(response);
}

void Policy::Role::Restore(const HttpRequestPtr& req, callback_func &&callback,
             id_t id)
{
    // Login and check Permission
    auto login = Request::Login(req, callback);
    if(!login.id || !login.hasPermission<::Role>("restore")) { return; }

    auto set = std::format("deleted_at = \"{}\", deleted_by = 0",
                           std::chrono::system_clock::time_point());
    DB::get()->Update<::Role>(id, set);

    auto response = HttpResponse::newHttpResponse();
    response->setStatusCode(drogon::k200OK);
    callback(response);
}


////////////////////////////////////////////////////////////////////////////////



void  Policy::Role::Permissions(const HttpRequestPtr& req, callback_func &&callback,
                               id_t id_role)
{
    // Login and check Permission
    auto login = Request::Login(req, callback);
    if(!login.id || !login.hasPermission<::Role>("get-list")) { return; }

    Json::Value json;
    auto condition =  std::format("role == {}", id_role);
    for(auto& user_role : DB::get()->Select<::RoleAndPermission>(condition))
    {
        CHECK_SOFT_DELETE(user_role) { continue; }
        auto condition = std::format("id == {}", user_role.second.permission.id);
        for(auto& role : DB::get()->Select<::Permission>(condition))
        {
            CHECK_SOFT_DELETE(role) { continue; }
            json.append(DTO::ToJson(role));
        }
    }

    auto response = HttpResponse::newHttpJsonResponse(json);
    response->setStatusCode(drogon::k200OK);
    callback(response);
}

void Policy::Role::Set(const HttpRequestPtr& req, callback_func &&callback,
                       id_t id_role)
{
    // Login and check Permission
    auto login = Request::Login(req, callback);
    if(!login.id || !login.hasPermission<::Role>("create")) { return; }

    auto role_perm = DTO::CreateFromRequestBody<::RoleAndPermission>(req->getBody());
    role_perm.role = id_role;
    role_perm.created_by.id = login.id;
    role_perm.created_at = std::chrono::system_clock::now();
    role_perm.deleted_by.id = 0;
    role_perm.deleted_at = time_p();

    // Сhecking that the fields with the NOTNULL attribute are filled in
    if(!DTO::SQL::CheckField::NotNull(role_perm))
    {
        auto response = HttpResponse::newHttpResponse();
        response->setStatusCode(drogon::k406NotAcceptable);
        callback(response);
        return;
    }

    // Сhecking that the fields with the attribute UNIQUE are unique
    auto result = DB::get()->Select<::RoleAndPermission>(DTO::SQL::CheckField::UniqueSQL(role_perm));
    if(result.size() != 0)
    {
        Json::Value json;
        json["message"] = "Не уникальные данные";

        auto response = HttpResponse::newHttpJsonResponse(json);
        response->setStatusCode(drogon::k406NotAcceptable);
        callback(response);
        return;
    }

    // Add RoleAndPermission to DB
    DB::get()->Insert(role_perm);

    auto response = HttpResponse::newHttpResponse();
    response->setStatusCode(drogon::k200OK);
    callback(response);
}

void Policy::Role::PermDelete(const HttpRequestPtr& req, callback_func &&callback,
                              id_t id_role, id_t id_perm)
{
    // Login and check Permission
    auto login = Request::Login(req, callback);
    if(!login.id || !login.hasPermission<::Role>("delete")) { return; }

    auto id = DB::get()->Select<::RoleAndPermission>(
        std::format("permission == {} AND role == {}", id_perm, id_role))[0].first;
    DB::get()->Delete<::RoleAndPermission>(id);

    auto response = HttpResponse::newHttpResponse();
    response->setStatusCode(drogon::k200OK);
    callback(response);
}


void Policy::Role::PermSoftDelete(const HttpRequestPtr& req, callback_func &&callback,
                                  id_t id_role, id_t id_perm)
{
    // Login and check Permission
    auto login = Request::Login(req, callback);
    if(!login.id || !login.hasPermission<::Role>("delete")) { return; }

    auto id = DB::get()->Select<::RoleAndPermission>(
        std::format("permission == {} AND role == {}", id_perm, id_role))[0].first;

    auto set = std::format("deleted_at = \"{}\", deleted_by = {}",
                           std::chrono::system_clock::now(), login.id);
    DB::get()->Update<::RoleAndPermission>(id, set);


    auto response = HttpResponse::newHttpResponse();
    response->setStatusCode(drogon::k200OK);
    callback(response);
}


void Policy::Role::PermRestore(const HttpRequestPtr& req, callback_func &&callback,
                               id_t id_role, id_t id_perm)
{
    // Login and check Permission
    auto login = Request::Login(req, callback);
    if(!login.id || !login.hasPermission<::Role>("restore")) { return; }

    auto id = DB::get()->Select<::RoleAndPermission>(
        std::format("permission == {} AND role == {}", id_perm, id_role))[0].first;

    auto set = std::format("deleted_at = \"{}\", deleted_by = 0",
                           time_p());
    DB::get()->Update<::RoleAndPermission>(id, set);

    auto response = HttpResponse::newHttpResponse();
    response->setStatusCode(drogon::k200OK);
    callback(response);
}
