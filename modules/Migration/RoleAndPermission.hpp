#pragma once

#include "System/DTO.hpp"
#include "System/utils.h"
#include "struct_declaration.hpp"
#include <chrono>

namespace Migration {
class RoleAndPermission
{
public:
    static void up()
    {
        auto clientPtr = drogon::app().getDbClient("master");
        clientPtr->execSqlSync(
            "CREATE TABLE IF NOT EXISTS RoleAndPermission "
            "( "
                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "role INTEGER, "
                "permission INTEGER, "

                SQL_SERVICE_FIELDS ", "

                "FOREIGN KEY(role) REFERENCES Role(id), "
                "FOREIGN KEY(permission) REFERENCES Permission(id) "
            ");");

        // Get Permission
        auto f  = clientPtr->execSqlAsyncFuture(
            std::format("SELECT * FROM {0} ", DTO::GetName<::Permission>())
        );
        auto perms = DTO::SQL::To<::Permission>(f.get());

        // Get Role
        f = clientPtr->execSqlAsyncFuture(
            std::format("SELECT * FROM {0} ", DTO::GetName<::Role>())
        );
        auto roles = DTO::SQL::To<::Role>(f.get());

        for(auto& role : roles) {
            for(auto& perm : perms)
            {
                if((role.second.name == "Admin") ||

                   (role.second.name == "User" &&
                       (perm.second.name == "get-list-User" ||
                       perm.second.name == "read-User" ||
                       perm.second.name == "update-User")
                   ) ||

                   (role.second.name == "Guest" &&
                        (perm.second.name == "get-list-User" ||
                         perm.second.name == "get-list-File"))
                )
                {
                    // If the network is a duplicat
                    auto f  = clientPtr->execSqlAsyncFuture(
                        std::format("SELECT * FROM {0} WHERE role == {1} AND permission == {2}",
                                    DTO::GetName<::RoleAndPermission>(),
                                    role.first, perm.first)
                        );
                    auto vec(DTO::SQL::To<::RoleAndPermission>(f.get()));
                    if(vec.size() != 0) { continue; }

                    ::RoleAndPermission role_perm {role.first, perm.first};
                    clientPtr->execSqlSync(DTO::SQL::Insert(role_perm));
                }
            }
        }
    }

    static void down()
    {
        auto clientPtr = drogon::app().getDbClient("master");
        clientPtr->execSqlSync(std::format("DROP TABLE IF EXISTS {};",
                                           DTO::GetName<::RoleAndPermission>()));
    }
};
}
