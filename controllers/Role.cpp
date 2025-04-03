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
    json = DTO::JSON::From(DB::get()->Select<::Role>(std::format("id == {}", id))[0]);

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
        json.append(DTO::JSON::From(log));
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
        json.append(DTO::JSON::From(row));
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

    auto role = DTO::RequestBody::To<::Role>(req->getBody());
    role.created_by = 0;
    role.created_at = std::chrono::system_clock::now();
    role.deleted_by = 0;
    role.deleted_at = time_p();

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

    auto role = DTO::RequestBody::To<::Role>(req->getBody());
    role.created_by = 0;
    role.created_at = time_p();
    role.deleted_by = 0;
    role.deleted_at = time_p();

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

    auto role = DB::get()->Select<::Role>(std::format("id == {}",
                                                      login.id))[0].second;
    role.deleted_at = std::chrono::system_clock::now();
    role.deleted_by = login.id;

    DB::get()->Update(id, role);

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

    auto role = DB::get()->Select<::Role>(std::format("id == {}", id))[0].second;
    role.deleted_at = time_p();
    role.deleted_by = 0;

    DB::get()->Update(id, role);

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
        auto condition = std::format("id == {}", user_role.second.permission);
        for(auto& role : DB::get()->Select<::Permission>(condition))
        {
            CHECK_SOFT_DELETE(role) { continue; }
            json.append(DTO::JSON::From(role));
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

    auto role_perm = DTO::RequestBody::To<::RoleAndPermission>(req->getBody());
    role_perm.role = id_role;
    role_perm.created_by = login.id;
    role_perm.created_at = std::chrono::system_clock::now();
    role_perm.deleted_by = 0;
    role_perm.deleted_at = time_p();

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

    auto entity = DB::get()->Select<::RoleAndPermission>(
        std::format("permission == {} AND role == {}", id_perm, id_role))[0];
    entity.second.deleted_at = std::chrono::system_clock::now();
    entity.second.deleted_by = login.id;

    DB::get()->Update(entity.first, entity.second);

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

    auto entity = DB::get()->Select<::RoleAndPermission>(
        std::format("permission == {} AND role == {}", id_perm, id_role))[0];
    entity.second.deleted_at = time_p();
    entity.second.deleted_by = 0;

    DB::get()->Update(entity.first, entity.second);

    auto response = HttpResponse::newHttpResponse();
    response->setStatusCode(drogon::k200OK);
    callback(response);
}
