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
namespace Ref {
class User : public drogon::HttpController<API::Ref::User>
{
public:
    //Connect methods to URL
    METHOD_LIST_BEGIN
        METHOD_ADD(User::GetList, "", drogon::Get);
        METHOD_ADD(User::Update, "/{1:id_user}/update", drogon::Put, drogon::Post);
        METHOD_ADD(User::Story, "/{1:id_user}/story", drogon::Get);
        METHOD_ADD(User::Roles, "/{1:id}/role", drogon::Get);
        METHOD_ADD(User::Set, "/{1:id}/role", drogon::Post);
        METHOD_ADD(User::Delete, "/{1:id_user}/role/{2:id_role}", drogon::Delete);
        METHOD_ADD(User::SoftDelete, "/{1:id_user}/role/{2:id_role}/soft", drogon::Delete);
        METHOD_ADD(User::Restore, "/{1:id_user}/role/{2:id_role}/restore", drogon::Post);
    METHOD_LIST_END

    //Declaring methods
    void GetList(const HttpRequestPtr& req, callback_func &&callback);

    void Story(const HttpRequestPtr& req, callback_func &&callback,
                id_t id_user);
    void Update(const HttpRequestPtr& req, callback_func&& callback,
                id_t id_user);

    void Roles(const HttpRequestPtr& req, callback_func &&callback,
                id_t id);

    void Set(const HttpRequestPtr& req, callback_func &&callback,
                id_t id);
    void Delete(const HttpRequestPtr& req, callback_func &&callback,
                id_t id_user, id_t id_role);
    void SoftDelete(const HttpRequestPtr& req, callback_func &&callback,
                id_t id_user, id_t id_role);
    void Restore(const HttpRequestPtr& req, callback_func &&callback,
                id_t id_user, id_t id_role);
};
}
}
