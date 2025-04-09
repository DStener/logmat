#pragma once

#pragma once

#include <drogon/drogon.h>
#include <boost/tokenizer.hpp>


#include "System/database.hpp"
#include "System/DTO.hpp"
#include "System/utils.h"

#include <algorithm>

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

    static void getRows(id_t id)
    {
    }

    static Json::Value GetJSONComplianceRules(DTORow<::ChangeLog>& row)
    {
        using esc_tokenizer = boost::tokenizer<boost::escaped_list_separator<char>>;

        Json::Value json = DTO::JSON::From(row);
        std::vector<std::string> before, after;

        esc_tokenizer before_tok{ row.second.before };
        esc_tokenizer after_tok{ row.second.after };

        esc_tokenizer::iterator before_it = before_tok.begin();
        esc_tokenizer::iterator after_it = after_tok.begin();

        if(row.second.before.size() && !row.second.after.size())
        {
            for(auto it : before_tok)
            {
                std::string before_txt = boost::algorithm::trim_copy(*before_it);
                bool hide_before = Log::isServiceField(before_txt);

                if (!hide_before)
                {
                    before.push_back(Log::hideSecret(before_txt));
                }
            }
            after.push_back("DELETE");
        }
        else if (row.second.after.size() && !row.second.before.size())
        {
            for(auto it : after_tok)
            {
                std::string after_txt = boost::algorithm::trim_copy(*after_it);
                bool hide_after = Log::isServiceField(after_txt);

                if (!hide_after)
                {
                    after.push_back(Log::hideSecret(after_txt));
                }
            }
            before.push_back("CREATE");
        }
        else if(row.second.before.size() && row.second.after.size())
        {
            for (; before_it != before_tok.end() || after_it != after_tok.end();)
            {
                std::string before_txt = boost::algorithm::trim_copy(*before_it);
                std::string after_txt = boost::algorithm::trim_copy(*after_it);

                bool before_finished = (before_it == before_tok.end());
                bool after_finished = (after_it == after_tok.end());

                bool hide_before = Log::isServiceField(before_txt);
                bool hide_after = Log::isServiceField(after_txt);


                if (before_txt.compare(after_txt) != 0 || before_finished || after_finished)
                {
                    if (!before_finished && !hide_before)
                    {
                        before.push_back(Log::hideSecret(before_txt));
                    }
                    if (!after_finished && !hide_after)
                    {
                        after.push_back(Log::hideSecret(after_txt));
                    }
                }

                ++before_it;
                ++after_it;

                before_it = (before_it != before_tok.end())? before_it : before_tok.end();
                after_it = (after_it != after_tok.end()) ? after_it : after_tok.end();
            }
        }

        if(before.empty() && after.empty())
        {
            json["before"] = "";

            if(after.empty())
            {
                json["after"] = "SOFT DELETE";
            }
            else
            {
                json["after"] = "RESTORE";
            }
        }
        else
        {
            json["before"] = boost::algorithm::join(before, ", ");
            json["after"] = boost::algorithm::join(after, ", ");
        }
        
        return json;
    }

    static bool Restore(id_t id)
    {
        auto _temp = DB::get()->Select<::ChangeLog>(std::format("id == {}", id));
        if(!_temp.size()) { return false; }

        auto log = _temp[0].second;

        auto clientPtr = drogon::app().getDbClient( );
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

    static std::string hideSecret(std::string& row)
    {
        if (row.starts_with("password"))
        {
            std::vector<std::string> cell;
            boost::algorithm::split(cell, row, boost::is_any_of("="));

            return std::format("{} = {}",
                boost::trim_copy(cell[0]),
                std::string("X"));
        }
        else
        {
            return row;
        }
    }

    static bool isServiceField(std::string& row)
    {
        return row.starts_with("_") ||
               row.starts_with("created_at") ||
               row.starts_with("created_by") ||
               row.starts_with("deleted_at") ||
               row.starts_with("deleted_by");
    }
};
}
