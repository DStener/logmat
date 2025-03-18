#pragma once

#include "field_reflection.hpp"
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
using time_p = std::chrono::utc_clock::time_point;

template <typename T>
using DTORow = std::pair<std::size_t,T>;
template <typename T>
using ResponseVec = std::vector<DTORow<T>>;

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
        std::string sql{"INSERT INTO"};
        sql += " " + DTO::GetName<T>();
        sql += " ( ";

        std::string values{};

        for_each_field(s, [&sql, &values](std::string_view field, auto& value)
        {
            using type_dec = std::remove_cvref_t<SQL::getType<decltype(value)>>;

            // ERROR: if convert directly to std::string<
            // without std::stringstream
            std::stringstream stream;
            stream << value;

            sql += std::string{field} + ", ";

            if constexpr (std::is_same_v<type_dec, std::string> ||
                          std::is_same_v<type_dec, time_p>)
            {
                values += std::format("\"{}\", ", stream.str());
            }
            else
            {
                values += std::format("{}, ", stream.str());
            }
        });

        sql = sql.substr(0,sql.size() - 2) + " ) VALUES ( " + values.substr(0,values.size() - 2) + " );";
        return sql;
    }

    // Convert request Body to DTO
    template <typename T, field_referenceable U = std::remove_cvref_t<T>>
    static T CreateFromRequestBody(const std::string_view& str)
    {
        T ret;
        DTO::fill(ret, str, std::make_index_sequence<field_count<U>>());
        return ret;
    }

    // Convert orm::Result (SQL Response) to DTO
    template <typename T, field_referenceable U = std::remove_cvref_t<T>>
    static ResponseVec<T> CreateFromSQLResult(const drogon::orm::Result& result)
    {
        ResponseVec<T> vec{};
        for(const auto& row : result)
        {
            DTORow<T> _cash{row.at("id").as<size_t>(), T()};
            DTO::fillFromSQLResult(_cash, row, std::make_index_sequence<field_count<U>>());
            vec.push_back(_cash);
        }
        return vec;
    }


private:
    // Fill function ()
    template <typename T, std::size_t... Is, field_referenceable U = std::remove_cvref_t<T>>
    static constexpr void fill(T& t,const std::string_view& str, std::index_sequence<Is...>)
    {
        // The function of searching for values in the request body
        // and assigning them to the DTO
        auto func = [&str](const std::string_view& name, auto& field)
        {
            using type_dec = std::remove_cvref_t<SQL::getType<decltype(field)>>;

            size_t start = str.find(name) + name.size() + 1;
            size_t size = str.find("&", start) - start;

            // field = std::string();
            auto substr = std::string{str.substr(start, size)};
            if constexpr (std::is_same_v<type_dec, int> ||
                          std::is_same_v<type_dec, std::size_t>)
            {
                field = std::stoi(substr);
            }
            else if constexpr(std::is_same_v<type_dec, std::string>)
            {
                field = substr;
            }
            else if constexpr(std::is_same_v<type_dec, time_p>)
            {
                std::istringstream{substr} >> std::chrono::parse("%Y-%m-%d", field);
            }
        };
        // The C++ "brute force" macro
        (func(field_name<T, Is>, get_field<Is>(t)), ...);
    }


    // Fill from SQL Result
    template <typename T, std::size_t... Is, field_referenceable U = std::remove_cvref_t<T>>
    static constexpr void fillFromSQLResult(DTORow<T>& t, const drogon::orm::Row& row, std::index_sequence<Is...>)
    {
        // The function of convert Row::Field to DTO::field
        auto func = [&row](const std::string_view& name, auto& field)
        {
            using type_dec = std::remove_cvref_t<SQL::getType<decltype(field)>>;

            if constexpr (SQL::isRef<const std::remove_cvref_t<decltype(field)>&>::value)
            {
                SQL::REMOVE_ATTRIB(field) =\
                    row[std::string{name}].as<size_t>();
            }
            else if constexpr (std::is_same_v<type_dec, time_p>)
            {
                std::istringstream{row[std::string{name}].c_str()} >>\
                    std::chrono::parse("%Y-%m-%d %H:%M:%S", SQL::REMOVE_ATTRIB(field));
            }
            else
            {
                SQL::REMOVE_ATTRIB(field) =\
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
            using type_dec = std::remove_cvref<SQL::getType<decltype(type)>>::type;

            // Work with string
            std::string ending{};

            str += ", ";
            str += std::string{name};

            // SQL Attrib
            if (SQL::isUnique<decltype(type)>::value)
            {
                ending += " UNIQUE";
            }
            if (SQL::isNotNull<decltype(type)>::value)
            {
                ending += " NOT NULL";
            }
            if (SQL::isRef<decltype(type)>::value)
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
