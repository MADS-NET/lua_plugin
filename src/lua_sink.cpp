/*
  ____  _       _            _             _       
 / ___|(_)_ __ | | __  _ __ | |_   _  __ _(_)_ __  
 \___ \| | '_ \| |/ / | '_ \| | | | |/ _` | | '_ \ 
  ___) | | | | |   <  | |_) | | |_| | (_| | | | | |
 |____/|_|_| |_|_|\_\ | .__/|_|\__,_|\__, |_|_| |_|
                      |_|            |___/         
# A Template for LuaSinkPlugin, a Sink Plugin
# Generated by the command: plugin -t sink -d . lua_sink
# Hostname: Fram-IV.local
# Current working directory: /Users/p4010/Develop/MADS_plugins/lua_plugin
# Creation date: 2024-08-27T13:16:00.326+0200
# NOTICE: MADS Version 1.2.0
*/

// Mandatory included headers
#include <sink.hpp>
#include <nlohmann/json.hpp>
#include <pugg/Kernel.h>
// other includes as needed here
// clang-format off
#include <fstream>
#include <filesystem>
#include <lua.hpp>
#include <lualib.h>
#include <lauxlib.h>
#include <sol/sol.hpp>
#include "utils.hpp"
// clang-format on

// Define the name of the plugin
#ifndef PLUGIN_NAME
#define PLUGIN_NAME "lua_sink"
#endif

// Load the namespaces
using namespace std;
using json = nlohmann::json;


// Plugin class. This shall be the only part that needs to be modified,
// implementing the actual functionality
class LuaSinkPlugin : public Sink<json>, public MADS::LuaPlugin {

public:
  ~LuaSinkPlugin() {
    _lua.collect_garbage();
    _lua.end();
  }

  string kind() override { return PLUGIN_NAME; }


  return_type load_data(json const &input, string topic = "") override {
    _lua["MADS"]["topic"] = topic;
    try {
      _lua["MADS"]["data"] = MADS::to_table(_lua, input);
    } catch (exception &e) {
      cerr << "Error: " << e.what() << endl;
      return return_type::error;
    }
    _deal_with_data(_self);
    return return_type::success;
  }

  void set_params(void const *params) override { 
    Sink::set_params(params);
    _params["script_file"] = "sink.lua";
    _params["search_paths"] = json::array();
    _params.merge_patch(*(json *)params);

    prepare_paths(_params);
    prepare_lua("deal_with_data");
    _deal_with_data = _self["deal_with_data"];
  }

  map<string, string> info() override { 
    return {{"script_file", _script_file}};
  };

private:
  function<void(sol::table)> _deal_with_data;
};



/*
  ____  _             _             _      _                
 |  _ \| |_   _  __ _(_)_ __     __| |_ __(_)_   _____ _ __ 
 | |_) | | | | |/ _` | | '_ \   / _` | '__| \ \ / / _ \ '__|
 |  __/| | |_| | (_| | | | | | | (_| | |  | |\ V /  __/ |   
 |_|   |_|\__,_|\__, |_|_| |_|  \__,_|_|  |_| \_/ \___|_|   
                |___/                                      
Enable the class as plugin 
*/
INSTALL_SINK_DRIVER(LuaSinkPlugin, json)


/*
                  _
  _ __ ___   __ _(_)_ __
 | '_ ` _ \ / _` | | '_ \
 | | | | | | (_| | | | | |
 |_| |_| |_|\__,_|_|_| |_|

For testing purposes, when directly executing the plugin
*/
int main(int argc, char const *argv[]) {
  LuaSinkPlugin plugin;
  json input, params;
  
  params["script_file"] = "sink.lua";

  if (argc > 1) {
    params["script_file"] = argv[1];
  }
  plugin.set_params(&params);

  input["data"] = json::object();
  input["data"]["key1"] = "value1";
  input["data"]["key2"] = json::array({1, 2, 3});

  // Process data
  plugin.load_data(input, "test_topic");

  return 0;
}
