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
        clientPtr->execSqlSync(DTO::CreateTableSQL<::Role>());

        std::vector<::Role> roles =
            {
             {{"Admin"}, {"Administrator"}, {"ADMIN"}},
             {{"User"}, {"Standert user"}, {"USER"}},
             {{"Guest"}, {"Guest"}, {"GUESR"}},
            };

        for(auto& role : roles)
        {
            clientPtr->execSqlSync(DTO::InsertSQL(role));
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
