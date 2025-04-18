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
class Article : public drogon::HttpController<API::Ref::Article>
{
public:
    // Connect methods to URL
    METHOD_LIST_BEGIN
        METHOD_ADD(Article::GetList, "", drogon::Get);
        METHOD_ADD(Article::Get, "/{1:id_file}", drogon::Get);
        METHOD_ADD(Article::GetInfo, "/{1:id_file}/info", drogon::Get);
        METHOD_ADD(Article::Upload, "/", drogon::Post);
        METHOD_ADD(Article::Update, "/{1:id_file}", drogon::Put, drogon::Post);
        METHOD_ADD(Article::Delete, "/{1:id_file}/delete", drogon::Delete, drogon::Post);
        METHOD_ADD(Article::SoftDelete, "/{1:id_file}/soft", drogon::Delete);
        METHOD_ADD(Article::Restore, "/{1:id_file}/restore", drogon::Post);
        METHOD_ADD(Article::Form, "/form", drogon::Get);
    METHOD_LIST_END

    // Declaring methods
    void GetList(const HttpRequestPtr& req, callback_func&& callback);
    void Get(const HttpRequestPtr& req, callback_func&& callback, id_t id_file);
    void GetInfo(const HttpRequestPtr& req, callback_func&& callback, id_t id_file);

    void Upload(const HttpRequestPtr& req, callback_func&& callback);
    void Update(const HttpRequestPtr& req, callback_func&& callback, id_t id_file);

    void Delete(const HttpRequestPtr& req, callback_func&& callback, id_t id_file);
    void SoftDelete(const HttpRequestPtr& req, callback_func&& callback, id_t id_file);
    void Restore(const HttpRequestPtr& req, callback_func&& callback, id_t id_file);
    void Form(const HttpRequestPtr& req, callback_func&& callback);
};
}
}