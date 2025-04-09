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
        json.append(DTO::JSON::From(row));
    }

    if(!json.size()) { json = "Нет записей"; }

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
        Request::Log::GetJSONComplianceRules(log);
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
        auto condition = std::format("id == {}", user_role.second.role);
        for(auto& role : DB::get()->Select<::Role>(condition))
        {
            CHECK_SOFT_DELETE(role) { continue; }
            json.append(DTO::JSON::From(role));
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

    auto user_role = DTO::ConvertTo<::UserAndRole>(req);
    user_role.user = id;
    user_role.created_by = login.id;
    user_role.created_at = std::chrono::system_clock::now();
    user_role.deleted_by = 0;
    user_role.deleted_at = time_p();

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

    auto entity = DB::get()->Select<::RoleAndPermission>(
        std::format("user == {} AND role == {}", id_user, id_role))[0];
    entity.second.deleted_at = std::chrono::system_clock::now();
    entity.second.deleted_by = login.id;

    DB::get()->Update(entity.first, entity.second);

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

    auto entity = DB::get()->Select<::RoleAndPermission>(
        std::format("user == {} AND role == {}", id_user, id_role))[0];
    entity.second.deleted_at = time_p();
    entity.second.deleted_by = 0;

    DB::get()->Update(entity.first, entity.second);

    auto response = HttpResponse::newHttpResponse();
    response->setStatusCode(drogon::k200OK);
    callback(response);
}

void Ref::User::Update(const HttpRequestPtr& req, callback_func&& callback,
    id_t id_user)
{
    // Login and check Permission
    auto login = Request::Login(req, callback);
    if (!login.id) { return; }

    auto user = DTO::ConvertTo<::User>(req);
    user.password = "";
    user.time2FA = time_p();

    DB::get()->Update<::User>(login.id, user);

    auto response = HttpResponse::newHttpResponse();
    auto redirect = req->getOptionalParameter<std::string>("redirect");

    if (redirect.has_value())
    {
        response = HttpResponse::newRedirectionResponse(redirect.value());
    }
    callback(response);
}