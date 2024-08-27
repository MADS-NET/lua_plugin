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
  std::string path = exec_path().parent_path();
  if (relative != "") {
    return std::filesystem::weakly_canonical(path / relative);
  } else {
    return std::filesystem::weakly_canonical(path);
  }
}


filesystem::path try_paths(vector<filesystem::path> paths, filesystem::path file) {
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


} // namespace MADS