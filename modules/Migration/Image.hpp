#pragma once

#include "System/DTO.hpp"
#include "System/utils.h"
#include "struct_declaration.hpp"
#include <chrono>
#include <filesystem>

#define PATH "/home/ext/Документы/logmat/img"

namespace Migration {
class Image
{
public:
    static void up()
    {
        auto clientPtr = drogon::app().getDbClient("master");
        clientPtr->execSqlSync(DTO::CreateTableSQL<::Image>());

        for (const auto & file : std::filesystem::directory_iterator(PATH))
        {
            ::Image img {};
            SQL::REMOVE_ATTRIB(img.name) = file.path().filename();
            SQL::REMOVE_ATTRIB(img.path) = file.path();
            SQL::REMOVE_ATTRIB(img.size) = file.file_size();

            clientPtr->execSqlSync(DTO::InsertSQL(img));
        }
    }

    static void down()
    {
        auto clientPtr = drogon::app().getDbClient("master");
        clientPtr->execSqlSync(std::format("DROP TABLE IF EXISTS {};",
                                           DTO::GetName<::Image>()));
    }
};
}
