#pragma once

#include <string>
#include <drogon/drogon.h>
#include <chrono>

#include "System/sql_template.h"

using time_p = std::chrono::utc_clock::time_point;

// For SQL
using namespace SQL_ATTRIB;

#define SERVICE_FIELDS  \
    time_p created_at; \
    REFERENCES<User> created_by; \
    time_p deleted_at; \
    REFERENCES<User> deleted_by; \

// DB
struct User {
    NOTNULL<UNIQUE<std::string>> username;
    NOTNULL<UNIQUE<std::string>> email;
    NOTNULL<time_p> birthday;
    NOTNULL<std::string> password;
};

struct Role {
    NOTNULL<UNIQUE<std::string>> name;
    std::string description;
    NOTNULL<UNIQUE<std::string>> code;
    SERVICE_FIELDS
};

struct Permission {
    NOTNULL<UNIQUE<std::string>> name;
    std::string description;
    NOTNULL<UNIQUE<std::string>> code;
    SERVICE_FIELDS
};

struct UserAndRole {
    REFERENCES<User> user;
    REFERENCES<Role> role;
    SERVICE_FIELDS
};

struct RoleAndPermission {
    REFERENCES<Role> role;
    REFERENCES<Permission> permission;
    SERVICE_FIELDS
};

struct Token {
    NOTNULL<UNIQUE<std::string>> token;
    time_p time;
    REFERENCES<User> user;
};

struct ReturnDTO {
    drogon::HttpStatusCode code;
    std::string message;
};

struct RegisterDTO {
    std::string username;
    std::string email;
    std::string password;
    std::string c_password;
    time_p birthday;
};

struct LoginDTO {
    std::string username;
    std::string password;
};
