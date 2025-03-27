#pragma once

#include "System/DTO.hpp"
#include "System/utils.h"
#include "struct_declaration.hpp"
#include <chrono>

namespace Migration {
class User
{
public:
    static void up()
    {
        auto clientPtr = drogon::app().getDbClient("master");
        clientPtr->execSqlSync(DTO::CreateTableSQL<::User>());

        // Add admin user
        std::stringstream hash{};
        hash << std::hash<std::string>{}(ADMIN_PASSWORD);

        ::User user {};
        SQL::REMOVE_ATTRIB(user.username) = ADMIN_NAME;
        SQL::REMOVE_ATTRIB(user.email) = "NONE";
        SQL::REMOVE_ATTRIB(user.time2FA) = time_p();
        SQL::REMOVE_ATTRIB(user.birthday) = std::chrono::system_clock::now();
        SQL::REMOVE_ATTRIB(user.password) = hash.str();

        clientPtr->execSqlSync(DTO::InsertSQL(user));
    }

    static void down()
    {
        auto clientPtr = drogon::app().getDbClient("master");
        clientPtr->execSqlSync(std::format("DROP TABLE IF EXISTS {};",
                                           DTO::GetName<::User>()));
    }
};
}
