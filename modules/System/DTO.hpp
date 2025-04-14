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
#include <boost/fusion/sequence.hpp>
#include <boost/fusion/mpl.hpp>
#include <boost/core/type_name.hpp>
#include <boost/algorithm/string.hpp>

#include <boost/tokenizer.hpp>
#include <algorithm>
#include <sstream>
#include "struct_declaration.hpp"

#include <drogon/drogon.h>

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

    template <typename T>
    static T ConvertTo(const drogon::HttpRequestPtr& req)
    {
        T ret;
        const std::string_view body = req->getBody();

        DTO::for_each(ret, [&](std::string_view field, auto& value)
        {
            using type_dec = std::remove_cvref_t<decltype(value)>;

            size_t start = body.find(field);
            size_t end = body.find("&", start);

            if(start == std::string_view::npos) { return; }
            if(end == std::string_view::npos) { end = body.size(); }


            std::string content(body.begin() + start + field.size() + 1,
                                body.begin() + end);
            content = drogon::utils::urlDecode(content);


            // Convert from string to var
            if constexpr (std::is_same_v<type_dec, int> ||
                std::is_same_v<type_dec, std::size_t> ||
                std::is_same_v<type_dec, id_t>)
            {
                value = std::stoi(content);
            }
            else if constexpr (std::is_same_v<type_dec, float> ||
                std::is_same_v<type_dec, double>)
            {
                value = std::stod(content);
            }
            else if constexpr (std::is_same_v<type_dec, std::string>)
            {
                value = content;
            }
            else if constexpr (std::is_same_v<type_dec, time_p>)
            {
                using namespace std::chrono;

                time_t rawtime = time(nullptr);
                std::tm time = *localtime(&rawtime);

                std::istringstream stream{ content };
                stream >> std::get_time(&time, "%Y-%m-%d");

                value = system_clock::from_time_t(std::mktime(&time));
            }
        });
        return ret;
    }



    // template <typename T>
    // static T ConvertTo(const drogon::HttpRequestPtr& req)
    // {
    //     using char_tokenizer = boost::tokenizer<boost::char_separator<char>>;
    //     T ret;

    //     boost::char_separator<char> sep{ "&" };
    //     const std::string body{req->getBody()};
    //     char_tokenizer tok{body, sep};

    //     std::cout << req->getBody() << std::endl;

    //     DTO::for_each(ret, [&tok](std::string_view field, auto& value)
    //     {
    //         using type_dec = std::remove_cvref_t<decltype(value)>;
    //         const std::string name{ field };

    //         char_tokenizer::iterator it =\
    //             std::find_if(tok.begin(), tok.end(), [&name](const std::string& str)
    //             {
    //                     return str.starts_with(name);
    //             });


    //         if (it == tok.end() && !(*it).starts_with(name)) { return; }

    //         // Get value before '='
    //         std::string_view _temp = (*it).substr((*it).find('=') + 1);
    //         std::string content(drogon::utils::urlDecode(_temp));

    //         //LOG_INFO << (*it).starts_with(name) << *it ;

    //         // Convert from string to var
    //         if constexpr (std::is_same_v<type_dec, int> ||
    //             std::is_same_v<type_dec, std::size_t> ||
    //             std::is_same_v<type_dec, id_t>)
    //         {
    //             value = std::stoi(content);
    //         }
    //         else if constexpr (std::is_same_v<type_dec, float> ||
    //             std::is_same_v<type_dec, double>)
    //         {
    //             value = std::stod(content);
    //         }
    //         else if constexpr (std::is_same_v<type_dec, std::string>)
    //         {
    //             value = content;
    //         }
    //         else if constexpr (std::is_same_v<type_dec, time_p>)
    //         {
    //             using namespace std::chrono;

    //             time_t rawtime = time(nullptr);
    //             std::tm time = *localtime(&rawtime);

    //             std::istringstream stream{ content };
    //             stream >> std::get_time(&time, "%Y-%m-%d");

    //             value = system_clock::from_time_t(std::mktime(&time));
    //         }
    //     });
    //     return ret;
    // }

    /////////////////////////// JSON METHODS ///////////////////////////////////
    class JSON {
    public:
        // Convert DTO to Json::Value
        template <typename T>
        static Json::Value From(T s)
        {
            return DTO::JSON::Fill(s);
        }

        // Convert pair<id_t, DTO> to Json::Value
        template <typename T>
        static Json::Value From(DTORow<T> s)
        {
            Json::Value json = DTO::JSON::Fill(s.second);
            json["id"] = s.first;

            return json;
        }
    private:
        template <typename T>
        static Json::Value Fill(T& t)
        {
            Json::Value json(Json::objectValue);

            DTO::for_each(t, [&json](std::string_view field, auto value)
            {
                if(DTO::SQL::CheckField::isSecret(field)) { return; }

                // ERROR: if convert directly to std::string<
                // without std::stringstream
                std::stringstream stream;
                stream << value;

                // json.append(std::string(field), );
                json[std::string(field)] = Json::Value(stream.str());
                // json.append()
            });
            return json;
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

            return std::format("INSERT OR REPLACE INTO {2} ( {0} ) VALUES ( {1} ) RETURNING id;",
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
#ifdef WIN32
                        stream >> std::chrono::parse("%Y-%m-%d %H:%M:%S", field);
#else
                        time_t rawtime = time(nullptr);
                        std::tm tm = *localtime(&rawtime);

                        stream >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");

                        field = system_clock::from_time_t(std::mktime(&tm)) +
                                current_zone()->get_info(time_p()).offset;

                        if (str.rfind('.') != std::string::npos)
                        {
                            field += nanoseconds(std::stoi(str.substr(str.rfind('.') + 1)));
                        }

#endif
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
                using namespace boost::algorithm;

                return std::format("\"{}\"", 
                                   replace_all_copy(stream.str(), "\"", "'"));
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

                return field.starts_with("_") ||
                       field == sv("password") ||
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
