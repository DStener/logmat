#pragma once

#include <drogon/drogon.h>
#include <memory>
#include "DTO.hpp"

#include "DTO_declaration.hpp"

using namespace drogon;


class DB
{
private:
    orm::DbClientPtr clientPtr;
    DB();
public:
    DB(DB const&) = delete;
    DB& operator=(DB const&) = delete;

    static std::shared_ptr<DB> get();

    std::string printPictureTable();

    // template<typename T>
    // T SelectFrom();


    ~DB() = default;
};


namespace DataBaseBBBBB {
void testBEST()
{
    LOG_INFO << "START BV";
}
};
