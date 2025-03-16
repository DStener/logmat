#include "Test.h"

#include "DTO.hpp"
#include "struct_declaration.hpp"




void Test::TestDB(const HttpRequestPtr& req, callback_func &&callback)
{
    Picture dog{
        "Dog",
        476893,
        "/etc/dog"
    };


    DB::get()->Insert(dog);


    Json::Value ret;

    // ret["message"] = DTO::InsertSQL(dog);

    for(auto& value : DB::get()->Select<Picture>()){
        ret.append(DTO::ToJson(value.second));
    }


    auto resp = HttpResponse::newHttpJsonResponse( ret);
    callback(resp);
}
