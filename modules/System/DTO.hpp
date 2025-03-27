#pragma once

#include "System/utils.h"
#include "System/field_reflection.hpp"
#include "sql_template.h"


#include <cctype>
#include <iostream>
#include <json/value.h>
#include <sstream>
#include <string>
#include <string_view>
#include <trantor/utils/Logger.h>
#include <cmath>
#include <chrono>
#include <type_traits>
#include <drogon/drogon.h>

// #include <cxxabi.h>

using namespace field_reflection;

class DTO
{
public:

    // Get DTO struct name
    template <typename T>
    static std::string GetName()
    {
        return std::string{field_reflection::type_name<T>};
    }

    // Convert DTO to Json::Value
    template <typename T>
    static Json::Value ToJson(T s)
    {
        Json::Value ret;
        for_each_field(s, [&ret](std::string_view field, auto value)
        {
            if(DTO::SQL::CheckField::isSecret(field)) { return; }

            // ERROR: if convert directly to std::string<
            // without std::stringstream
            std::stringstream stream;
            stream << value;

            ret[std::string(field)] = stream.str();
        });
        return ret;
    }

    template <typename T>
    static Json::Value ToJson(DTORow<T> s)
    {
        Json::Value ret;
        ret["id"] = s.first;
        for_each_field(s.second, [&ret](std::string_view field, auto value)
        {
            if(DTO::SQL::CheckField::isSecret(field)) { return; }

            // ERROR: if convert directly to std::string<
            // without std::stringstream
            std::stringstream stream;
            stream << value;

            ret[std::string(field)] = stream.str();
        });
        return ret;
    }



    // Return SQL command to create Teable based on DTO strucr T
    template <typename T, field_referenceable U = std::remove_cvref_t<T>>
    static std::string CreateTableSQL()
    {
        std::string sql{"CREATE TABLE IF NOT EXISTS"};
        std::string references{};
        sql += " " + DTO::GetName<T>();
        sql += " ( id INTEGER PRIMARY KEY AUTOINCREMENT ";

        DTO::fillSQLCreateTable<T>(sql, references, std::make_index_sequence<field_count<U>>());

        sql += references + " );";
        return sql;
    }

    // Return SQL command to insert to Teable DTO strucr T
    template <typename T, field_referenceable U = std::remove_cvref_t<T>>
    static std::string InsertSQL(T& s)
    {
        // T _cash;
        std::string sql{"INSERT OR IGNORE INTO"};
        sql += " " + DTO::GetName<T>();
        sql += " ( ";

        std::string values{};

        for_each_field(s, [&sql, &values](std::string_view field, auto& value)
        {
            sql += std::string{field} + ", ";
            values += std::format("{},", DTO::SQL::to_string(value));
        });

        return sql.substr(0,sql.size() - 2) + " ) VALUES ( " + values.substr(0,values.size() - 1) + " );";
    }

    // Convert request Body to DTO
    template <typename T>
    static T CreateFromRequestBody(const std::string_view& str)
    {
        T s;
        for_each_field(s, [&str](std::string_view field, auto& value)
        {
            using type_dec = std::remove_cvref_t<::SQL::getType<decltype(value)>>;

            if (str.find(field) == std::string_view::npos) { return; }

            size_t start = str.find(field) + field.size() + 1;
            size_t size = str.find("&", start) - start;

            auto substr = std::string{str.substr(start, size)};
            if constexpr (std::is_same_v<type_dec, int> ||
                          std::is_same_v<type_dec, std::size_t> ||
                          std::is_same_v<type_dec, id_t>)
            {
                ::SQL::REMOVE_ATTRIB(value) = std::stoi(substr);
            }
            else if constexpr(std::is_same_v<type_dec, std::string>)
            {
                ::SQL::REMOVE_ATTRIB(value) = substr;
            }
            else if constexpr(std::is_same_v<type_dec, time_p>)
            {
                using namespace std::chrono;

                time_t rawtime = time(nullptr);
                std::tm time = *localtime(&rawtime);

                std::istringstream stream{substr};
                stream >> std::get_time(&time, "%Y-%m-%d");

                ::SQL::REMOVE_ATTRIB(value) = system_clock::from_time_t(std::mktime(&time));
            }
        });
        return s;
    }


    // Convert orm::Result (SQL Response) to DTO
    template <typename T, field_referenceable U = std::remove_cvref_t<T>>
    static ResponseVec<T> CreateFromSQLResult(const drogon::orm::Result& result)
    {
        ResponseVec<T> vec{};
        for(const auto& row : result)
        {
            DTORow<T> _cash{row.at("id").as<id_t>(), T()};
            DTO::fillFromSQLResult(_cash, row, std::make_index_sequence<field_count<U>>());
            vec.push_back(_cash);
        }
        return vec;
    }

    class SQL {
    public:

        template <typename T>
        static std::string to_string(T& t)
        {
            using type_dec = std::remove_cvref_t<::SQL::getType<decltype(t)>>;

            // ERROR: if convert directly to std::string
            // without std::stringstream
            std::stringstream stream;
            stream << t;

            if constexpr (std::is_same_v<type_dec, std::string> ||
                          std::is_same_v<type_dec, time_p>)
            {
                return std::format("\"{}\"", stream.str());
            }
            else
            {
                return std::format("{}", stream.str());
            }
        }

        class CheckField {
        public:
            static bool isSecret(std::string_view& field)
            {
                using sv = std::string_view;
                return field == sv("password") ||
                       field == sv("created_at") ||
                       field == sv("created_by") ||
                       field == sv("deleted_at") ||
                       field == sv("deleted_by");
            }

            template <typename T>
            static bool NotNull(T& t)
            {
                bool flag = false;
                for_each_field(t, [&flag](std::string_view field, auto& value)
                {
                    using type = const std::remove_cvref_t<decltype(value)>&;

                    if (!::SQL::isNotNull<type>::value) { return; }

                    flag |= isNull(value);
                });
                return !flag;
            }

            template <typename T>
            static std::string UpdateNotNull(T& s)
            {
                std::string sql{};
                for_each_field(s, [&sql](std::string_view field, auto& value)
                {
                    // std::cout << std::format("{}: [{}] = {}", isNull(value), field, DTO::SQL::to_string(value)) << std::endl;
                    if (isNull(value)) { return; }
                    sql += std::format("{} = {}, ", field, DTO::SQL::to_string(value));
                });
                // if (sql.size() == 0) return sql;
                return sql.substr(0,sql.size() - 2);
                // return  std::string_view{"sql"};
            }

            template <typename T>
            static std::string UniqueSQL(T& t)
            {
                std::string sql;
                for_each_field(t, [&sql](std::string_view field, auto& value)
                {
                    using type = const std::remove_cvref_t<decltype(value)>&;

                    if (!::SQL::isUnique<type>::value) { return; }

                    sql += std::format(" {} == {} OR ", field, DTO::SQL::to_string(value));
                });
                return sql.substr(0,sql.size() - 3);
            }

            template <typename T>
            static bool isNull(T& t)
            {
                using type_dec = std::remove_cvref_t<::SQL::getType<decltype(t)>>;

                if constexpr (std::is_same_v<type_dec, int> ||
                              std::is_same_v<type_dec, std::size_t> ||
                              std::is_same_v<type_dec, id_t>)
                {
                    return (t == 0);
                }
                else if constexpr(std::is_same_v<type_dec, std::string>)
                {
                    return (::SQL::REMOVE_ATTRIB(t) == "" ||
                             ::SQL::REMOVE_ATTRIB(t) == " ");
                }
                else if constexpr(std::is_same_v<type_dec, time_p>)
                {
                    return (t == std::chrono::system_clock::time_point());
                }
                return true;
            }
        };
    };



private:
    // Fill from SQL Result
    template <typename T, std::size_t... Is, field_referenceable U = std::remove_cvref_t<T>>
    static constexpr void fillFromSQLResult(DTORow<T>& t, const drogon::orm::Row& row, std::index_sequence<Is...>)
    {
        // The function of convert Row::Field to DTO::field
        auto func = [&row](const std::string_view& name, auto& field)
        {
            using type_dec = std::remove_cvref_t<::SQL::getType<decltype(field)>>;

            if constexpr (::SQL::isRef<const std::remove_cvref_t<decltype(field)>&>::value)
            {
                ::SQL::REMOVE_ATTRIB(field) =\
                    row[std::string{name}].as<size_t>();
            }
            else if constexpr (std::is_same_v<type_dec, time_p>)
            {
                using namespace std::chrono;

                time_t rawtime = time(nullptr);
                std::tm tm = *localtime(&rawtime);

                std::istringstream stream{row[std::string{name}].c_str()};
                stream >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S.%f");

                ::SQL::REMOVE_ATTRIB(field) =\
                        system_clock::from_time_t(std::mktime(&tm)) +
                        current_zone()->get_info(time_p()).offset;
            }
            else
            {
                ::SQL::REMOVE_ATTRIB(field) =\
                    row[std::string{name}].as<type_dec>();
            }
        };
        // The C++ "brute force" macro
        (func(field_name<T, Is>, get_field<Is>(t.second)), ...);
    }


    template <typename T, std::size_t... Is, field_referenceable U = std::remove_cvref_t<T>>
    static constexpr void fillSQLCreateTable(std::string& str, std::string& reference, std::index_sequence<Is...>)
    {
        // Convert c++ representation to SQL
        auto func = [&str, &reference](const std::string_view& name,
                           const auto& type)
        {
            // Declaring type
            using type_dec = std::remove_cvref<::SQL::getType<decltype(type)>>::type;

            // Work with string
            std::string ending{};

            str += ", ";
            str += std::string{name};

            // SQL Attrib
            if (::SQL::isUnique<decltype(type)>::value)
            {
                ending += " UNIQUE";
            }
            if (::SQL::isNotNull<decltype(type)>::value)
            {
                ending += " NOT NULL";
            }
            if (::SQL::isRef<decltype(type)>::value)
            {
                ending += " INTEGER ";

                reference += std::format(
                    ", FOREIGN KEY({}) REFERENCES {}(id)",
                    std::string{name},
                    type_name<type_dec>
                );
            }

            if (std::is_same_v<type_dec, int> ||
                std::is_same_v<type_dec, std::size_t>)
            {
                str += " INTEGER";
            }
            else if(std::is_same_v<type_dec, std::string>)
            {
                str += " VARCHAR";
            }
            else if(std::is_same_v<type_dec, time_p>)
            {
                str += " TIMESTAMP";
            }
            str += ending;
        };

        // The C++ "brute force" macro
        (func(field_name<T, Is>, field_type<T, Is>()), ...);
    }


};
