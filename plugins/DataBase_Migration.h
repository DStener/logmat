#pragma once

#include <filesystem>
#include <fstream>

#include <drogon/plugins/Plugin.h>
#include <drogon/drogon.h>

namespace DataBase
{

class Migration : public drogon::Plugin<Migration>
{
  public:
    Migration() {}
    /// This method must be called by drogon to initialize and start the plugin.
    /// It must be implemented by the user.
    void initAndStart(const Json::Value &config) override;

    /// This method must be called by drogon to shutdown the plugin.
    /// It must be implemented by the user.
    void shutdown() override;
};

}
