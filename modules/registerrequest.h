#pragma once

#include <drogon/drogon.h>
#include "DTO_declaration.hpp"

using namespace drogon;

namespace API {

class RegisterRequest
{
public:

    static ReturnDTO newUser(RegisterDTO& info);

private:

    static bool checkUsername(const std::string& username,
                              std::string& message);

    static bool checkEmail(const std::string& email,
                           std::string& message);

    static bool checkPassword(const std::string& password,
                              std::string& message);

    static bool checkCPassword(const std::string& password,
                               const std::string& c_password,
                               std::string& message);

    static bool checkAge(const std::string& strDate, std::string& message);

    RegisterRequest() = delete;
    ~RegisterRequest() = delete;

};
}
