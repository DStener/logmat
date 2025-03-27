#pragma once

#include <drogon/drogon.h>

#include "System/database.hpp"
#include "System/utils.h"

using namespace drogon;
using callback_func = std::function<void (const HttpResponsePtr &)>;


namespace Request {
class Register
{
public:
    id_t id;
    std::string message;

private:
    HttpRequestPtr reqest;
    callback_func& callback;
    RegisterDTO info;

public:

    Register(const HttpRequestPtr& req, callback_func& callback)
        : id(0), reqest(req), callback(callback)
    {
        info = DTO::CreateFromRequestBody<RegisterDTO>(req->getBody());

        bool asJson = (info.asJSON == "true");

        // Check condition
        if(!checkUsername(asJson) ||
           !checkEmail(asJson) ||
           !checkPassword(asJson) ||
           !checkCPassword(asJson) ||
           !checkAge(asJson))
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
    }

private:

    bool checkUsername(bool& asJson)
    {
        const std::regex latin(R"(([A-Z]|[a-z]){1,})");


        if(!std::regex_match(info.username,latin))
        {
            message = "Имя должно содержать только буквы латинского алфавита.";
            return false;
        }

        if(!isupper(info.username[0]))
        {
            message = "Имя должно начинаться с заглавной буквы.";
            return false;
        }

        if(!isupper(info.username.size() < 7))
        {
            message = "Имя должно быть не меньше 7 символов.";
            return false;
        }

        if(DB::get()->Select<User>(std::format("username == \"{}\"", info.username)).size() != 0)
        {
            message = "Данной имя используется другим аккаунтом.";
            return false;
        }

        return true;
    }

    bool checkEmail(bool& asJson)
    {
        const std::regex pattern(R"([a-zA-Z0-9._]{1,}%40[a-zA-Z0-9._]{1,}\.[a-zA-Z]{2,3})");


        if(!std::regex_match(info.email, pattern))
        {
            message = "Некорректная электронная почта.";
            return false;
        }

        if(DB::get()->Select<User>(std::format("email == \"{}\"", info.email)).size() != 0)
        {
            message = "Данная почта используется для другого аккаунта";
            return false;
        }

        return true;
    }

    bool checkPassword(bool& asJson)
    {
        const std::regex one_number(R"([0-9])");
        const std::regex one_upper_char(R"([A-Z])");
        const std::regex one_lower_char(R"([A-Z])");


        if(info.password.size() <= 8)
        {
            message = "Минимальная длина пароля 8 символов.";
            return false;
        }

        if(!std::regex_search(info.password, one_number))
        {
            message = "Пароль не содержит цифр.";
            return false;
        }

        if(!std::regex_search(info.password, one_upper_char))
        {
            message = "Пароль не содержит символов в верхнем регистре.";
            return false;
        }

        if(!std::regex_search(info.password, one_lower_char))
        {
            message = "Пароль не содержит символов в нижнем регистре.";
            return false;
        }

        return true;
    }

    bool checkCPassword(bool& asJson)
    {
        if(info.password.compare(info.c_password) != 0)
        {
            message = "Значения паролей не совпадают.";
            return false;
        }

        return true;
    }

    bool checkAge(bool& asJson)
    {
        using namespace std::chrono;

        if(info.birthday > system_clock::now())
        {
            message = "Некорректная дата.";
            return false;
        }

        if(system_clock::now() - info.birthday < years(14))
        {
            message = "Сайтом разрешено пользоваться с 14 лет.";
            return false;
        }

        return true;
    }
};
}
