#pragma once

#include "System/DTO.hpp"
#include "System/utils.h"
#include "struct_declaration.hpp"
#include <chrono>

namespace Migration {
class Role
{
public:
    static void up()
    {
        auto clientPtr = drogon::app().getDbClient("master");
        clientPtr->execSqlSync(
            "CREATE TABLE IF NOT EXISTS Role "
            "( "
                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "name VARCHAR UNIQUE NOT NULL, "
                "description VARCHAR, "
                "code VARCHAR UNIQUE NOT NULL, "

                SQL_SERVICE_FIELDS
            ");");

        std::vector<::Role> roles =
            {
             {{"Admin"}, {"Administrator"}, {"ADMIN"}},
             {{"User"}, {"Standert user"}, {"USER"}},
             {{"Guest"}, {"Guest"}, {"GUESR"}},
            };

        for(auto& role : roles)
        {
            clientPtr->execSqlSync(DTO::SQL::Insert(role));
        }
    }

    static void down()
    {
        auto clientPtr = drogon::app().getDbClient("master");
        clientPtr->execSqlSync(std::format("DROP TABLE IF EXISTS {};",
                                           DTO::GetName<::Role>()));
    }
};
}
