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

class Director : public drogon::HttpController<Director>
{
public:
    //Connect methods to URL
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(Director::Test, "/temp", drogon::Get);
        ADD_METHOD_TO(Director::Account, "/me", drogon::Get);
    METHOD_LIST_END

    void Test(const HttpRequestPtr& req, callback_func&& callback);
    void Account(const HttpRequestPtr& req, callback_func&& callback);
   
};

