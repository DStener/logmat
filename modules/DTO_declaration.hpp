#pragma once

#include <string>
#include <drogon/drogon.h>
#include "DTO.hpp"

// For SQL
//

#define SERVICE_FIELDS  \
    std::time_t created_at; \
    SQL::ref<User> created_by; \
    std::time_t deleted_at; \
    SQL::ref<User> deleted_by; \

// DB
struct User {
    SQL::notnull<SQL::unique<std::string>> username;
    SQL::notnull<SQL::unique<std::string>> email;
    SQL::notnull<std::string> birthday;
    SQL::notnull<std::string> password;
};

struct Role {
    SQL::notnull<SQL::unique<std::string>> name;
    std::string description;
    SQL::notnull<SQL::unique<std::string>> code;
    SERVICE_FIELDS
};

struct Permission {
    SQL::notnull<SQL::unique<std::string>> name;
    std::string description;
    SQL::notnull<SQL::unique<std::string>> code;
    SERVICE_FIELDS
};

struct UserAndRole {
    SQL::ref<User> user;
    SQL::ref<Role> role;
    SERVICE_FIELDS
};

struct RoleAndPermission {
    SQL::ref<Role> role;
    SQL::ref<Permission> permission;
    SERVICE_FIELDS
};

struct Token {
    SQL::notnull<SQL::unique<std::string>> token;
    std::time_t time;
    SQL::ref<User> user;
};

struct Picture {
    SQL::notnull<SQL::unique<std::string>> name;
    std::size_t size;
    std::string path;
};


// clientPtr->execSqlAsyncFuture(
//     "CREATE TABLE IF NOT EXISTS Pictures"
//     "("
//     "  id SERIAL PRIMARY KEY,"
//     "  name CHARACTER VARYING(30),"
//     "  size INTEGER,"
//     "  path CHARACTER VARYING(256)"
//     ");"
//     );






struct ReturnDTO {
    drogon::HttpStatusCode code;
    std::string message;
};

struct RegisterDTO {
    std::string username;
    std::string email;
    std::string password;
    std::string c_password;
    std::string birthday;
};

struct LoginDTO {
    std::string username;
    std::string password;
};
