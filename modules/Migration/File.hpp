#pragma once

#include "System/DTO.hpp"
#include "System/utils.h"
#include "struct_declaration.hpp"
#include <chrono>

namespace Migration {
class File
{
public:
    static void up()
    {
        auto clientPtr = drogon::app().getDbClient("master");
        clientPtr->execSqlSync(
            "CREATE TABLE IF NOT EXISTS File "
            "( "
                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "name VARCHAR UNIQUE NOT NULL, "
                "description VARCHAR, "
                "tags VARCHAR, "
                "size INTEGER, "
                "_path VARCHAR NOT NULL, "
                "_avatar_path VARCHAR, "

                SQL_SERVICE_FIELDS
            ");");

    }

    static void down()
    {
        auto clientPtr = drogon::app().getDbClient("master");
        clientPtr->execSqlSync(std::format("DROP TABLE IF EXISTS {};",
                                           DTO::GetName<::File>()));
    }
};
}
