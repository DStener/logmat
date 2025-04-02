#pragma once

#pragma once

#include <drogon/drogon.h>
#include <boost/tokenizer.hpp>


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
        return DB::get()->Select<::ChangeLog>(std::format(
                "name_table == \"{}\" AND id_row == {}",
                DTO::GetName<T>(), id));
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
        newlog.created_at = std::chrono::system_clock::now();

        try
        {

            // If operation is CREATE
            if(!log.before.size())
            {
                clientPtr->execSqlSync(std::format(
                    "DELETE FROM {0} WHERE id == {1};",
                    log.name_table, log.id_row
                ));
            }
            // If operation is DELETE
            else if(!log.after.size())
            {
                std::vector<std::string> fields, values;

                boost::tokenizer<boost::escaped_list_separator<char>> tok{log.before};
                for (const auto &t : tok)
                {
                    std::string str = boost::algorithm::replace_all_copy(t, "'", "\"");
                    std::vector<std::string> row;

                    boost::algorithm::split(row, str, boost::is_any_of("="));

                    fields.push_back(boost::trim_copy(row[0]));
                    values.push_back(boost::trim_copy(row[1]));
                }

                clientPtr->execSqlSync(std::format(
                    "INSERT OR IGNORE INTO {0} "
                    "( id, {2} ) VALUES ( {1} ,{3} );",

                    log.name_table, log.id_row,
                    boost::algorithm::join(fields, ", "),
                    boost::algorithm::join(values, ", ")
                ));
            }
            // If operation is UPDATE
            else
            {
                clientPtr->execSqlSync(std::format(
                    "UPDATE {0} SET {2} WHERE id == {1};",

                    log.name_table, log.id_row,
                    boost::algorithm::replace_all_copy(log.before, "'", "\"")
                ));
            }

            clientPtr->execSqlSync(DTO::SQL::Insert(newlog));
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
