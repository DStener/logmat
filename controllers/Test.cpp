#include "Test.h"

#include "DTO.hpp"
#include "DTO_declaration.hpp"




void Test::TestDB(const HttpRequestPtr& req, callback_func &&callback)
{
    Json::Value ret;
    // Json::Value::


    // ret["message"] = DB::get()->printPictureTable();
    // ret["message"] = DTO::GetName(t);
    // ret["message"] = DTO::CreateTableSQL<Role>();
    // ret["message"] = DTO::GetName<ref<User>>();
    // DB::get()->SelectFrom<Picture>();

    // for(DTORow<Picture> value : DB::get()->SelectFrom<Picture>()){
    //     Json::Value _cash;
    //     std::stringstream stream;
    //     stream << value.second.name;
    //     _cash["name"] = stream.str();
    //     // ret.append(DTO::ToJson(value.second));
    //     ret.append(_cash);
    // }


    auto resp = HttpResponse::newHttpJsonResponse( ret);
    callback(resp);
}
