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
        //ADD_METHOD_TO(Director::Main, "/", drogon::Get);
        ADD_METHOD_TO(Director::Test, "/temp", drogon::Get, "Filter::Login");
        ADD_METHOD_TO(Director::AdminPanel, "/admin", drogon::Get, "Filter::Login");
        ADD_METHOD_TO(Director::Account, "/me", drogon::Get, "Filter::Login");
        ADD_METHOD_TO(Director::ArticleEditor, "/article/{1:id}/editor", drogon::Get, "Filter::Login");
        ADD_METHOD_TO(Director::Article, "/article/{1:id}", drogon::Get);
        ADD_METHOD_TO(Director::Version, "/info/version", drogon::Get);
    METHOD_LIST_END

    void Version(const HttpRequestPtr& req, callback_func&& callback);
    void Test(const HttpRequestPtr& req, callback_func&& callback);
    void Account(const HttpRequestPtr& req, callback_func&& callback);
    void AdminPanel(const HttpRequestPtr& req, callback_func&& callback);
    void ArticleEditor(const HttpRequestPtr& req, callback_func&& callback, id_t id);
    void Article(const HttpRequestPtr& req, callback_func&& callback, id_t id);
   
};

