#include "Webhook.h"
#include "Request/login.hpp"

std::optional<trantor::InetAddress> Hooks::user_addres;

void Hooks::GitUpdate(const HttpRequestPtr& req, callback_func&& callback)
{
	auto webhook = DTO::ConvertTo<::WebHook>(req);
	Json::Value json;

    if (webhook.secret_key != drogon::app().getCustomConfig()["secret_key"].asString())
	{
		json["message"] = "Некорректный secret_key";

		auto response = HttpResponse::newHttpJsonResponse(json);
		response->setStatusCode(drogon::k406NotAcceptable);
		callback(response);

		return;
	}

    if (user_addres.has_value())
    {
        json["message"] = "Обновление уже выполняется иным юзером";

        auto response = HttpResponse::newHttpJsonResponse(json);
        response->setStatusCode(drogon::k406NotAcceptable);
        callback(response);

        return;
    }

    // Set user ip addres
    user_addres.emplace(req->getPeerAddr());

	json["message"] = "Переключение на WebSocket";
	json["websocet_url"] = "/hooks/message";

	auto response = HttpResponse::newHttpJsonResponse(json);
	callback(response);
}
