#pragma once

#include <drogon/drogon.h>

#include "System/database.hpp"
#include "System/utils.h"

using namespace drogon;
using callback_func = std::function<void (const HttpResponsePtr &)>;

using namespace drogon;

#undef CHECK_AND_CALLBACK
#define CHECK_AND_CALLBACK(condition, txt)                      \
if(condition) {                                                 \
    Json::Value json(txt);                                      \
                                                                \
    auto response = HttpResponse::newHttpJsonResponse(json);    \
    response->setStatusCode(drogon::k415UnsupportedMediaType);  \
    callback(response);                                         \
                                                                \
    return false;                                               \
}

namespace Request {
class Register
{
public:
    id_t id;

private:
    HttpRequestPtr reqest;
    callback_func& callback;

public:

    Register(RegisterDTO& info, const HttpRequestPtr& req, callback_func& callback)
                 : id(0), reqest(req), callback(callback)
    {
        // Check condition
        if(!checkUsername(info.username) ||
           !checkEmail(info.email) ||
           !checkPassword(info.password) ||
           !checkCPassword(info.password, info.c_password) ||
           !checkAge(info.birthday))
        {
            return;
        }

        // Calcuulate password hash
        std::stringstream hash{};
        hash << std::hash<std::string>{}(info.password);

        User user {};
        SQL::REMOVE_ATTRIB(user.username) = info.username;
        SQL::REMOVE_ATTRIB(user.email) = info.email;
        SQL::REMOVE_ATTRIB(user.birthday) = info.birthday;
        SQL::REMOVE_ATTRIB(user.password) = hash.str();

        // Add user to DB
        DB::get()->Insert(user);

        // Get current user and role "User" id in DB
        auto id_user = DB::get()->Select<::User>(std::format("username == \"{}\"", info.username))[0].first;
        auto id_role = DB::get()->Select<::Role>("name == \"User\"")[0].first;

        // Linking the user to the role
        ::UserAndRole user_role{{id_user}, {id_role}};
        DB::get()->Insert(user_role);

        auto response = HttpResponse::newHttpResponse();
        response->setStatusCode(drogon::k201Created);
        callback(response);
    }

private:

    bool checkUsername(const std::string& username)
    {
        const std::regex latin(R"(([A-Z]|[a-z]){1,})");

        CHECK_AND_CALLBACK(!std::regex_match(username,latin),
              "Имя должно содержать только буквы латинского алфавита. ");
        CHECK_AND_CALLBACK(!isupper(username[0]),
              "Имя должно начинаться с заглавной буквы. ");
        CHECK_AND_CALLBACK(username.size() < 7,
              "Имя должно быть не меньше 7 символов. ");
        CHECK_AND_CALLBACK(DB::get()->Select<User>(std::format("username == \"{}\"", username)).size() != 0,
              "Данной имя используется другим аккаунтом. ")

        return true;
    }

    bool checkEmail(const std::string& email)
    {
        const std::regex pattern(R"([a-zA-Z0-9._]{1,}%40[a-zA-Z0-9._]{1,}\.[a-zA-Z]{2,3})");

        CHECK_AND_CALLBACK(!std::regex_match(email, pattern),
              "Некорректная электронная почта. ")
        CHECK_AND_CALLBACK(DB::get()->Select<User>(std::format("email == \"{}\"", email)).size() != 0,
              "Данная почта используется для другого аккаунта")

        return true;
    }

    bool checkPassword(const std::string& password)
    {
        const std::regex one_number(R"([0-9])");
        const std::regex one_upper_char(R"([A-Z])");
        const std::regex one_lower_char(R"([A-Z])");

        CHECK_AND_CALLBACK(password.size() <= 8,
              "Минимальная длина пароля 8 символов. ")
        CHECK_AND_CALLBACK(!std::regex_search(password, one_number),
              "Пароль не содержит цифр. ")
        CHECK_AND_CALLBACK(!std::regex_search(password, one_upper_char),
              "Пароль не содержит символов в верхнем регистре. ")
        CHECK_AND_CALLBACK(!std::regex_search(password, one_lower_char),
              "Пароль не содержит символов в нижнем регистре. ")

        return true;
    }

    bool checkCPassword(const std::string& password,
                        const std::string& c_password)
    {
        CHECK_AND_CALLBACK(password.compare(c_password) != 0,
              "Значения паролей не совпадают. ")

        return true;
    }

    bool checkAge(const time_p& birthday)
    {
        using namespace std::chrono;

        CHECK_AND_CALLBACK(birthday > system_clock::now(),
              "Некорректная дата. ")
        CHECK_AND_CALLBACK(system_clock::now() - birthday < years(14),
              "Сайтом разрешено пользоваться с 14 лет. ")

        return true;
    }
};
}
