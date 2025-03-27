#include "Director.h"

#include "System/database.hpp"
#include "Request/register.hpp"
#include "Request/login.hpp"
#include "System/utils.h"


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

    // Generate new Captcha
    auto captcha = API::Utils::generateCaptcha();
    ::Captcha capt{};
    capt.uid = std::time(nullptr);
    capt.answer = captcha.first;


    DB::get()->Insert(capt);

    // Create captcha cookie
    Cookie cookie_capt("captcha", std::format("{}", capt.uid));
    cookie_capt.setPath("/");



    // If it's a simple page opening
    if(!req->getBody().size())
    {
        data.insert("message_visibility", "\"visibility: hidden;\"");
        data.insert("captcha", captcha.second);
        data.insert("message_txt", "NULL");

        auto response = HttpResponse::newHttpViewResponse("signup.csp", data);
        response->addCookie(cookie_capt);
        callback(response);

        return;
    }

    // Check Captcha
    auto info = DTO::CreateFromRequestBody<::Captcha>(req->getBody());
    auto capts =\
        DB::get()->Select<::Captcha>(std::format("uid == \"{}\"", req->getCookie("captcha")));

    if(!capts.size() || capts[0].second.answer != info.answer)
    {
        data.insert("message_visibility", "\"visibility: visible;\"");
        data.insert("captcha", captcha.second);
        data.insert("message_txt", "Некорректный ответ на Captcha");

        auto response = HttpResponse::newHttpViewResponse("signup.csp", data);
        response->setStatusCode(drogon::k415UnsupportedMediaType);
        response->addCookie(cookie_capt);
        callback(response);

        return;
    }


    // Delete old captcha answer
    if(req->getCookie("captcha") != "" && req->getCookie("captcha") != " ")
    {
        auto capts =\
            DB::get()->Select<::Captcha>(std::format("uid == \"{}\"", req->getCookie("captcha")));
        if(capts.size())
        {
            DB::get()->Delete<::Captcha>(capts[0].first);
        }
    }


    auto reg = Request::Register(req, callback);

    // If there is any error
    if(reg.message.size())
    {
        data.insert("message_visibility", "\"visibility: visible;\"");
        data.insert("captcha", captcha.second);
        data.insert("message_txt", reg.message);

        auto response = HttpResponse::newHttpViewResponse("signup.csp", data);
        response->setStatusCode(drogon::k415UnsupportedMediaType);
        response->addCookie(cookie_capt);
        callback(response);

        return;
    }

    auto response = HttpResponse::newRedirectionResponse("/signin");
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
                std::format("{}?count={}", req->getPath(), count));
    callback(response);
}
