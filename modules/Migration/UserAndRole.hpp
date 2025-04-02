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
        clientPtr->execSqlSync(
            "CREATE TABLE IF NOT EXISTS UserAndRole "
            "( "
                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "user INTEGER, "
                "role INTEGER, "

                SQL_SERVICE_FIELDS  ", "

                "FOREIGN KEY(user) REFERENCES User(id), "
                "FOREIGN KEY(role) REFERENCES Role(id) "
            ");");

        // Get Admin id
        auto f  = clientPtr->execSqlAsyncFuture(
            std::format("SELECT * FROM {0} WHERE username == \"{1}\"",
                        DTO::GetName<::User>(), ADMIN_NAME)
            );
        auto id_user = DTO::SQL::To<::User>(f.get())[0].first;

        // Get Role
        f  = clientPtr->execSqlAsyncFuture(
            std::format("SELECT * FROM {0} WHERE name == \"{1}\"",
                        DTO::GetName<::Role>(), "Admin")
            );
        auto id_role = DTO::SQL::To<::Role>(f.get())[0].first;

        ::UserAndRole user_role;
        user_role.user = id_user;
        user_role.role = id_role;

        clientPtr->execSqlSync(DTO::SQL::Insert(user_role));
    }

    static void down()
    {
        auto clientPtr = drogon::app().getDbClient("master");
        clientPtr->execSqlSync(std::format("DROP TABLE IF EXISTS {};",
                                           DTO::GetName<::UserAndRole>()));
    }
};
}
