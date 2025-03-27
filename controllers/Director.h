#pragma once

#include <drogon/HttpController.h>
#include <drogon/HttpController.h>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <drogon/HttpTypes.h>

using namespace drogon;

using callback_func = std::function<void (const HttpResponsePtr &)>;

class Director : public drogon::HttpController<Director>
{
public:
    //Connect methods to URL
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(Director::Signin, "/signin", Get, Post);
        ADD_METHOD_TO(Director::Signup, "/signup", Get, Post);
        ADD_METHOD_TO(Director::Gellery, "/gallery", Get);
        ADD_METHOD_TO(Director::Reviews, "/reviews?count={1:count}", Get);
        ADD_METHOD_TO(Director::ReviewsPut, "/reviews?count={1:count}", Post);
    METHOD_LIST_END

    //Declaring methods
    void Signin(const HttpRequestPtr& req, callback_func &&callback);
    void Signup(const HttpRequestPtr& req, callback_func &&callback);
    void Gellery(const HttpRequestPtr& req, callback_func &&callback);
    void Reviews(const HttpRequestPtr& req, callback_func &&callback, size_t count);
    void ReviewsPut(const HttpRequestPtr& req, callback_func &&callback, size_t count);
};

