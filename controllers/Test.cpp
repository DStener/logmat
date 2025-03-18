#include "Test.h"

#include "DTO.hpp"
#include "struct_declaration.hpp"




void Test::TestDB(const HttpRequestPtr& req, callback_func &&callback)
{
    // Picture dog{
    //     "Dog",
    //     476893,
    //     "/etc/dog"
    // };

    // Picture cat{
    //     "Cat",
    //     789345,
    //     "/etc/cat"
    // };


    // DB::get()->Insert(dog);
    // DB::get()->Insert(cat);

    Json::Value ret;

    // ret["message"] = DTO::InsertSQL(dog);

    for(auto& value : DB::get()->Select<Picture>(WHERE(name == "Dog"))){
        ret.append(DTO::ToJson(value.second));
    }


    auto resp = HttpResponse::newHttpJsonResponse( ret);
    callback(resp);
}
