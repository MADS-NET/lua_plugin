#include <filesystem>
#include <lauxlib.h>
#include <lua.hpp>
#include <lualib.h>
#include <nlohmann/json.hpp>
#include <sol/sol.hpp>

#if defined(_WIN32)
#include <Shlwapi.h>
#include <io.h>
#include <windows.h>
#define PATH_MAX MAX_PATH
#define access _access_s
#endif

#ifdef __APPLE__
#include <libgen.h>
#include <limits.h>
#include <mach-o/dyld.h>
#include <unistd.h>
#endif

#ifdef __linux__
#include <libgen.h>
#include <limits.h>
#include <unistd.h>

#if defined(__sun)
#define PROC_SELF_EXE "/proc/self/path/a.out"
#else
#define PROC_SELF_EXE "/proc/self/exe"
#endif

#endif

#ifndef INSTALL_PREFIX
#define INSTALL_PREFIX "/usr/local"
#warning "INSTALL_PREFIX not defined, using /usr/local"
#endif

#define LUA_ADD_PATH(lua, p)                                                   \
  lua["package"]["path"] = lua["package"]["path"].get<string>() + ";" + p

using namespace std;
using json = nlohmann::json;

namespace MADS {

sol::table to_table(sol::state &lua, json const &j) {
  sol::table t = lua.create_table();
  for (auto &el : j.items()) {
    if (el.value().is_array()) {
      sol::table a = lua.create_table();
      int i = 1;
      for (auto &el2 : el.value()) {
        if (el2.is_object()) {
          a[i++] = to_table(lua, el2);
        } else if (el2.is_string()) {
          a[i++] = el2.get<string>();
        } else if (el2.is_number_float()) {
          a[i++] = el2.get<double>();
        } else if (el2.is_number_integer()) {
          a[i++] = el2.get<long int>();
        } else if (el2.is_boolean()) {
          a[i++] = el2.get<bool>();
        } else {
          a[i++] = el2;
        }
      }
      t[el.key()] = a;
    } else if (el.value().is_object()) {
      t[el.key()] = to_table(lua, el.value());
    } else if (el.value().is_string()) {
      t[el.key()] = el.value().get<string>();
    } else if (el.value().is_number()) {
      t[el.key()] = el.value().get<double>();
    } else {
      t[el.key()] = el.value();
    }
  }
  return t;
}

std::filesystem::path exec_path() {
  char raw_path[PATH_MAX];
#if defined(_WIN32)
  GetModuleFileNameA(NULL, raw_path, MAX_PATH);
  return std::string(raw_path);
#elif defined(__linux__)
  realpath(PROC_SELF_EXE, raw_path);
#elif defined(__APPLE__)
  uint32_t rawPathSize = (uint32_t)sizeof(raw_path);
  _NSGetExecutablePath(raw_path, &rawPathSize);
#endif
  return std::filesystem::weakly_canonical(raw_path);
}

std::filesystem::path exec_dir(std::filesystem::path relative = "") {
  std::filesystem::path path = exec_path().parent_path();
  if (relative != "") {
    return std::filesystem::weakly_canonical(path / relative);
  } else {
    return std::filesystem::weakly_canonical(path);
  }
}

filesystem::path try_paths(vector<filesystem::path> paths,
                           filesystem::path file) {
  for (auto &path : paths) {
    if (!path.is_absolute()) {
      path = exec_dir(path);
    }
    if (filesystem::exists(path / file)) {
      return path / file;
    }
  }
  return file;
}

class LuaPlugin {

protected:
  void prepare_paths(json &params) {
    for (auto &path : params["search_paths"]) {
      _default_paths.push_back(filesystem::path(path.get<string>()));
    }

    _script_file = params["script_file"];
    _script_path = filesystem::path(_script_file);
    if (!_script_path.is_absolute()) {
      _script_file = MADS::try_paths(_default_paths, _script_path).string();
    }
  }

  void prepare_lua(string method = "") {
    _lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::string,
                        sol::lib::table, sol::lib::math, sol::lib::string,
                        sol::lib::os, sol::lib::io);
    for (auto &path : _default_paths) {
      if (!path.is_absolute()) {
        path = MADS::exec_dir() / path;
      }
      LUA_ADD_PATH(_lua, path.string() + "/?.lua");
    }
    _lua.script(R"END(
json = require("json")
MADS = {}
function MADS.dump(o)
  if type(o) == 'table' then
     local s = '{'
     for k,v in pairs(o) do
        if type(k) == 'number' then 
          s = s .. k .. ", "
        else 
          s = s .. k .. ": " .. MADS.dump(v) .. ', '
        end
     end
     s = string.sub(s, 1, -3)
     return s .. '}'
  else
    if type(o) == "string" then
      return '"' .. o .. '"'
    else
      return tostring(o)
    end
  end
end
    )END");
    try {
      _lua.script_file(_script_file);
    } catch (sol::error &e) {
      cerr << "Error: " << e.what() << endl;
      exit(EXIT_FAILURE);
    }
    _self = _lua["MADS"];
    if (method != "" && !_self[method].valid()) {
      cerr << "Error: the script " << _script_file
           << " must define a function named MADS:" << method
           << "() that returns"
              " a valid JSON string"
           << endl;
      exit(EXIT_FAILURE);
    }
  }

  sol::state _lua;
  sol::table _self;
  string _script_file;
  filesystem::path _script_path;
  // those are the defauilt paths where the script can be found
  // RELATIVE to the current executable!
  // clang-format off
  vector<filesystem::path> _default_paths = {
      "./lua",      
      "./scripts",           
      "../lua",
      "../scripts", 
      "../../lua",
      "../../scripts", 
      INSTALL_PREFIX "/lua", 
      INSTALL_PREFIX "/scripts"};
  // clang-format on
};

} // namespace MADS