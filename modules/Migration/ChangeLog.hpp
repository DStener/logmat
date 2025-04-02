#pragma once

#include "System/DTO.hpp"
#include "System/utils.h"
#include "struct_declaration.hpp"
#include <chrono>

namespace Migration {
class ChangeLog
{
public:
    static void up()
    {
        auto clientPtr = drogon::app().getDbClient("master");
        clientPtr->execSqlSync(
            "CREATE TABLE IF NOT EXISTS ChangeLog "
            "( "
                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "name_table VARCHAR NOT NULL, "
                "id_row NOT NULL, "
                "before VARCHAR NOT NULL, "
                "after VARCHAR NOT NULL, "

                SQL_SERVICE_FIELDS
            ");");
    }

    static void down()
    {
        auto clientPtr = drogon::app().getDbClient("master");
        clientPtr->execSqlSync(std::format("DROP TABLE IF EXISTS {};",
                                           DTO::GetName<::ChangeLog>()));
    }
};
}
