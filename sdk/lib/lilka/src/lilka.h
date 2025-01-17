#ifndef LILKA_H
#define LILKA_H

#include "lilka/serial.h"
#include "lilka/spi.h"
#include "lilka/controller.h"
#include "lilka/display.h"
#include "lilka/filesystem.h"
#include "lilka/sdcard.h"
#include "lilka/battery.h"
#include "lilka/buzzer.h"
#include "lilka/ui.h"
#include "lilka/loader.h"
#include "lilka/sys.h"
#include "lilka/resources.h"
#include "lilka/fmath.h"
#include "lua/luarunner.h"
#include "mjs/mjsrunner.h"

namespace lilka {
/// Ініціалізація Лілки
///
/// Ініціалізує всі підсистеми Лілки - дисплей, кнопки, файлову систему, SD-карту, батарею, п'єзо-динамік і т.д.
///
/// Рекомендується викликати цю функцію один раз на початку програми в вашій функції ``setup()``.
void begin();
}

#endif // LILKA_H
