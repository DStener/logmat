#pragma once

#include <iostream>
#include <drogon/drogon.h>
#include <memory>

#include "DTO.hpp"
#include "struct_declaration.hpp"

using namespace drogon;


class DataBase
{
private:
    orm::DbClientPtr clientPtr;

    DataBase()
    {
        clientPtr = drogon::app().getDbClient("master");

        clientPtr->execSqlAsyncFuture(DTO::CreateTableSQL<User>());
        clientPtr->execSqlAsyncFuture(DTO::CreateTableSQL<Token>());

        clientPtr->execSqlAsyncFuture(DTO::CreateTableSQL<Picture>());
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
    ResponseVec<T> Select()
    {
        auto f = clientPtr->execSqlAsyncFuture("SELECT * FROM " +
                                               DTO::GetName<T>());
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
    void Insert(T& s)
    {
        try
        {
            auto result = clientPtr->execSqlSync(DTO::InsertSQL(s));
        }
        catch (const orm::DrogonDbException &e)
        {
            std::cerr << "error:" << e.base().what() << std::endl;
        }
    }

    ~DataBase() = default;
};

using DB = DataBase;

