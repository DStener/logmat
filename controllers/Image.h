#pragma once

#include <drogon/HttpController.h>
#include <drogon/HttpController.h>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <drogon/HttpTypes.h>

using namespace drogon;

using callback_func = std::function<void (const HttpResponsePtr &)>;

namespace Data {
class Image : public drogon::HttpController<Data::Image>
{
public:
    //Connect methods to URL
    METHOD_LIST_BEGIN
        METHOD_ADD(Image::GetList, "/", drogon::Get);
        METHOD_ADD(Image::Get, "/{1:id_image}", drogon::Get);
        // METHOD_ADD(Image::Signup, "/signup", Get);
    METHOD_LIST_END

    //Declaring methods
    void GetList(const HttpRequestPtr& req, callback_func &&callback);
    void Get(const HttpRequestPtr& req, callback_func &&callback,
             id_t id_image);
    // void Signup(const HttpRequestPtr& req, callback_func &&callback);
};
} // data
