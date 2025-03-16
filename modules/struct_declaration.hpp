#pragma once

#include <string>
#include <drogon/drogon.h>
#include "DTO.hpp"

// For SQL
using namespace SQL_ATTRIB;

#define SERVICE_FIELDS  \
    std::time_t created_at; \
    REFERENCES<User> created_by; \
    std::time_t deleted_at; \
    REFERENCES<User> deleted_by; \


// DB
struct User {
    NOTNULL<UNIQUE<std::string>> username;
    NOTNULL<UNIQUE<std::string>> email;
    NOTNULL<std::string> birthday;
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
    std::time_t time;
    REFERENCES<User> user;
};

struct Picture {
    NOTNULL<UNIQUE<std::string>> name;
    std::size_t size;
    std::string path;
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
    std::string birthday;
};

struct LoginDTO {
    std::string username;
    std::string password;
};
