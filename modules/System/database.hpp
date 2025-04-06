#pragma once

#include <iostream>
#include <drogon/drogon.h>
#include <memory>

#include "System/DTO.hpp"
#include "System/utils.h"
#include "struct_declaration.hpp"
// #include "Migrations.hpp"

#include <boost/algorithm/string.hpp>

using namespace drogon;


class DataBase
{
private:
    orm::DbClientPtr clientPtr;

    DataBase()
    {
        clientPtr = drogon::app().getDbClient( );
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
            vec = DTO::SQL::To<T>(f.get());
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
            // Get old value of field [DIRECT]
            auto orig = DataBase::Select<T>(std::format("id == {}", id))[0].second;

            clientPtr->execSqlSync("BEGIN TRANSACTION;");

            // Delete row [TRANSACTION]
            clientPtr->execSqlSync(
                std::format("DELETE FROM {0} WHERE id == {1};", DTO::GetName<T>(), id));

            ::ChangeLog log {};
            log.name_table = DTO::GetName<T>();
            log.id_row = id;
            log.before = boost::algorithm::replace_all_copy(
                                DTO::SQL::Update(orig), "\"", "'");
            log.after = "";
            log.created_by = 1;
            log.deleted_by = 0;
            log.created_at = std::chrono::system_clock::now();
            log.deleted_at = time_p();

            // Write log [TRANSACTION]
            clientPtr->execSqlSync(DTO::SQL::Insert(log));

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
    id_t Insert(T& s)
    {
        try
        {
            clientPtr->execSqlSync("BEGIN TRANSACTION;");

            // Add new entity [TRANSACTION]
#ifdef WIN32
            id_t id = clientPtr->execSqlSync(DTO::SQL::Insert(s)).front().at("id").as<id_t>();
#else
            id_t id = std::stoi(clientPtr->execSqlSync(DTO::SQL::Insert(s)).front().at("id").c_str());
#endif
            // Write log [TRANSACTION]
            clientPtr->execSqlSync(std::format(
                "INSERT INTO ChangeLog"
                "( name_table, id_row, before, after, created_by, created_at ) VALUES"
                "( \"{0}\", (SELECT COUNT(*) FROM {0}), \"\", \"{1}\", 1, \"{2}\" );",

                DTO::GetName<T>(),
                boost::algorithm::replace_all_copy(DTO::SQL::Update(s), "\"", "'"),
                std::chrono::system_clock::now()
            ));

            clientPtr->execSqlSync("COMMIT;");
            return id;
        }
        catch (const orm::DrogonDbException &e)
        {
            clientPtr->execSqlSync("ROLLBACK;");
            std::cerr << "error:" << e.base().what() << std::endl;
            return 0;
        }
    }

    template<typename T>
    void Update(id_t id, T& t)
    {
        try
        {
            // Get old value of field [DIRECT]
            auto orig = DataBase::Select<T>(std::format("id == {}", id))[0].second;
            DTO::SQL::CheckField::CopyIfNull(orig, t);

            clientPtr->execSqlSync("BEGIN TRANSACTION;");

            // Send update [TRANSACTION]
            clientPtr->execSqlSync(std::format("UPDATE {1} SET {2} WHERE id == {0};",
                                              id, DTO::GetName<T>(),
                                              DTO::SQL::Update(t)));

            ::ChangeLog log {};
            log.name_table = DTO::GetName<T>();
            log.id_row = id;
            log.before = boost::algorithm::replace_all_copy(
                                DTO::SQL::Update(orig), "\"", "'");
            log.after = boost::algorithm::replace_all_copy(
                                DTO::SQL::Update(t), "\"", "'");
            log.created_by = 1;
            log.deleted_by = 0;
            log.created_at = std::chrono::system_clock::now();
            log.deleted_at = time_p();

            // // Write log [TRANSACTION]
            clientPtr->execSqlSync(DTO::SQL::Insert(log));

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
