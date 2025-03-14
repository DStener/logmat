#pragma once

#include <drogon/HttpController.h>
#include "modules/database.h"

using namespace drogon;

using callback_func = std::function<void (const HttpResponsePtr &)>;

class Test : public drogon::HttpController<Test>
{
  public:

    // Binding the URL of the path to the method
    METHOD_LIST_BEGIN
    METHOD_ADD(Test::TestDB, "/database", Get);
    METHOD_LIST_END

    // Declaring methods
    void TestDB(const HttpRequestPtr& req, callback_func &&callback);

};
