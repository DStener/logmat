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
        clientPtr->execSqlSync(
            "CREATE TABLE IF NOT EXISTS User "
            "( "
                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "username VARCHAR UNIQUE NOT NULL, "
                "email VARCHAR UNIQUE NOT NULL, "
                "time2FA TIMESTAMP, "
                "birthday TIMESTAMP NOT NULL, "
                "password VARCHAR NOT NULL "
            ");");

        // Add admin user
        std::stringstream hash{};
        hash << std::hash<std::string>{}(ADMIN_PASSWORD);

        ::User user {};
        user.username = ADMIN_NAME;
        user.email = "NONE";
        user.time2FA = time_p();
        user.birthday = std::chrono::system_clock::now();
        user.password = hash.str();

        clientPtr->execSqlSync(DTO::SQL::Insert(user));
    }

    static void down()
    {
        auto clientPtr = drogon::app().getDbClient("master");
        clientPtr->execSqlSync(std::format("DROP TABLE IF EXISTS {};",
                                           DTO::GetName<::User>()));
    }
};
}
