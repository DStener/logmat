#pragma once

#include "System/DTO.hpp"
#include "System/utils.h"
#include "struct_declaration.hpp"
#include <chrono>

namespace Migration {
class Permission
{
public:
    static void up()
    {
        auto clientPtr = drogon::app().getDbClient("master");
        clientPtr->execSqlSync(
            "CREATE TABLE IF NOT EXISTS Permission "
            "( "
                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "name VARCHAR UNIQUE NOT NULL, "
                "description VARCHAR, "
                "code VARCHAR UNIQUE NOT NULL, "

                SQL_SERVICE_FIELDS
            ");");

        std::vector<::Permission> perms =
        {
            {{"get-list"}, {"Permission to SELECT all data"}, {"GL"}},
            {{"get-story"}, {"Permission to SELECT all story data"}, {"GSL"}},
            {{"read"}, {"Permission to SELECT current data"}, {"R"}},
            {{"create"}, {"Permission to CREATE data"}, {"C"}},
            {{"update"}, {"Permission to UPDATE data"}, {"U"}},
            {{"delete"}, {"Permission to DELETE current data"}, {"D"}},
            {{"restore"}, {"Permission to RESTORE current data"}, {"RES"}}
        };

        std::vector<std::string> entities =
        {
            DTO::GetName<::User>(),
            DTO::GetName<::Role>(),
            DTO::GetName<::Permission>()
        };

        // Create basic periccion
        for(auto& entity : entities)
        {
            for(auto perm : perms)
            {
                perm.name += std::format("-{}",entity);
                perm.description += std::format(" {}", entity);
                perm.code += std::format("{}", entity);

                auto f = clientPtr->execSqlAsyncFuture(DTO::SQL::Insert(perm));
                try {f.get();}
                catch (const drogon::orm::DrogonDbException &e)
                {
                    std::cerr << "error:" << e.base().what() << std::endl;
                    std::cerr << "\t\t" << DTO::SQL::Insert(perm) << std::endl;
                }
            }
        }
    }

    static void down()
    {
        auto clientPtr = drogon::app().getDbClient("master");
        clientPtr->execSqlSync(std::format("DROP TABLE IF EXISTS {};",
                                           DTO::GetName<::Permission>()));
    }
};
}
