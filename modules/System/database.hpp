#pragma once

#include <iostream>
#include <drogon/drogon.h>
#include <memory>

#include "System/DTO.hpp"
#include "struct_declaration.hpp"

using namespace drogon;

#define WHERE(condition) #condition

class DataBase
{
private:
    orm::DbClientPtr clientPtr;

    DataBase()
    {
        clientPtr = drogon::app().getDbClient("master");

        this->Create<User>();
        this->Create<Token>();
    }

public:
    DataBase(DataBase const&) = delete;
    DataBase& operator=(DataBase const&) = delete;

    static std::shared_ptr<DataBase> get()
    {
        static std::once_flag flag;
        static std::shared_ptr<DataBase> instance;
        std::call_once(flag, [&](){ instance.reset(new DataBase()); });
        return instance;
    }

    template<typename T>
    void Create()
    {
        try
        {
            clientPtr->execSqlSync(DTO::CreateTableSQL<T>());
        }
        catch (const orm::DrogonDbException &e)
        {
            std::cerr << "error: " << DTO::GetName<T>() << " - " << e.base().what() << std::endl;
            std::cerr << "\t\t" << DTO::CreateTableSQL<T>() << std::endl;
        }
        // return vec;
    }

    template<typename T>
    ResponseVec<T> Select() { return Select<T>(WHERE()); }

    template<typename T>
    ResponseVec<T> Select(std::string condition)
    {
        condition = (condition.size() > 0)? " WHERE " + condition : "";
        auto f = clientPtr->execSqlAsyncFuture("SELECT * FROM " +
                                               DTO::GetName<T>() +
                                               condition);
        ResponseVec<T> vec{};
        try
        {
            vec = DTO::CreateFromSQLResult<T>(f.get());
        }
        catch (const orm::DrogonDbException &e)
        {
            std::cerr << "error:" << e.base().what() << std::endl;
        }
        return vec;
    }

    template<typename T>
    bool Delete(std::string condition)
    {
        try
        {
            condition = (condition.size() > 0)? " WHERE " + condition : "";
            clientPtr->execSqlSync("DELETE FROM " +
                                   DTO::GetName<T>() +
                                   condition);
            return true;
        }
        catch (const orm::DrogonDbException &e)
        {
            std::cerr << "error:" << e.base().what() << std::endl;
            return false;
        }
    }


    template<typename T>
    void Insert(T& s)
    {
        try
        {
            clientPtr->execSqlSync(DTO::InsertSQL(s));
        }
        catch (const orm::DrogonDbException &e)
        {
            std::cerr << "error:" << e.base().what() << std::endl;
        }
    }

    ~DataBase() = default;
};

using DB = DataBase;
