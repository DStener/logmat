#include "Webhook.h"
#include "Request/login.hpp"

void Hooks::GitUpdate(const HttpRequestPtr& req, callback_func&& callback)
{
	auto login = Request::Login(req, callback);
	if (!login.id) { return; }

	auto webhook = DTO::ConvertTo<::WebHook>(req);
	Json::Value json;

	if (webhook.secret_key != "null")
	{
		json["message"] = "Некорректный secret_key";

		auto response = HttpResponse::newHttpJsonResponse(json);
		response->setStatusCode(drogon::k406NotAcceptable);
		callback(response);

		return;
	}

	json["message"] = "Переключение на WebSocket";
	json["websocet_url"] = "/hooks/message";

	auto response = HttpResponse::newHttpJsonResponse(json);
	callback(response);
}