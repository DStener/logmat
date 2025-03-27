#pragma once

#include "System/DTO.hpp"
#include "System/utils.h"
#include "struct_declaration.hpp"
#include <chrono>
#include <filesystem>

#define PATH "/home/ext/Документы/logmat/img"

namespace Migration {
class Review
{
public:
    static void up()
    {
        auto clientPtr = drogon::app().getDbClient("master");
        clientPtr->execSqlSync(DTO::CreateTableSQL<::Review>());

        ::Review review;
        review.user_id = 1;
        review.time = std::chrono::system_clock::now();
        review.rating = 5;
        review.text = "BEST OF THE BEST";

        clientPtr->execSqlSync(DTO::InsertSQL(review));

    }

    static void down()
    {
        auto clientPtr = drogon::app().getDbClient("master");
        clientPtr->execSqlSync(std::format("DROP TABLE IF EXISTS {};",
                                           DTO::GetName<::Review>()));
    }
};
}
