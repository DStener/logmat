#include "database.h"
#include <string>

#include "DTO.hpp"
#include "DTO_declaration.hpp"

DB::DB()
{
    clientPtr = drogon::app().getDbClient("master");

    clientPtr->execSqlAsyncFuture(DTO::CreateTableSQL<User>());
    clientPtr->execSqlAsyncFuture(DTO::CreateTableSQL<Token>());

    clientPtr->execSqlAsyncFuture(DTO::CreateTableSQL<Picture>());

    // clientPtr->execSqlAsyncFuture(DTO::CreateTableSQL<Role>());
    // clientPtr->execSqlAsyncFuture(DTO::CreateTableSQL<Permission>());
    // clientPtr->execSqlAsyncFuture(DTO::CreateTableSQL<UserAndRole>());
    // clientPtr->execSqlAsyncFuture(DTO::CreateTableSQL<RoleAndPermission>());

    // clientPtr->execSqlAsyncFuture(
    //     "CREATE TABLE IF NOT EXISTS Picture"
    //     "("
    //     "  id SERIAL PRIMARY KEY,"
    //     "  name CHARACTER VARYING(30),"
    //     "  size INTEGER,"
    //     "  path CHARACTER VARYING(256)"
    //     ");"
    //     );
    clientPtr->execSqlAsyncFuture(
        "INSERT INTO Picture"
        "(name, size, path) VALUES"
        "('Cat', 20000, '/dir/main'),"
        "('tst', 3454, '/dir/test');"
        );
}

// template<typename T>
// T DB::SelectFrom()
// {
//     auto f = clientPtr->execSqlAsyncFuture("SELECT * FROM $1",
//                                            DTO::GetName<T>());
//     // std::vector<T> list();
//     // try
//     // {
//     //     // list = DTO::CreateFromSQLResult<T>(f.get());
//     // }
//     // catch (const orm::DrogonDbException &e)
//     // {
//     //     // std::cerr << "error:" << e.base().what() << std::endl;
//     // }
//     // return list;
//     return T();
// }



std::string DB::printPictureTable()
{
    auto f = clientPtr->execSqlAsyncFuture(
        "SELECT * FROM Picture; "
    );

    try
    {
        std::stringstream stream;

        auto result = f.get(); // Block until we get the result or catch the exception;
        for (auto row : result)
        {
            stream
                << row["id"].as<std::string>()
                << row["name"].as<std::string>()
                << row["size"].as<std::string>()
                << row["path"].as<std::string>()
                << std::endl;
        }

        return stream.str();
    }
    catch (const orm::DrogonDbException &e)
    {
        return "error";
        // std::cerr << "error:" << e.base().what() << std::endl;
    }

}

// Singelton get insatnce of class DataBase
std::shared_ptr<DB> DB::get()
{
    static std::once_flag flag;
    static std::shared_ptr<DB> instance;
    std::call_once(flag, [&](){ instance.reset(new DB()); });
    return instance;
}



