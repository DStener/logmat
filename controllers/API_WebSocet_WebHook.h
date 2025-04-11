#pragma once

#include <drogon/WebSocketController.h>
#include <trantor/net/InetAddress.h>
#include <optional>
#include "Request/hookgit.hpp"
		
#include "Webhook.h"
using namespace drogon;

namespace API
{
namespace WebSocet
{
class WebHook : public drogon::WebSocketController<WebHook>
{
public:
    void handleNewMessage(const WebSocketConnectionPtr&,
                                std::   string &&,
                                const WebSocketMessageType &) override;
    void handleNewConnection(const HttpRequestPtr &,
                                    const WebSocketConnectionPtr&) override;
    void handleConnectionClosed(const WebSocketConnectionPtr&) override;
    WS_PATH_LIST_BEGIN
    WS_PATH_ADD("/hooks/message");
    WS_PATH_LIST_END
    
};
}
}
