#include <stdio.h>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>

#include "config.h"

#include <drogon/drogon.h>

#ifdef WIN32
#define pclose _pclose;
#define popen _popen;
#define pclose _pclose;
#endif


#include <drogon/WebSocketController.h>

using namespace drogon;

namespace Request {
class HookGit {
private:

public:
    static void Update(const WebSocketConnectionPtr& wsConnPtr)
    {
        static std::vector<std::string> commands {
            std::format("cd {}", CMAKE_CURRENT_SOURCE_DIR),
            "sh setup.sh --update",
            "exec ./build/bash"
        };

        std::array<char, 128> buffer;

        for (const auto& cmd : commands)
        {
            std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.data(), "r"), pclose);

            if (!pipe) {
                throw std::runtime_error("popen() failed!");
            }
            while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr) {
                wsConnPtr->send(utils::base64Encode(buffer.data()));
            }
        }
        wsConnPtr->send("RELAUNCH");
        drogon::app().quit();
    }
};
}
