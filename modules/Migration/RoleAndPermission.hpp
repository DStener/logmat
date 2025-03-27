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
        clientPtr->execSqlSync(DTO::CreateTableSQL<::RoleAndPermission>());

        // Get Permission
        auto f  = clientPtr->execSqlAsyncFuture(
            std::format("SELECT * FROM {0} ", DTO::GetName<::Permission>())
        );
        auto perms(DTO::CreateFromSQLResult<::Permission>(f.get()));

        // Get Role
        f = clientPtr->execSqlAsyncFuture(
            std::format("SELECT * FROM {0} ", DTO::GetName<::Role>())
        );
        auto roles(DTO::CreateFromSQLResult<::Role>(f.get()));

        for(auto& role : roles) {
            for(auto& perm : perms)
            {
                if((SQL::REMOVE_ATTRIB(role.second.name) == "Admin") ||

                   (SQL::REMOVE_ATTRIB(role.second.name) == "User" &&
                       (SQL::REMOVE_ATTRIB(perm.second.name) == "get-list-User" ||
                       SQL::REMOVE_ATTRIB(perm.second.name) == "read-User" ||
                       SQL::REMOVE_ATTRIB(perm.second.name) == "update-User")
                   ) ||

                   (SQL::REMOVE_ATTRIB(role.second.name) == "Guest" &&
                   SQL::REMOVE_ATTRIB(perm.second.name) == "get-list-User"))
                {
                    // If the network is a duplicat
                    auto f  = clientPtr->execSqlAsyncFuture(
                        std::format("SELECT * FROM {0} WHERE role == {1} AND permission == {2}",
                                    DTO::GetName<::RoleAndPermission>(),
                                    role.first, perm.first)
                        );
                    auto vec(DTO::CreateFromSQLResult<::RoleAndPermission>(f.get()));
                    if(vec.size() != 0) { continue; }

                    ::RoleAndPermission role_perm {{role.first},{perm.first}};
                    clientPtr->execSqlSync(DTO::InsertSQL(role_perm));
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
