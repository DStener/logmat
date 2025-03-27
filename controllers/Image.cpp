#include "Image.h"

#include "System/database.hpp"
#include "System/DTO.hpp"
#include "struct_declaration.hpp"


void Data::Image::GetList(const HttpRequestPtr& req, callback_func &&callback)
{
    auto images = DB::get()->Select<::Image>();

    Json::Value json;
    for(auto& row : images)
    {
        json.append(DTO::ToJson(row));
    }

    auto response = HttpResponse::newHttpJsonResponse(json);
    callback(response);
}

void Data::Image::Get(const HttpRequestPtr& req, callback_func &&callback,
         id_t id_image)
{
    auto images = DB::get()->Select<::Image>(std::format("id == {}", id_image));

    // If no images are found
    if(!images.size())
    {
        auto response = HttpResponse::newHttpResponse();
        response->setStatusCode(drogon::k404NotFound);
        callback(response);
    }

    auto response = HttpResponse::newFileResponse(
                SQL::REMOVE_ATTRIB(images[0].second.path));
    callback(response);
}
