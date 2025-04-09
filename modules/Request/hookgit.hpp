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
            "git reset --hard",
            "git checkout origin/main",
            "git pull",
            "cmake -B build .",
            "cd ./build",
            "cmake --build . --clean-first --config Release"
        };

        for (const auto& cmd : commands)
        {
            auto log_cmd = std::format("> {}", cmd);
            auto _temp = HookGit::exec(cmd.data());

            LOG_INFO << _temp;
            wsConnPtr->send(utils::urlEncode(_temp));

        }
    }
private:
    static std::string exec(const char* cmd) {
        std::array<char, 128> buffer;
        std::string result;
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
        if (!pipe) {
            throw std::runtime_error("popen() failed!");
        }
        while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr) {
            result += buffer.data();
        }
        return result;
    }
};
}
