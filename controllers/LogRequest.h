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
namespace Log {
class Request : public drogon::HttpController<API::Ref::Log::Request>
{
private:
    static void logger(const HttpRequestPtr& req, const HttpResponsePtr& resp);
public:
    //Connect methods to URL
    METHOD_LIST_BEGIN
        drogon::app().registerPostHandlingAdvice(Request::logger);
        METHOD_ADD(Request::Get, "/{1:id}", drogon::Get);
    METHOD_LIST_END
            
    //Declaring methods
    void Get(const HttpRequestPtr& req, callback_func&& callback, id_t id);
    void GetList(const HttpRequestPtr& req, callback_func&& callback);
};
}
}
}