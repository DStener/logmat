#pragma once

#include "System/DTO.hpp"
#include "System/utils.h"
#include "struct_declaration.hpp"
#include <chrono>

namespace Migration {
class UserAndRole
{
public:
    static void up()
    {
        auto clientPtr = drogon::app().getDbClient("master");
        clientPtr->execSqlSync(DTO::CreateTableSQL<::UserAndRole>());

        // Get Admin id
        auto f  = clientPtr->execSqlAsyncFuture(
            std::format("SELECT * FROM {0} WHERE username == \"{1}\"",
                        DTO::GetName<::User>(), ADMIN_NAME)
            );
        auto id_user = DTO::CreateFromSQLResult<::User>(f.get())[0].first;

        // Get Role
        f  = clientPtr->execSqlAsyncFuture(
            std::format("SELECT * FROM {0} WHERE name == \"{1}\"",
                        DTO::GetName<::Role>(), "Admin")
            );
        auto id_role = DTO::CreateFromSQLResult<::Role>(f.get())[0].first;

        ::UserAndRole user_role;
        SQL::REMOVE_ATTRIB(user_role.user) = id_user;
        SQL::REMOVE_ATTRIB(user_role.role) = id_role;

        clientPtr->execSqlSync(DTO::InsertSQL(user_role));
    }

    static void down()
    {
        auto clientPtr = drogon::app().getDbClient("master");
        clientPtr->execSqlSync(std::format("DROP TABLE IF EXISTS {};",
                                           DTO::GetName<::UserAndRole>()));
    }
};
}
