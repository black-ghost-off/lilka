#include "lualilka_display.h"

namespace lilka {

Arduino_GFX* getDrawable(lua_State* L) {
    // Check if display is buffered.
    // If buffered, return canvas pointer from registry.
    // If not buffered, return pointer to actual display.
    lua_getfield(L, LUA_REGISTRYINDEX, "isBuffered");
    bool isBuffered = lua_toboolean(L, -1);
    lua_pop(L, 1);

    if (isBuffered) {
        // Return canvas pointer from registry
        lua_getfield(L, LUA_REGISTRYINDEX, "canvas");
        Canvas* canvas = (Canvas*)lua_touserdata(L, -1);
        lua_pop(L, 1);
        return canvas;
    } else {
        return &display;
    }
}

int lualilka_display_setBuffered(lua_State* L) {
    bool buffered = lua_toboolean(L, 1);

    // Store isBuffered in registry

    lua_pushboolean(L, buffered);
    lua_setfield(L, LUA_REGISTRYINDEX, "isBuffered");

    return 0;
}

int lualilka_display_color565(lua_State* L) {
    int r = luaL_checknumber(L, 1);
    int g = luaL_checknumber(L, 2);
    int b = luaL_checknumber(L, 3);
    uint16_t color = display.color565(r, g, b);
    lua_pushinteger(L, color);
    return 1;
}

int lualilka_display_setCursor(lua_State* L) {
    int x = luaL_checknumber(L, 1);
    int y = luaL_checknumber(L, 2);
    getDrawable(L)->setCursor(x, y);
    return 0;
}

const uint8_t* fonts[] = {
    u8g2_font_4x6_t_cyrillic,  u8g2_font_5x7_t_cyrillic,  u8g2_font_5x8_t_cyrillic,
    u8g2_font_6x12_t_cyrillic, u8g2_font_6x13_t_cyrillic, u8g2_font_7x13_t_cyrillic,
    u8g2_font_8x13_t_cyrillic, u8g2_font_9x15_t_cyrillic, u8g2_font_10x20_t_cyrillic,
};

const char fontNames[][12] = {
    "4x6", "5x7", "5x8", "6x12", "6x13", "7x13", "8x13", "9x15", "10x20",
};

int lualilka_display_setFont(lua_State* L) {
    const char* fontName = luaL_checkstring(L, 1);
    for (int i = 0; i < 9; i++) {
        if (strcmp(fontName, fontNames[i]) == 0) {
            getDrawable(L)->setFont(fonts[i]);
            return 0;
        }
    }
    return luaL_error(L, "Невідомий шрифт - %s", fontName);
}

int lualilka_display_setTextSize(lua_State* L) {
    int size = luaL_checknumber(L, 1);
    getDrawable(L)->setTextSize(size);
    return 0;
}

int lualilka_display_setTextColor(lua_State* L) {
    uint16_t fgColor = luaL_checkinteger(L, 1);
    // Check if bgColor is provided
    if (lua_gettop(L) > 1) {
        uint16_t bgColor = luaL_checkinteger(L, 2);
        // Set both fgColor & bgColor
        getDrawable(L)->setTextColor(fgColor, bgColor);
        return 0;
    }
    // Set only fgColor
    getDrawable(L)->setTextColor(fgColor);
    return 0;
}

int lualilka_display_print(lua_State* L) {
    int n = lua_gettop(L);
    for (int i = 1; i <= n; i++) {
        if (lua_isstring(L, i)) {
            getDrawable(L)->print(lua_tostring(L, i));
        } else if (lua_isnumber(L, i)) {
            getDrawable(L)->print(lua_tonumber(L, i));
        } else if (lua_islightuserdata(L, i)) {
            getDrawable(L)->printf("%p", lua_topointer(L, i));
        } else {
            getDrawable(L)->print(lua_typename(L, lua_type(L, i)));
        }
    }
    return 0;
}

int lualilka_display_fillScreen(lua_State* L) {
    uint16_t color = luaL_checkinteger(L, 1);
    getDrawable(L)->fillScreen(color);
    return 0;
}

int lualilka_display_drawPixel(lua_State* L) {
    int x = luaL_checknumber(L, 1);
    int y = luaL_checknumber(L, 2);
    uint16_t color = luaL_checkinteger(L, 3);
    getDrawable(L)->drawPixel(x, y, color);
    return 0;
}

int lualilka_display_drawLine(lua_State* L) {
    int x0 = luaL_checknumber(L, 1);
    int y0 = luaL_checknumber(L, 2);
    int x1 = luaL_checknumber(L, 3);
    int y1 = luaL_checknumber(L, 4);
    uint16_t color = luaL_checkinteger(L, 5);
    getDrawable(L)->drawLine(x0, y0, x1, y1, color);
    return 0;
}

int lualilka_display_drawRect(lua_State* L) {
    int x = luaL_checknumber(L, 1);
    int y = luaL_checknumber(L, 2);
    int w = luaL_checknumber(L, 3);
    int h = luaL_checknumber(L, 4);
    uint16_t color = luaL_checkinteger(L, 5);
    getDrawable(L)->drawRect(x, y, w, h, color);
    return 0;
}

int lualilka_display_fillRect(lua_State* L) {
    int x = luaL_checknumber(L, 1);
    int y = luaL_checknumber(L, 2);
    int w = luaL_checknumber(L, 3);
    int h = luaL_checknumber(L, 4);
    uint16_t color = luaL_checkinteger(L, 5);
    getDrawable(L)->fillRect(x, y, w, h, color);
    return 0;
}

int lualilka_display_drawCircle(lua_State* L) {
    int x = luaL_checknumber(L, 1);
    int y = luaL_checknumber(L, 2);
    int r = luaL_checknumber(L, 3);
    uint16_t color = luaL_checkinteger(L, 4);
    getDrawable(L)->drawCircle(x, y, r, color);
    return 0;
}

int lualilka_display_fillCircle(lua_State* L) {
    int x = luaL_checknumber(L, 1);
    int y = luaL_checknumber(L, 2);
    int r = luaL_checknumber(L, 3);
    uint16_t color = luaL_checkinteger(L, 4);
    getDrawable(L)->fillCircle(x, y, r, color);
    return 0;
}

int lualilka_display_drawTriangle(lua_State* L) {
    int x0 = luaL_checknumber(L, 1);
    int y0 = luaL_checknumber(L, 2);
    int x1 = luaL_checknumber(L, 3);
    int y1 = luaL_checknumber(L, 4);
    int x2 = luaL_checknumber(L, 5);
    int y2 = luaL_checknumber(L, 6);
    uint16_t color = luaL_checkinteger(L, 7);
    getDrawable(L)->drawTriangle(x0, y0, x1, y1, x2, y2, color);
    return 0;
}

int lualilka_display_fillTriangle(lua_State* L) {
    int x0 = luaL_checknumber(L, 1);
    int y0 = luaL_checknumber(L, 2);
    int x1 = luaL_checknumber(L, 3);
    int y1 = luaL_checknumber(L, 4);
    int x2 = luaL_checknumber(L, 5);
    int y2 = luaL_checknumber(L, 6);
    uint16_t color = luaL_checkinteger(L, 7);
    getDrawable(L)->fillTriangle(x0, y0, x1, y1, x2, y2, color);
    return 0;
}

int lualila_display_drawEllipse(lua_State* L) {
    int x = luaL_checknumber(L, 1);
    int y = luaL_checknumber(L, 2);
    int rx = luaL_checknumber(L, 3);
    int ry = luaL_checknumber(L, 4);
    uint16_t color = luaL_checkinteger(L, 5);
    getDrawable(L)->drawEllipse(x, y, rx, ry, color);
    return 0;
}

int lualila_display_fillEllipse(lua_State* L) {
    int x = luaL_checknumber(L, 1);
    int y = luaL_checknumber(L, 2);
    int rx = luaL_checknumber(L, 3);
    int ry = luaL_checknumber(L, 4);
    uint16_t color = luaL_checkinteger(L, 5);
    getDrawable(L)->fillEllipse(x, y, rx, ry, color);
    return 0;
}

int lualilka_display_drawArc(lua_State* L) {
    int x = luaL_checknumber(L, 1);
    int y = luaL_checknumber(L, 2);
    int r1 = luaL_checknumber(L, 3);
    int r2 = luaL_checknumber(L, 4);
    int startAngle = luaL_checknumber(L, 5);
    int endAngle = luaL_checknumber(L, 6);
    uint16_t color = luaL_checkinteger(L, 7);
    getDrawable(L)->drawArc(x, y, r1, r2, startAngle, endAngle, color);
    return 0;
}

int lualilka_display_fillArc(lua_State* L) {
    int x = luaL_checknumber(L, 1);
    int y = luaL_checknumber(L, 2);
    int r1 = luaL_checknumber(L, 3);
    int r2 = luaL_checknumber(L, 4);
    int startAngle = luaL_checknumber(L, 5);
    int endAngle = luaL_checknumber(L, 6);
    uint16_t color = luaL_checkinteger(L, 7);
    getDrawable(L)->fillArc(x, y, r1, r2, startAngle, endAngle, color);
    return 0;
}

int lualilka_display_drawImage(lua_State* L) {
    // Args are image table, X & Y
    // First argument is table that contains image width, height and pointer. We only need the pointer.
    lua_getfield(L, 1, "pointer");
    // Check if value is a valid pointer

    // TODO: Check if crap ain't broken since the user may pass anything here and this often causes core panic
    if (!lua_islightuserdata(L, -1)) {
        return luaL_error(L, "Некоректне зображення");
    }

    Image* image = (Image*)lua_touserdata(L, -1);
    lua_pop(L, 1);

    int16_t x = luaL_checknumber(L, 2);
    int16_t y = luaL_checknumber(L, 3);

    if (image->transparentColor >= 0) {
        getDrawable(L)->draw16bitRGBBitmapWithTranColor(
            x, y, image->pixels, image->transparentColor, image->width, image->height
        );
    } else {
        getDrawable(L)->draw16bitRGBBitmap(x, y, image->pixels, image->width, image->height);
    }

    return 0;
};

int lualilka_display_render(lua_State* L) {
    // Check if display is buffered
    lua_getfield(L, LUA_REGISTRYINDEX, "isBuffered");
    bool isBuffered = lua_toboolean(L, -1);
    lua_pop(L, 1);
    if (!isBuffered) {
        // Throw error
        return luaL_error(L, "Буферизація вимкнена, використовуйте display.render() тільки з буферизацією");
    }
    lua_getfield(L, LUA_REGISTRYINDEX, "canvas");
    Canvas* canvas = (Canvas*)lua_touserdata(L, -1);
    lua_pop(L, 1);
    display.renderCanvas(*canvas);
    return 0;
}

static const luaL_Reg lualilka_display[] = {
    {"set_buffered", lualilka_display_setBuffered},
    {"color565", lualilka_display_color565},
    {"set_cursor", lualilka_display_setCursor},
    {"set_font", lualilka_display_setFont},
    {"set_text_size", lualilka_display_setTextSize},
    {"set_text_color", lualilka_display_setTextColor},
    {"print", lualilka_display_print},
    {"fill_screen", lualilka_display_fillScreen},
    {"draw_pixel", lualilka_display_drawPixel},
    {"draw_line", lualilka_display_drawLine},
    {"draw_rect", lualilka_display_drawRect},
    {"fill_rect", lualilka_display_fillRect},
    {"draw_circle", lualilka_display_drawCircle},
    {"fill_circle", lualilka_display_fillCircle},
    {"draw_triangle", lualilka_display_drawTriangle},
    {"fill_triangle", lualilka_display_fillTriangle},
    {"draw_ellipse", lualila_display_drawEllipse},
    {"fill_ellipse", lualila_display_fillEllipse},
    {"draw_arc", lualilka_display_drawArc},
    {"fill_arc", lualilka_display_fillArc},
    {"draw_image", lualilka_display_drawImage},
    {"render", lualilka_display_render},
    {NULL, NULL},
};

// int luaopen_lilka_display(lua_State* L) {
//     // Set isBuffered to true by default in registry
//     lua_pushboolean(L, true);
//     lua_setfield(L, LUA_REGISTRYINDEX, "isBuffered");
//
//     luaL_newlib(L, lualilka_display);
//     // Add display width & height as library properties
//     lua_pushinteger(L, LILKA_DISPLAY_WIDTH);
//     lua_setfield(L, -2, "width");
//     lua_pushinteger(L, LILKA_DISPLAY_HEIGHT);
//     lua_setfield(L, -2, "height");
//     return 1;
// }

int lualilka_display_register(lua_State* L) {
    // Create global "display" table that contains all display functions
    luaL_newlib(L, lualilka_display);
    // Add display width & height as library properties
    lua_pushinteger(L, LILKA_DISPLAY_WIDTH);
    lua_setfield(L, -2, "width");
    lua_pushinteger(L, LILKA_DISPLAY_HEIGHT);
    lua_setfield(L, -2, "height");
    // Set isBuffered to true by default in registry
    lua_pushboolean(L, true);
    lua_setfield(L, LUA_REGISTRYINDEX, "isBuffered");
    lua_setglobal(L, "display");
    return 0;
}

} // namespace lilka
