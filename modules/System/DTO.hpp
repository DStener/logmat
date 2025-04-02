#pragma once

#include "System/utils.h"


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

#include <boost/fusion/adapted.hpp>
#include <boost/fusion/mpl.hpp>
#include <boost/core/type_name.hpp>

class DTO
{
public:
    ///////////////////////// MAIN DTO METHOD //////////////////////////////////
    template <typename T, typename Func>
    static void for_each(T& t, Func f)
    {
        using size = boost::fusion::extension::struct_size<T>;
        DTO::FOREACH(t, f, std::make_index_sequence<size::value>());
    }


    // Get DTO struct name
    template <typename T>
    static std::string GetName()
    {
        return std::string{boost::core::type_name<T>()};
    }

    /////////////////////////// JSON METHODS ///////////////////////////////////
    class JSON {
    public:
        // Convert DTO to Json::Value
        template <typename T>
        static Json::Value From(T s)
        {
            Json::Value json;

            DTO::JSON::Fill(s, json);

            return json;
        }

        // Convert pair<id_t, DTO> to Json::Value
        template <typename T>
        static Json::Value From(DTORow<T> s)
        {
            Json::Value json;
            json["id"] = s.first;

            DTO::JSON::Fill(s.second, json);

            return json;
        }
    private:
        template <typename T>
        static void Fill(T& t, Json::Value& json)
        {
            DTO::for_each(t, [&json](std::string_view field, auto value)
            {
                if(DTO::SQL::CheckField::isSecret(field)) { return; }

                // ERROR: if convert directly to std::string<
                // without std::stringstream
                std::stringstream stream;
                stream << value;

                json[std::string(field)] = stream.str();
            });
        }
    };

    //////////////////////// RequestBody METHODS ///////////////////////////////
    class RequestBody {
    public:
        // Convert request Body to DTO
        template <typename T>
        static T To(const std::string_view& str)
        {
            T ret;
            DTO::for_each(ret, [&str](std::string_view field, auto& value)
            {
                using type_dec = std::remove_cvref_t<decltype(value)>;

                size_t start = str.find(field);
                if (start == std::string_view::npos) { return; }

                start +=  field.size() + 1;
                size_t size = str.find("&", start) - start;

                auto substr = drogon::utils::urlDecode( API::Utils::trim(
                                    std::string{str.substr(start, size)}));
                if constexpr (std::is_same_v<type_dec, int> ||
                              std::is_same_v<type_dec, std::size_t> ||
                              std::is_same_v<type_dec, id_t>)
                {
                    value = std::stoi(substr);
                }
                else if constexpr(std::is_same_v<type_dec, std::string>)
                {
                    value = substr;
                }
                else if constexpr(std::is_same_v<type_dec, time_p>)
                {
                    using namespace std::chrono;

                    time_t rawtime = time(nullptr);
                    std::tm time = *localtime(&rawtime);

                    std::istringstream stream{substr};
                    stream >> std::get_time(&time, "%Y-%m-%d");

                    value = system_clock::from_time_t(std::mktime(&time));
                }
            });
            return ret;
        }

    };

    ///////////////////////////// SQL METHODS //////////////////////////////////
    class SQL {
    public:

        // Return SQL command to insert to Teable DTO strucr T
        template <typename T>
        static std::string Insert(T& s)
        {
            std::string values{};
            std::string fields{};

            DTO::for_each(s, [&fields, &values](std::string_view field, auto& value)
            {
                fields += std::format("{}, ", field);
                values += std::format("{}, ", DTO::SQL::to_string(value));
            });

            return std::format("INSERT OR IGNORE INTO {2} ( {0} ) VALUES ( {1} );",
                               fields.substr(0,fields.size() - 2),
                               values.substr(0,values.size() - 2),
                               DTO::GetName<T>());
        }

        template <typename T>
        static std::string Update(T& s)
        {
            std::string str{};

            DTO::for_each(s, [&str](std::string_view field, auto& value)
            {
                str += std::format("{} = {}, ", field, DTO::SQL::to_string(value));
            });

            return str.substr(0, str.size() - 2);
        }

        // Convert orm::Result (SQL Response) to DTO
        template <typename T>
        static ResponseVec<T> To(const drogon::orm::Result& result)
        {
            ResponseVec<T> vec{};
            for(const auto& row : result)
            {
                DTORow<T> _cash{row.at("id").as<id_t>(), T{}};
                DTO::for_each(_cash.second, [&row](std::string_view name, auto& field)
                {
                    using type_dec = std::remove_cvref_t<decltype(field)>;

                    if constexpr (std::is_same_v<type_dec, time_p>)
                    {
                        using namespace std::chrono;

                        std::string str = row[std::string{ name }].c_str();
                        std::istringstream stream{ str };
#ifdef _WIN32
                        stream >> std::chrono::parse("%Y-%m-%d %H:%M:%S", field);
#else
                        time_t rawtime = time(nullptr);
                        std::tm tm = *localtime(&rawtime);

                        
                        
                        stream >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");

                        field = system_clock::from_time_t(std::mktime(&tm)) +
                            nanoseconds(std::stoi(str.substr(str.rfind('.') + 1))) +
                            current_zone()->get_info(time_p()).offset;
#endif
                        
                        std::cout << str << " - " << field << std::endl;
                    }
                    else
                    {
                        field = row[std::string{name}].as<type_dec>();
                    }
                });

                vec.push_back(_cash);
            }
            return vec;
        }

        template <typename T>
        static std::string to_string(T& t)
        {
            using type_dec = std::remove_cvref_t<decltype(t)>;

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
            static void CopyIfNull(T& from, T& to)
            {

                using size = boost::fusion::extension::struct_size<T>;
                auto func = [](std::string_view field, auto& from, auto& to)
                {
                    if(!DTO::SQL::CheckField::isNull(to)) { return; }
                    to = from;
                };

                DTO::FOREACH_TWO(from, to, func,
                                 std::make_index_sequence<size::value>());
            }
            // template <typename T>
            // static bool NotNull(T& t)
            // {
            //     bool flag = false;
            //     DTO::for_each(t, [&flag](std::string_view field, auto& value)
            //     {
            //         using type = const std::remove_cvref_t<decltype(value)>&;

            //         if (!::SQL::isNotNull<type>::value) { return; }

            //         flag |= isNull(value);
            //     });
            //     return !flag;
            // }

            // template <typename T>
            // static std::string UpdateNotNull(T& s)
            // {
            //     std::string sql{};
            //     DTO::for_each(s, [&sql](std::string_view field, auto& value)
            //     {
            //         // std::cout << std::format("{}: [{}] = {}", isNull(value), field, DTO::SQL::to_string(value)) << std::endl;
            //         if (isNull(value)) { return; }
            //         sql += std::format("{} = {}, ", field, DTO::SQL::to_string(value));
            //     });
            //     // if (sql.size() == 0) return sql;
            //     return sql.substr(0,sql.size() - 2);
            //     // return  std::string_view{"sql"};
            // }

            // template <typename T>
            // static std::string UniqueSQL(T& t)
            // {
            //     std::string sql;
            //     DTO::for_each(t, [&sql](std::string_view field, auto& value)
            //     {
            //         using type = const std::remove_cvref_t<decltype(value)>&;

            //         // if (!::SQL::isUnique<type>::value) { return; }

            //         sql += std::format(" {} == {} OR ", field, DTO::SQL::to_string(value));
            //     });
            //     return sql.substr(0,sql.size() - 3);
            // }

            template <typename T>
            static bool isNull(T& t)
            {
                using type_dec = std::remove_cvref_t<decltype(t)>;

                if constexpr (std::is_same_v<type_dec, int> ||
                              std::is_same_v<type_dec, std::size_t> ||
                              std::is_same_v<type_dec, id_t>)
                {
                    return (t == 0);
                }
                else if constexpr(std::is_same_v<type_dec, std::string>)
                {
                    return (t == "" || t == " ");
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
    template <typename T, typename Func, std::size_t... I>
    static constexpr void FOREACH(T& t, Func f, std::index_sequence<I...>)
    {
        (
            f(boost::fusion::extension::struct_member_name<T, I>::call(),
              boost::fusion::at_c<I>(t)),
        ...);
    }
    template <typename T, typename Func, std::size_t... I>
    static constexpr void FOREACH_TWO(T& t1, T& t2, Func f, std::index_sequence<I...>)
    {
        (
            f(boost::fusion::extension::struct_member_name<T, I>::call(),
              boost::fusion::at_c<I>(t1), boost::fusion::at_c<I>(t2)),
        ...);
    }
};
