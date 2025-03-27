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
        clientPtr->execSqlSync(DTO::CreateTableSQL<::ChangeLog>());
    }

    static void down()
    {
        auto clientPtr = drogon::app().getDbClient("master");
        clientPtr->execSqlSync(std::format("DROP TABLE IF EXISTS {};",
                                           DTO::GetName<::ChangeLog>()));
    }
};
}
