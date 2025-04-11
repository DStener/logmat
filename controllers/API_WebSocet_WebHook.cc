#include "API_WebSocet_WebHook.h"

using namespace API;

static std::optional<trantor::InetAddress> addres;

void WebSocet::WebHook::handleNewMessage(const WebSocketConnectionPtr& wsConnPtr, std::string &&message, const WebSocketMessageType &type)
{
    LOG_INFO << "NEW MESSAGE: " << message;
}

void WebSocet::WebHook::handleNewConnection(const HttpRequestPtr &req, const WebSocketConnectionPtr& wsConnPtr)
{

    if (addres.has_value())
    {
        wsConnPtr->shutdown(CloseCode::kNormalClosure, "Обновление уже выполняется другим пользователем");
        return;
    }

    // Set user address
    addres.emplace(req->getLocalAddr());
    wsConnPtr->send(utils::base64Encode("START UPDATE\n\n"));


    Request::HookGit::Update(wsConnPtr);

    wsConnPtr->send(utils::base64Encode("END UPDATE\n\n"));
}

void WebSocet::WebHook::handleConnectionClosed(const WebSocketConnectionPtr& wsConnPtr)
{
    LOG_INFO << "CLOSE CONNECT";
    addres.reset();
}
