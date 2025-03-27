#include "User.h"
#include "System/utils.h"
#include "Request/login.hpp"
#include "Request/log.hpp"

using namespace API;

void Ref::User::GetList(const HttpRequestPtr& req, callback_func &&callback)
{
    // Login and check Permission
    auto login = Request::Login(req, callback);
    if(!login.id || !login.hasPermission<::User>("get-list")) { return; }

    Json::Value json;
    for(auto& row : DB::get()->Select<::User>())
    {
        json.append(DTO::ToJson(row));
    }

    auto response = HttpResponse::newHttpJsonResponse(json);
    response->setStatusCode(drogon::k200OK);
    callback(response);
}

void Ref::User::Story(const HttpRequestPtr& req, callback_func &&callback,
            id_t id_user)
{
    // Login and check Permission
    auto login = Request::Login(req, callback);
    if(!login.id || !login.hasPermission<::User>("get-story")) { return; }

    Json::Value json;
    for(auto& log : Request::Log::Get<::User>(id_user))
    {
        CHECK_SOFT_DELETE(log) { continue; }
        json.append(DTO::ToJson(log));
    }

    auto response = HttpResponse::newHttpJsonResponse(json);
    response->setStatusCode(drogon::k200OK);
    callback(response);
}

void Ref::User::Roles(const HttpRequestPtr& req, callback_func &&callback,
           id_t id)
{
    // Login and check Permission
    auto login = Request::Login(req, callback);
    if(!login.id || !login.hasPermission<::User>("get-list")) { return; }

    Json::Value json;
    auto condition =  std::format("user == {}", id);
    for(auto& user_role : DB::get()->Select<::UserAndRole>(condition))
    {
        CHECK_SOFT_DELETE(user_role) { continue; }
        auto condition = std::format("id == {}", user_role.second.role.id);
        for(auto& role : DB::get()->Select<::Role>(condition))
        {
            CHECK_SOFT_DELETE(role) { continue; }
            json.append(DTO::ToJson(role));
        }
    }

    auto response = HttpResponse::newHttpJsonResponse(json);
    response->setStatusCode(drogon::k200OK);
    callback(response);
}


void Ref::User::Set(const HttpRequestPtr& req, callback_func &&callback,
         id_t id)
{
    // Login and check Permission
    auto login = Request::Login(req, callback);
    if(!login.id || !login.hasPermission<::User>("create")) { return; }

    auto user_role = DTO::CreateFromRequestBody<::UserAndRole>(req->getBody());
    user_role.user = id;
    user_role.created_by.id = login.id;
    user_role.created_at = std::chrono::system_clock::now();
    user_role.deleted_by.id = 0;
    user_role.deleted_at = time_p();

    // Сhecking that the fields with the NOTNULL attribute are filled in
    if(!DTO::SQL::CheckField::NotNull(user_role))
    {
        auto response = HttpResponse::newHttpResponse();
        response->setStatusCode(drogon::k406NotAcceptable);
        callback(response);
        return;
    }

    // Сhecking that the fields with the attribute UNIQUE are unique
    auto result = DB::get()->Select<::UserAndRole>(DTO::SQL::CheckField::UniqueSQL(user_role));
    if(result.size() != 0)
    {
        Json::Value json;
        json["message"] = "Не уникальные данные";

        auto response = HttpResponse::newHttpJsonResponse(json);
        response->setStatusCode(drogon::k406NotAcceptable);
        callback(response);
        return;
    }

    // Add UserAndRole to DB
    DB::get()->Insert(user_role);

    auto response = HttpResponse::newHttpResponse();
    response->setStatusCode(drogon::k200OK);
    callback(response);
}


void Ref::User::Delete(const HttpRequestPtr& req, callback_func &&callback,
            id_t id_user, id_t id_role)
{
    // Login and check Permission
    auto login = Request::Login(req, callback);
    if(!login.id || !login.hasPermission<::User>("delete")) { return; }

    auto id = DB::get()->Select<::RoleAndPermission>(
        std::format("user == {} AND role == {}", id_user, id_role))[0].first;
    DB::get()->Delete<::UserAndRole>(id);

    auto response = HttpResponse::newHttpResponse();
    response->setStatusCode(drogon::k200OK);
    callback(response);
}


void Ref::User::SoftDelete(const HttpRequestPtr& req, callback_func &&callback,
                id_t id_user, id_t id_role)
{
    // Login and check Permission
    auto login = Request::Login(req, callback);
    if(!login.id || !login.hasPermission<::User>("delete")) { return; }

    auto id = DB::get()->Select<::RoleAndPermission>(
        std::format("user == {} AND role == {}", id_user, id_role))[0].first;

    auto set = std::format("deleted_at = \"{}\", deleted_by = {}",
                           std::chrono::system_clock::now(), login.id);
    DB::get()->Update<::UserAndRole>(id, set);

    auto response = HttpResponse::newHttpResponse();
    response->setStatusCode(drogon::k200OK);
    callback(response);
}



void Ref::User::Restore(const HttpRequestPtr& req, callback_func &&callback,
            id_t id_user, id_t id_role)
{
    // Login and check Permission
    auto login = Request::Login(req, callback);
    if(!login.id || !login.hasPermission<::User>("restore")) { return; }

    auto id = DB::get()->Select<::RoleAndPermission>(
        std::format("user == {} AND role == {}", id_user, id_role))[0].first;

    auto set = std::format("deleted_at = \"{}\", deleted_by = 0", time_p());
    DB::get()->Update<::UserAndRole>(id, set);

    auto response = HttpResponse::newHttpResponse();
    response->setStatusCode(drogon::k200OK);
    callback(response);
}
