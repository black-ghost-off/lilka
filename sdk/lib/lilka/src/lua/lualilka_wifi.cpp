#include <csetjmp>

#include "lualilka_wifi.h"
#include "WiFi.h"

namespace lilka {

extern jmp_buf stopjmp;

int lualilka_wifi_connect(lua_State* L){
    int n = lua_gettop(L);

    if (n != 2) {
        return luaL_error(L, "Очікується 2 аргументи, отримано %d", n);
    }

    const char* ssid = lua_tostring(L, 1);
    const char* password = lua_tostring(L, 2);
    WiFi.begin(ssid, password);

    return 0;
}

int lualilka_wifi_status(lua_State* L){
    lua_pushinteger(L, WiFi.status());
    return 1;
}

int lualilka_wifi_disconnect(lua_State* L){
    WiFi.disconnect();
    return 0;
}

static const luaL_Reg lualilka_wifi[] = {
    {"wifi_connect", lualilka_wifi_connect},
    {"wifi_status", lualilka_wifi_status},
    {"wifi_disconnect", lualilka_wifi_disconnect},
    {NULL, NULL},
};

// int luaopen_lilka_wifi(lua_State* L) {
//     luaL_newlib(L, lualilka_wifi);
//     return 1;
// }

int lualilka_wifi_register(lua_State* L) {
    // Create global "wifi" table that contains all wifi functions
    luaL_newlib(L, lualilka_wifi);
    lua_setglobal(L, "wifi");
    return 0;
}

} // namespace lilka
