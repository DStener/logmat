#pragma once

#include <iostream>
#include <drogon/drogon.h>
#include <memory>

#include "System/DTO.hpp"
#include "System/utils.h"
// #include "struct_declaration.hpp"
// #include "Migrations.hpp"

// Migration
#include "Migration/User.hpp"
#include "Migration/Token.hpp"
#include "Migration/Role.hpp"
#include "Migration/Permission.hpp"
#include "Migration/RoleAndPermission.hpp"
#include "Migration/UserAndRole.hpp"
#include "Migration/ChangeLog.hpp"
#include "Migration/Image.hpp"
#include "Migration/Review.hpp"
#include "Migration/Captcha.hpp"

using namespace drogon;


class DataBase
{
private:
    orm::DbClientPtr clientPtr;

    DataBase()
    {
        clientPtr = drogon::app().getDbClient("master");

        Migration::User::up();
        Migration::Token::up();

        Migration::Role::up();
        Migration::Permission::up();

        Migration::RoleAndPermission::up();
        Migration::UserAndRole::up();

        Migration::ChangeLog::up();

        // Web ldb
        Migration::Image::up();
        Migration::Review::up();
        Migration::Captcha::up();
    }

public:
    DataBase(DataBase const&) = delete;
    DataBase& operator=(DataBase const&) = delete;

    template<typename T>
    ResponseVec<T> Select() { return Select<T>(""); }

    template<typename T>
    ResponseVec<T> Select(std::string condition)
    {
        auto f  = clientPtr->execSqlAsyncFuture(
            std::format("SELECT * FROM {0} {2} {1};",
                        DTO::GetName<T>(), condition,
                        ((condition.size() > 0)? "WHERE" : ""))
        );

        ResponseVec<T> vec{};
        try
        {
            vec = DTO::CreateFromSQLResult<T>(f.get());
        }
        catch (const orm::DrogonDbException &e)
        {
            std::cerr << "error:" << e.base().what() << std::endl;
            std::cerr << "\t\t" <<
                std::format("SELECT * FROM {0} {2} {1};",
                            DTO::GetName<T>(), condition,
                            ((condition.size() > 0)? "WHERE" : "")) <<
                std::endl;
        }
        return vec;
    }

    template<typename T>
    bool Delete(id_t id)
    {
        try
        {
            // Get old value of field
            auto row = clientPtr->execSqlSync(
                std::format("SELECT * FROM {0} WHERE id == {1};", DTO::GetName<T>(), id))[0];

            clientPtr->execSqlSync("BEGIN TRANSACTION;");

            for(auto& field : row)
            {
                // Fill ChangeLog
                ::ChangeLog log;
                SQL::REMOVE_ATTRIB(log.name_table) = DTO::GetName<T>();
                SQL::REMOVE_ATTRIB(log.id_row) = 0;
                SQL::REMOVE_ATTRIB(log.name_field) = field.name();
                SQL::REMOVE_ATTRIB(log.before) = field.c_str();
                SQL::REMOVE_ATTRIB(log.after) = "";
                SQL::REMOVE_ATTRIB(log.created_by) = 1;
                SQL::REMOVE_ATTRIB(log.created_at) = std::chrono::system_clock::now();
                SQL::REMOVE_ATTRIB(log.deleted_by) = 0;
                SQL::REMOVE_ATTRIB(log.deleted_at) = time_p();

                // Insert log to ChangeLog
                clientPtr->execSqlSync(DTO::InsertSQL(log));
            }

            clientPtr->execSqlSync(
                std::format("DELETE FROM {0} WHERE id == {1};", DTO::GetName<T>(), id));

            clientPtr->execSqlSync("COMMIT;");

            return true;
        }
        catch (const orm::DrogonDbException &e)
        {
            clientPtr->execSqlSync("ROLLBACK;");
            std::cerr << "error:" << e.base().what() << std::endl;
            return false;
        }
    }


    template<typename T>
    void Insert(T& s)
    {
        try
        {
            clientPtr->execSqlSync("BEGIN TRANSACTION;");
            std::string cmd = DTO::InsertSQL(s);

            size_t start_fields = cmd.find('(');
            size_t end_fields = cmd.find(')');
            size_t start_values = cmd.find('(', start_fields + 1);
            size_t end_values = cmd.find(')', end_fields + 1);

            size_t idx_f{start_fields};
            size_t idx_v{start_values};

            while(idx_f < end_fields) {

                size_t end_f = cmd.find(",", idx_f + 1);
                size_t end_v = cmd.find(",", idx_v + 1);


                // Fill ChangeLog
                ::ChangeLog log;
                SQL::REMOVE_ATTRIB(log.name_table) = DTO::GetName<T>();
                SQL::REMOVE_ATTRIB(log.id_row) = 0;
                SQL::REMOVE_ATTRIB(log.name_field) =\
                    API::Utils::trim(cmd.substr(idx_f + 1, std::min(end_f, end_fields) - idx_f - 1));

                SQL::REMOVE_ATTRIB(log.before) = "";
                SQL::REMOVE_ATTRIB(log.after) =\
                    API::Utils::trim(cmd.substr(idx_v + 1, std::min(end_v, end_values) - idx_v - 1));

                SQL::REMOVE_ATTRIB(log.created_by) = 1;
                SQL::REMOVE_ATTRIB(log.created_at) = std::chrono::system_clock::now();
                SQL::REMOVE_ATTRIB(log.deleted_by) = 0;
                SQL::REMOVE_ATTRIB(log.deleted_at) = time_p();


                // Insert log to ChangeLog
                clientPtr->execSqlSync(DTO::InsertSQL(log));

                idx_f = end_f;
                idx_v = end_v;
            }
            clientPtr->execSqlSync(cmd);
            clientPtr->execSqlSync("COMMIT;");
        }
        catch (const orm::DrogonDbException &e)
        {
            clientPtr->execSqlSync("ROLLBACK;");
            std::cerr << "error:" << e.base().what() << std::endl;
        }
    }

    template<typename T>
    void Update(id_t id, std::string set)
    {
        try
        {
            // Get old value of field
            auto res = clientPtr->execSqlSync(
                std::format("SELECT * FROM {0} WHERE id == {1};", DTO::GetName<T>(), id));

            clientPtr->execSqlSync("BEGIN TRANSACTION;");

            std::string cmd = std::format("UPDATE {0} SET {1} WHERE id == {2};",
                                          DTO::GetName<T>(), set, id);

            size_t index{0};
            while (true)
            {
                size_t end = set.find(',', index + 1);
                size_t center = set.find('=', index + 1);

                // Fill ChangeLog
                ::ChangeLog log;
                SQL::REMOVE_ATTRIB(log.name_table) = DTO::GetName<T>();
                SQL::REMOVE_ATTRIB(log.id_row) = id;
                SQL::REMOVE_ATTRIB(log.name_field) =\
                    API::Utils::trim(set.substr(index, std::min(center, set.size()) - index - 1));

                SQL::REMOVE_ATTRIB(log.before) =\
                    res[0][SQL::REMOVE_ATTRIB(log.name_field).data()].c_str();
                SQL::REMOVE_ATTRIB(log.after) =\
                    API::Utils::trim(set.substr(center + 1, std::min(end, set.size()) - center - 1));

                SQL::REMOVE_ATTRIB(log.created_by) = 1;
                SQL::REMOVE_ATTRIB(log.created_at) = std::chrono::system_clock::now();
                SQL::REMOVE_ATTRIB(log.deleted_by) = 0;
                SQL::REMOVE_ATTRIB(log.deleted_at) = time_p();

                clientPtr->execSqlSync(DTO::InsertSQL(log));

                index = end + 1;
                if(end == std::string::npos || center == std::string::npos) { break; }
            }
            clientPtr->execSqlSync(cmd);
            clientPtr->execSqlSync("COMMIT;");
        }
        catch (const orm::DrogonDbException &e)
        {
            clientPtr->execSqlSync("ROLLBACK;");
            std::cerr << "error:" << e.base().what() << std::endl;
        }
    }


    static std::shared_ptr<DataBase> get()
    {
        static std::once_flag flag;
        static std::shared_ptr<DataBase> instance;
        std::call_once(flag, [&](){ instance.reset(new DataBase()); });
        return instance;
    }

    ~DataBase() = default;
private:
};

using DB = DataBase;
