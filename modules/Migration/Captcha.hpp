#pragma once

#include "System/DTO.hpp"
#include "System/utils.h"
#include "struct_declaration.hpp"
#include <chrono>

namespace Migration {
class Captcha
{
public:
    static void up()
    {
        auto clientPtr = drogon::app().getDbClient("master");
        clientPtr->execSqlSync(
            "CREATE TABLE IF NOT EXISTS Captcha "
            "( "
                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "token VARCHAR UNIQUE NOT NULL, "
                "answer REAL, "
                "time TIMESTAMP "
            ");");

    }

    static void down()
    {
        auto clientPtr = drogon::app().getDbClient("master");
        clientPtr->execSqlSync(std::format("DROP TABLE IF EXISTS {};",
                                           DTO::GetName<::Captcha>()));
    }
};
}
