#include "registerrequest.h"

#include "System/database.hpp"

#define CHECK(conditions, txt) { \
    if(conditions) { \
        message += txt; \
        flag = false; \
    } \
}

using namespace API;


ReturnDTO RegisterRequest::newUser(RegisterDTO& info)
{
    ReturnDTO ret {HttpStatusCode::k201Created};
    // Check condition
    bool isInvalid = \
                     !checkUsername(info.username, ret.message) ||
                     !checkEmail(info.email, ret.message) ||
                     !checkPassword(info.password, ret.message) ||
                     !checkCPassword(info.password, info.c_password, ret.message) ||
                     !checkAge(info.birthday, ret.message);
    if (isInvalid){
        ret.code = HttpStatusCode::k415UnsupportedMediaType;
        return ret;
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
    return ret;
}
// Check correct username
bool RegisterRequest::checkUsername(const std::string& username,
                                         std::string& message)
{
    bool flag = true;
    const std::regex latin(R"(([A-Z]|[a-z]){1,})");
    CHECK(!std::regex_match(username,latin),
          "Имя должно содержать только буквы латинского алфавита. ");
    CHECK(!isupper(username[0]),
          "Имя должно начинаться с заглавной буквы. ");
    CHECK(username.size() < 7,
          "Имя должно быть не меньше 7 символов. ");
    CHECK(DB::get()->Select<User>(std::format("username == \"{}\"", username)).size() != 0,
          "Данной имя используется другим аккаунтом. ")
    return flag;
}
// Check the correct email
bool RegisterRequest::checkEmail(const std::string& email,
                                      std::string& message)
{
    bool flag = true;
    const std::regex pattern(R"([a-zA-Z0-9._]{1,}%40[a-zA-Z0-9._]{1,}\.[a-zA-Z]{2,3})");
    CHECK(!std::regex_match(email, pattern),
          "Некорректная электронная почта. ")
    CHECK(DB::get()->Select<User>(std::format("email == \"{}\"", email)).size() != 0,
          "Данная почта используется для другого аккаунта")
    return flag;
}
// Check correct password
bool RegisterRequest::checkPassword(const std::string& password,
                                         std::string& message)
{
    bool flag = true;
    const std::regex one_number(R"([0-9])");
    const std::regex one_upper_char(R"([A-Z])");
    const std::regex one_lower_char(R"([A-Z])");
    CHECK(password.size() <= 8,
          "Минимальная длина пароля 8 символов. ")
    CHECK(!std::regex_search(password, one_number),
          "Пароль не содержит цифр. ")
    CHECK(!std::regex_search(password, one_upper_char),
          "Пароль не содержит символов в верхнем регистре. ")
    CHECK(!std::regex_search(password, one_lower_char),
          "Пароль не содержит символов в нижнем регистре. ")
    return flag;
}
// Checking for a password match
bool RegisterRequest::checkCPassword(const std::string& password,
                           const std::string& c_password,
                           std::string& message)
{
    bool flag = true;
    CHECK(password.compare(c_password) != 0,
          "Значения паролей не совпадают. ")
    return flag;
}

// Age verification for more than 14 years
bool RegisterRequest::checkAge(const time_p& birthday,
                                    std::string& message)
{
    using namespace std::chrono;
    bool flag = true;

    CHECK(birthday > utc_clock::now(),
          "Некорректная дата. ")
    CHECK(utc_clock::now() - birthday < years(14),
          "Сайтом разрешено пользоваться с 14 лет. ")

    return flag;
}
