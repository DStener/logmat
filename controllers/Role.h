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
namespace Policy {
class Role : public drogon::HttpController<API::Ref::Policy::Role>
{
public:
    METHOD_LIST_BEGIN
        METHOD_ADD(Role::GetList, "", drogon::Get);
        METHOD_ADD(Role::Get, "/{1:id}", drogon::Get);
        METHOD_ADD(Role::Story, "/{1:id_role}/story", drogon::Get);
        METHOD_ADD(Role::Create, "/", drogon::Post);
        METHOD_ADD(Role::Update, "/{1:id}", drogon::Put);
        METHOD_ADD(Role::Delete, "/{1:id}", drogon::Delete);
        METHOD_ADD(Role::SoftDelete, "/{1:id}/soft", drogon::Delete);
        METHOD_ADD(Role::Restore, "/{1:id}/restore", drogon::Post);

        METHOD_ADD(Role::Permissions, "/{1:id_role}/permission", drogon::Get);
        METHOD_ADD(Role::Set, "/{1:id_role}/permission", drogon::Post);
        METHOD_ADD(Role::PermDelete, "/{1:id_role}/permission/{2:id_perm}", drogon::Delete);
        METHOD_ADD(Role::PermSoftDelete, "/{1:id_role}/permission/{2:id_perm}/soft", drogon::Delete);
        METHOD_ADD(Role::PermRestore, "/{1:id_role}/permission/{2:id_perm}/restore", drogon::Post);
    METHOD_LIST_END

    //Declaring methods


    void Get(const HttpRequestPtr& req, callback_func &&callback,
              id_t id);
    void Story(const HttpRequestPtr& req, callback_func &&callback,
                id_t id_role);
    void GetList(const HttpRequestPtr& req, callback_func &&callback);

    void Create(const HttpRequestPtr& req, callback_func &&callback);
    void Update(const HttpRequestPtr& req, callback_func &&callback,
                id_t id);

    void Delete(const HttpRequestPtr& req, callback_func &&callback,
                id_t id);
    void SoftDelete(const HttpRequestPtr& req, callback_func &&callback,
                    id_t id);

    void Restore(const HttpRequestPtr& req, callback_func &&callback,
                 id_t id);



    void Permissions(const HttpRequestPtr& req, callback_func &&callback,
                     id_t id_role);
    void Set(const HttpRequestPtr& req, callback_func &&callback,
                     id_t id_role);
    void PermDelete(const HttpRequestPtr& req, callback_func &&callback,
                    id_t id_role, id_t id_perm);
    void PermSoftDelete(const HttpRequestPtr& req, callback_func &&callback,
                    id_t id_role, id_t id_perm);
    void PermRestore(const HttpRequestPtr& req, callback_func &&callback,
                    id_t id_role, id_t id_perm);
};
}
}
}
