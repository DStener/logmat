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
class Permission : public drogon::HttpController<API::Ref::Policy::Permission>
{
public:
    //Connect methods to URL
    METHOD_LIST_BEGIN
        METHOD_ADD(Permission::GetList, "", drogon::Get);
        METHOD_ADD(Permission::Get, "/{1:id}", drogon::Get);
        METHOD_ADD(Permission::Story, "/{1:id_perm}/story", drogon::Get);
        METHOD_ADD(Permission::Create, "/", drogon::Post);
        METHOD_ADD(Permission::Update, "/{1:id}", drogon::Put);
        METHOD_ADD(Permission::Delete, "/{1:id}", drogon::Delete);
        METHOD_ADD(Permission::SoftDelete, "/{1:id}/soft", drogon::Delete);
        METHOD_ADD(Permission::Restore, "/{1:id}/restore", drogon::Post);
        METHOD_ADD(Permission::Form, "/form", drogon::Get);
    METHOD_LIST_END

    //Declaring methods
    void GetList(const HttpRequestPtr& req, callback_func &&callback);
    void Story(const HttpRequestPtr& req, callback_func &&callback,
                id_t id_perm);
    void Get(const HttpRequestPtr& req, callback_func &&callback,
             id_t id);

    void Create(const HttpRequestPtr& req, callback_func &&callback);
    void Update(const HttpRequestPtr& req, callback_func &&callback,
             id_t id);

    void Delete(const HttpRequestPtr& req, callback_func &&callback,
                id_t id);
    void SoftDelete(const HttpRequestPtr& req, callback_func &&callback,
                id_t id);
    void Restore(const HttpRequestPtr& req, callback_func &&callback,
                id_t id);
    void Form(const HttpRequestPtr& req, callback_func &&callback);
};
}
}
}



