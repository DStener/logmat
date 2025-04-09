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

class Hooks : public drogon::HttpController<Hooks>
{
public:
    //Connect methods to URL
    METHOD_LIST_BEGIN
        METHOD_ADD(Hooks::GitUpdate, "/git", drogon::Post);
    METHOD_LIST_END

    //Declaring methods
    void GitUpdate(const HttpRequestPtr& req, callback_func &&callback);
};

