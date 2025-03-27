#include "Director.h"

#include "System/database.hpp"
#include "Request/register.hpp"
#include "Request/login.hpp"


void Director::Signin(const HttpRequestPtr& req, callback_func &&callback)
{
    HttpViewData data;

    // If it's a simple page opening
    if(!req->getBody().size())
    {
        data.insert("message_visibility", "\"visibility: hidden;\"");
        data.insert("message_txt", "NULL");

        auto response = HttpResponse::newHttpViewResponse("signin.csp", data);
        callback(response);

        return;
    }

    // Convert request body to LoginDTO and login
    auto info = DTO::CreateFromRequestBody<LoginDTO>(req->getBody());
    auto login = Request::Login(info, req, callback);

    // If there is any error
    if(!login.id)
    {
        data.insert("message_visibility", "\"visibility: visible;\"");
        data.insert("message_txt", login.message);

        auto response = HttpResponse::newHttpViewResponse("signin.csp", data);
        response->setStatusCode(drogon::k415UnsupportedMediaType);
        callback(response);

        return;
    }

    // If everything is fine
    // Create token cookie
    Cookie cookie("token", login.message);
    cookie.setPath("/");

    // Create response
    auto response = HttpResponse::newRedirectionResponse("/");
    response->addCookie(cookie);

    callback(response);

}

void Director::Signup(const HttpRequestPtr& req, callback_func &&callback)
{
    HttpViewData data;

    // If it's a simple page opening
    if(!req->getBody().size())
    {
        data.insert("message_visibility", "\"visibility: hidden;\"");
        data.insert("message_txt", "NULL");

        auto response = HttpResponse::newHttpViewResponse("signup.csp", data);
        callback(response);

        return;
    }

    auto reg = Request::Register(req, callback);

    // If there is any error
    if(reg.message.size())
    {
        data.insert("message_visibility", "\"visibility: visible;\"");
        data.insert("message_txt", reg.message);

        auto response = HttpResponse::newHttpViewResponse("signup.csp", data);
        response->setStatusCode(drogon::k415UnsupportedMediaType);
        callback(response);

        return;
    }

    auto response = HttpResponse::newRedirectionResponse("/");
    callback(response);
}

void Director::Gellery(const HttpRequestPtr& req, callback_func &&callback)
{
    HttpViewData data;
    data.insert("message_visibility", "\"visibility: hidden;\"");
    data.insert("images", DB::get()->Select<::Image>());

    auto response = HttpResponse::newHttpViewResponse("gallery.csp", data);
    callback(response);
}


void Director::Reviews(const HttpRequestPtr& req, callback_func &&callback, size_t count)
{
    HttpViewData data;

    auto login = Request::Login(req, callback);
    data.insert("is_login", login.id != 0);
    data.insert("count", ((!count)? 5 : count));
    data.insert("reviews", DB::get()->Select<::Review>());



    auto response = HttpResponse::newHttpViewResponse("reviews.csp", data);
    callback(response);
}

void Director::ReviewsPut(const HttpRequestPtr& req, callback_func &&callback, size_t count)
{
    auto login = Request::Login(req, callback);
    if(!login.id) { return; }

    // Convert request body to LoginDTO and login
    auto review = DTO::CreateFromRequestBody<::Review>(req->getBody());
    review.user_id = login.id;
    review.time = std::chrono::system_clock::now();

    DB::get()->Insert(review);

    // std::cout << count << std::endl;

    auto response = HttpResponse::newRedirectionResponse(
                std::format("{}?count={}", req->getPath(), ((!count)? 5 : count)));
    callback(response);
}
