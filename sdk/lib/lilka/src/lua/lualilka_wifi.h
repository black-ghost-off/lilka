#ifndef LUALILKA_WIFI_H
#define LUALILKA_WIFI_H

#include <lua.hpp>
#include <Arduino.h>

namespace lilka {

// int luaopen_lilka_wifi(lua_State* L);
int lualilka_wifi_register(lua_State* L);

}

#endif // LUALILKA_WIFI_H
