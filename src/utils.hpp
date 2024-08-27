#include <lua.hpp>
#include <lualib.h>
#include <lauxlib.h>
#include <sol/sol.hpp>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

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