#pragma once

#include <string>
#include <drogon/drogon.h>
#include <chrono>

#include "System/utils.h"
#include <boost/fusion/adapted/struct.hpp>


// For SQL
#define SERVICE_FIELDS  \
    time_p created_at; \
    id_t created_by; \
    time_p deleted_at; \
    id_t deleted_by; \

#define SQL_SERVICE_FIELDS  \
    "created_at TIMESTAMP DEFAULT \"1970-01-01 00:00:00.000000000\", " \
    "created_by INTEGER DEFAULT 0, " \
    "deleted_at TIMESTAMP DEFAULT \"1970-01-01 00:00:00.000000000\", " \
    "deleted_by INTEGER DEFAULT 0, " \
    "FOREIGN KEY(created_by) REFERENCES User(id), " \
    "FOREIGN KEY(deleted_by) REFERENCES User(id) "

#define ADAPT_SERVICE_FIELDS created_at, created_by, deleted_at, deleted_by

// DB
struct User {
    std::string username;
    std::string email;
    id_t avatar;
    time_p time2FA;
    time_p birthday;
    std::string password;
};
BOOST_FUSION_ADAPT_STRUCT(User, username, email, avatar, time2FA, birthday, password)

struct ChangeLog {
    std::string name_table;
    id_t id_row;
    std::string before;
    std::string after;
    SERVICE_FIELDS
};
BOOST_FUSION_ADAPT_STRUCT(ChangeLog, name_table, id_row, before,
                          after, ADAPT_SERVICE_FIELDS)

struct Role {
    std::string name;
    std::string description;
    std::string code;
    SERVICE_FIELDS
};
BOOST_FUSION_ADAPT_STRUCT(Role, name, description, code, ADAPT_SERVICE_FIELDS)

struct Permission {
    std::string name;
    std::string description;
    std::string code;
    SERVICE_FIELDS
};
BOOST_FUSION_ADAPT_STRUCT(Permission, name, description, code,
                          ADAPT_SERVICE_FIELDS)

struct UserAndRole {
    id_t user;
    id_t role;
    SERVICE_FIELDS
};
BOOST_FUSION_ADAPT_STRUCT(UserAndRole, user, role, ADAPT_SERVICE_FIELDS)

struct RoleAndPermission {
    id_t role;
    id_t permission;
    SERVICE_FIELDS
};
BOOST_FUSION_ADAPT_STRUCT(RoleAndPermission, role, permission,
                          ADAPT_SERVICE_FIELDS)

struct Token {
    std::string token;
    id_t user;
};
BOOST_FUSION_ADAPT_STRUCT(Token, token, user)

struct Captcha {
    std::string token;
    double answer;
    time_p time;
};
BOOST_FUSION_ADAPT_STRUCT(Captcha, token, answer, time)

struct File {
    std::string name;
    std::string description;
    std::string tags;
    std::size_t size;
    std::string _path;
    std::string _avatar_path;
    SERVICE_FIELDS
};
BOOST_FUSION_ADAPT_STRUCT(File, name, description, tags, size,
                          _path, _avatar_path, ADAPT_SERVICE_FIELDS)






struct ReturnDTO {
    drogon::HttpStatusCode code;
    std::string message;
};
BOOST_FUSION_ADAPT_STRUCT(ReturnDTO, code, message)

struct RegisterDTO {
    std::string username;
    std::string email;
    std::string password;
    std::string c_password;
    time_p birthday;
};
BOOST_FUSION_ADAPT_STRUCT(RegisterDTO, username, email, password, c_password,
                          birthday)

struct LoginDTO {
    std::string username;
    std::string password;
    std::string code;
};
BOOST_FUSION_ADAPT_STRUCT(LoginDTO, username, password, code)

