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
class Log : public drogon::HttpController<API::Log>
{
public:
    //Connect methods to URL
    METHOD_LIST_BEGIN
        METHOD_ADD(Log::GetList, "", drogon::Get);
        METHOD_ADD(Log::Restore, "/{1:id_log}/restore", drogon::Post);
    METHOD_LIST_END

    //Declaring methods
    void GetList(const HttpRequestPtr& req, callback_func &&callback);
    void Restore(const HttpRequestPtr& req, callback_func &&callback,
                 id_t id_log);
};
}
