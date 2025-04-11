#include <stdio.h>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <cstdlib>

#include "config.h"

#include <drogon/drogon.h>
#include <fstream>

#ifdef WIN32
#define pclose _pclose;
#define popen _popen;
#define pclose _pclose;
#endif


#include <drogon/WebSocketController.h>
#include "System/database.hpp"
#include "Webhook.h"

using namespace drogon;

namespace Request {
class HookGit {
private:

public:
    static void Update(const WebSocketConnectionPtr& wsConnPtr)
    {
        std::array<char, 128> buffer;
        std::ofstream log;

        // Fill DTO
        ::File FileDB{};
        FileDB.name = std::format("Log {}.txt", std::chrono::system_clock::now());
        FileDB.description = "LOG FILE OF UPDATE";
        FileDB.tags = "txt|log";
        FileDB._path = std::format("{}/{}", app().getUploadPath(), FileDB.name);
        FileDB._avatar_path = "/img/file/txt.png";


        // Open new log file
        log.open(FileDB._path);

        // Write log header
        if(log.is_open()){
            log << Hooks::user_addres.value().toIp() << " - "
                << std::chrono::system_clock::now()
                << "\n-=-=-=-=-=-=-=-=-=-=-=-=--\n";
        }


        // Open new pipe
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(std::format("sh {0}/setup.sh {0} --update", CMAKE_CURRENT_SOURCE_DIR).data(), "r"), pclose);

        if (!pipe) {
            throw std::runtime_error("popen() failed!");
        }
        while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr) {
            wsConnPtr->send(utils::base64Encode(buffer.data()));
            if(log.is_open()){
                log << buffer.data();
            }
        }

        // Close log file and write his size
        log.close();
        FileDB.size = std::filesystem::file_size(FileDB._path);


        // Upload file info to DB and get id row
        id_t id = DB::get()->Insert(FileDB);

        LOG_INFO << id;
        // Start updates version
        std::system(std::format("nohup {}/build/logmat & > /dev/null", CMAKE_CURRENT_SOURCE_DIR).data());

        // Send End message
        wsConnPtr->send(utils::base64Encode(std::format("%RELAUNCH-{}", id)));

        // terminated
        drogon::app().quit();
    }
};
}
