#pragma once
#include "DTO.hpp"
#include "struct_declaration.hpp"
#include <drogon/Cookie.h>
#include <drogon/HttpTypes.h>
#include <string>

using namespace drogon;

#define MAX_TOKEN 32

namespace API {
class LoginRequest {
public:
    static ReturnDTO loginUser(const LoginDTO& info);
    static std::size_t getUserByToken(std::string token, ReturnDTO& ret);
private:
    static Token generateToken();
    LoginRequest() = delete;
};
}
