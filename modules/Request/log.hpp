#pragma once

#pragma once

#include <drogon/drogon.h>


#include "System/database.hpp"
#include "System/DTO.hpp"
#include "System/utils.h"

using namespace drogon;

namespace Request{
class Log {
public:
    template <typename T>
    static auto Get(id_t id)
    {
        auto condition = std::format("name_table == \"{}\" AND id_row == {}",
                                     DTO::GetName<T>(), id);
        return DB::get()->Select<::ChangeLog>(condition);
    }

    static bool Restore(id_t id)
    {
        auto _temp = DB::get()->Select<::ChangeLog>(std::format("id == {}", id));
        if(!_temp.size()) { return false; }

        auto log = _temp[0].second;

        auto clientPtr = drogon::app().getDbClient("master");
        clientPtr->execSqlSync("BEGIN TRANSACTION;");

        ::ChangeLog newlog = log;
        std::swap(newlog.after, newlog.before);
        SQL::REMOVE_ATTRIB(newlog.created_at) = std::chrono::system_clock::now();

        try
        {
            // If operation is DELETE or CREATE
            if(log.id_row == 0) { return false; }

            clientPtr->execSqlSync(std::format("UPDATE {0} SET {1} = \"{2}\" WHERE id == {3};",
                                   SQL::REMOVE_ATTRIB(log.name_table),
                                   SQL::REMOVE_ATTRIB(log.name_field),
                                   SQL::REMOVE_ATTRIB(log.before),
                                   SQL::REMOVE_ATTRIB(log.id_row)));

            clientPtr->execSqlSync(DTO::InsertSQL(newlog));
            clientPtr->execSqlSync("COMMIT;");
        }
        catch (const orm::DrogonDbException &e)
        {
            clientPtr->execSqlSync("ROLLBACK;");
            std::cerr << "error:" << e.base().what() << std::endl;
        }
        return true;
    }

private:
    Log() = delete;
    ~Log() = delete;
};
}
