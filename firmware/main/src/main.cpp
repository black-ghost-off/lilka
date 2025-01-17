#include <esp_wifi.h>
#include <esp_task_wdt.h>

#include <Arduino_GFX_Library.h>

#include "nes/hw_config.h"

extern "C" {
#include <nofrendo.h>
}

#include <lilka.h>
#include "icons/file.h"
#include "icons/folder.h"
#include "icons/nes.h"
#include "icons/bin.h"
#include "icons/lua.h"
#include "icons/js.h"
#include "icons/demos.h"
#include "icons/sdcard.h"
#include "icons/memory.h"
#include "icons/dev.h"
#include "icons/settings.h"
#include "icons/info.h"

#include "demos/demos.h"

void setup() {
    lilka::begin();
}

void demos_menu() {
    void (*demo_funcs[])() = {
        demo_lines, demo_disc, demo_ball, demo_epilepsy, demo_letris, demo_user_spi, demo_scan_i2c, 0,
    };

    String demos[] = {
        "Лінії", "Шайба", "М'ячик", "Епілепсія", "Летріс", "Тест SPI", "I2C-сканер", "<< Назад",
    };
    int count = sizeof(demos) / sizeof(demos[0]);
    int cursor = 0;
    while (1) {
        cursor = lilka::ui_menu("Оберіть демо:", demos, count, cursor);
        if (!demo_funcs[cursor]) {
            return;
        }
        demo_funcs[cursor]();
    }
}

const menu_icon_t *get_file_icon(const String &filename) {
    if (filename.endsWith(".rom") || filename.endsWith(".nes")) {
        return &nes;
    } else if (filename.endsWith(".bin")) {
        return &bin;
    } else if (filename.endsWith(".lua")) {
        return &lua;
    } else if (filename.endsWith(".js")) {
        return &js;
    } else {
        return &file;
    }
}

const uint16_t get_file_color(const String &filename) {
    if (filename.endsWith(".rom") || filename.endsWith(".nes")) {
        return lilka::display.color565(255, 128, 128);
    } else if (filename.endsWith(".bin")) {
        return lilka::display.color565(128, 255, 128);
    } else if (filename.endsWith(".lua")) {
        return lilka::display.color565(128, 128, 255);
    } else if (filename.endsWith(".js")) {
        return lilka::display.color565(255, 200, 128);
    } else {
        return lilka::display.color565(200, 200, 200);
    }
}

void select_file(String path) {
    if (path.endsWith(".rom") || path.endsWith(".nes")) {
        char *argv[1];
        char fullFilename[256];
        strcpy(fullFilename, path.c_str());
        argv[0] = fullFilename;

        TaskHandle_t idle_0 = xTaskGetIdleTaskHandleForCPU(0);
        esp_task_wdt_delete(idle_0);

        Serial.print("NoFrendo start! Filename: ");
        Serial.println(argv[0]);
        nofrendo_main(1, argv);
        Serial.println("NoFrendo end!\n");
    } else if (path.endsWith(".bin")) {
        int error;
        lilka::LoaderHandle *handle = lilka::loader.createHandle(path);
        error = handle->start();
        if (error) {
            delete handle;
            lilka::ui_alert("Помилка", String("Етап: 1\nКод: ") + error);
            return;
        }
        lilka::display.fillScreen(lilka::display.color565(0, 0, 0));
        lilka::display.setTextColor(lilka::display.color565(255, 255, 255), lilka::display.color565(0, 0, 0));
        lilka::display.setFont(u8g2_font_10x20_t_cyrillic);
        lilka::display.setTextBound(16, 0, LILKA_DISPLAY_WIDTH - 16, LILKA_DISPLAY_HEIGHT);
        while ((error = handle->process()) != 0) {
            float progress = (float)handle->getBytesWritten() / handle->getBytesTotal();
            lilka::display.setCursor(16, LILKA_DISPLAY_HEIGHT / 2 - 10);
            lilka::display.printf("Завантаження (%d%%)\n", (int)(progress * 100));
            lilka::display.println(path);
            String buf = String(handle->getBytesWritten()) + " / " + handle->getBytesTotal();
            int16_t x, y;
            uint16_t w, h;
            lilka::display.getTextBounds(buf, lilka::display.getCursorX(), lilka::display.getCursorY(), &x, &y, &w, &h);
            lilka::display.fillRect(x, y, w, h, lilka::display.color565(0, 0, 0));
            lilka::display.println(buf);
            lilka::display.fillRect(
                16, LILKA_DISPLAY_HEIGHT / 2 + 40, LILKA_DISPLAY_WIDTH - 32, 5, lilka::display.color565(64, 64, 64)
            );
            lilka::display.fillRect(
                16, LILKA_DISPLAY_HEIGHT / 2 + 40, (LILKA_DISPLAY_WIDTH - 32) * progress, 5,
                lilka::display.color565(255, 128, 0)
            );
        }
        if (error) {
            delete handle;
            lilka::ui_alert("Помилка", String("Етап: 2\nКод: ") + error);
            return;
        }
        error = handle->finishAndReboot();
        if (error) {
            delete handle;
            lilka::ui_alert("Помилка", String("Етап: 3\nКод: ") + error);
            return;
        }
    } else if (path.endsWith(".lua")) {
        int retCode = lilka::lua_runfile(path);
        if (retCode) {
            lilka::ui_alert("Lua", String("Увага!\nКод завершення: ") + retCode);
        }
    } else if (path.endsWith(".js")) {
        int retCode = lilka::mjs_run(path);
        if (retCode) {
            lilka::ui_alert("Lua", String("Увага!\nКод завершення: ") + retCode);
        }
    } else {
        // Get file size
        FILE *file = fopen(path.c_str(), "r");
        if (!file) {
            lilka::ui_alert("Помилка", "Не вдалося відкрити файл");
            return;
        }
        fseek(file, 0, SEEK_END);
        long size = ftell(file);
        fclose(file);
        lilka::ui_alert(path, String("Розмір:\n") + size + " байт");
    }
}

void sd_browser_menu(String path) {
    if (!lilka::sdcard.available()) {
        lilka::ui_alert("Помилка", "SD-карта не знайдена");
        return;
    }

    lilka::Entry entries
        [32]; // TODO - allocate dynamically (increasing to 64 causes task stack overflow which is limited by ARDUINO_LOOP_STACK_SIZE)
    int numEntries = lilka::sdcard.listDir(path, entries);

    if (numEntries == -1) {
        lilka::ui_alert("Помилка", "Не вдалося прочитати директорію");
        return;
    }

    String filenames[32];
    const menu_icon_t *icons[32];
    uint16_t colors[32];
    for (int i = 0; i < numEntries; i++) {
        filenames[i] = entries[i].name;
        icons[i] = entries[i].type == lilka::EntryType::ENT_DIRECTORY ? &folder : get_file_icon(filenames[i]);
        colors[i] = entries[i].type == lilka::EntryType::ENT_DIRECTORY ? lilka::display.color565(255, 255, 200)
                                                                       : get_file_color(filenames[i]);
    }
    filenames[numEntries++] = "<< Назад";
    icons[numEntries - 1] = 0;
    colors[numEntries - 1] = 0;

    int cursor = 0;
    while (1) {
        cursor = lilka::ui_menu(String("SD: ") + path, filenames, numEntries, cursor, icons, colors);
        if (cursor == numEntries - 1) {
            return;
        }
        if (entries[cursor].type == lilka::EntryType::ENT_DIRECTORY) {
            sd_browser_menu(path + entries[cursor].name + "/");
        } else {
            select_file(lilka::sdcard.abspath(path + entries[cursor].name));
        }
    }
}

void spiffs_browser_menu() {
    if (!lilka::filesystem.available()) {
        lilka::ui_alert("Помилка", "SPIFFS не підтримується");
        return;
    }

    String filenames
        [32]; // TODO - allocate dynamically (increasing to 64 causes task stack overflow which is limited by ARDUINO_LOOP_STACK_SIZE)
    int numEntries = lilka::filesystem.readdir(filenames);

    if (numEntries == -1) {
        lilka::ui_alert("Помилка", "Не вдалося прочитати корінь SPIFFS");
        return;
    }

    const menu_icon_t *icons[32];
    uint16_t colors[32];
    for (int i = 0; i < numEntries; i++) {
        icons[i] = get_file_icon(filenames[i]);
        colors[i] = get_file_color(filenames[i]);
    }
    filenames[numEntries++] = "<< Назад";
    icons[numEntries - 1] = 0;
    colors[numEntries - 1] = 0;

    int cursor = 0;
    while (1) {
        cursor = lilka::ui_menu(String("SPIFFS"), filenames, numEntries, cursor, icons, colors);
        if (cursor == numEntries - 1) {
            return;
        }
        select_file(lilka::filesystem.abspath(filenames[cursor]));
    }
}

void live_lua() {
    // Drain the serial buffer
    Serial.setTimeout(10);
    while (Serial.available()) {
        Serial.read();
    }

    while (1) {
        if (lilka::controller.getState().a.justPressed) {
            return;
        }
        lilka::display.setFont(FONT_10x20);
        lilka::display.setCursor(8, 48);
        lilka::display.fillScreen(lilka::display.color565(0, 0, 0));
        lilka::display.setTextBound(8, 0, LILKA_DISPLAY_WIDTH - 16, LILKA_DISPLAY_HEIGHT);
        lilka::display.print("Очікування коду\nз UART...\n\n");
        lilka::display.print("Натисніть [A]\n");
        lilka::display.print("для виходу.");
        lilka::display.setCursor(8, 180);

        // Read serial data
        Serial.setTimeout(100);
        String code;
        while (!Serial.available()) {
            if (lilka::controller.getState().a.justPressed) {
                return;
            }
        }
        while (1) {
            // Read lines from serial.
            // It nothing is read for 0.5 seconds, stop reading.
            if (lilka::controller.getState().a.justPressed) {
                return;
            }
            String line = Serial.readString();
            if (line.length() == 0) {
                lilka::display.print("!");
                break;
            }
            lilka::display.print(".");
            code += line;
        }

        // Serial.println("Code received:");
        // Serial.println(code);

        // Those darn line ends...
        // If code contains \r and \n - replace them with \n
        // If code contains only \r - replace it with \n
        // If code contains only \n - leave it as is
        if (code.indexOf('\r') != -1) {
            if (code.indexOf('\n') != -1) {
                lilka::serial_log("Line ends: CR and LF");
                code.replace("\r", "");
            } else {
                lilka::serial_log("Line ends: CR only");
                code.replace("\r", "\n");
            }
        } else {
            lilka::serial_log("Line ends: LF only");
        }

        // TODO: This is a temporary fix: https://github.com/espressif/arduino-esp32/issues/9221
        lilka::sdcard.available();

        // Run the code
        int retCode = lilka::lua_runsource(code);
        if (retCode) {
            lilka::ui_alert("Lua", String("Увага!\nКод завершення: ") + retCode);
        }
    }
}

void lua_repl() {
    // Drain the serial buffer
    Serial.setTimeout(10);
    while (Serial.available()) {
        Serial.read();
    }

    lilka::display.setFont(FONT_10x20);
    lilka::display.setCursor(8, 48);
    lilka::display.fillScreen(lilka::display.color565(0, 0, 0));
    lilka::display.setTextBound(8, 0, LILKA_DISPLAY_WIDTH - 16, LILKA_DISPLAY_HEIGHT);
    lilka::display.print("Lua REPL\n\n");
    lilka::display.print("Під'єднайтесь до\nЛілки через серійний\nтермінал та починайте\nвводити команди!");

    int retCode = lilka::lua_repl_start();
    if (retCode) {
        lilka::ui_alert("Lua", String("Увага!\nКод завершення: ") + retCode);
        return;
    }

    // TODO: This is a temporary fix: https://github.com/espressif/arduino-esp32/issues/9221
    lilka::sdcard.available();

    bool quit = false;
    while (!quit) {
        String input;
        bool eol = false;
        while (!eol) {
            if (lilka::controller.getState().a.justPressed) {
                quit = true;
                break;
            }
            if (Serial.available()) {
                char c = Serial.read();
                // If backspace
                if (c == 8) {
                    if (input.length() > 0) {
                        input.remove(input.length() - 1);
                        Serial.write(c);
                        Serial.write(' ');
                        Serial.write(c);
                    }
                } else {
                    input += c;
                    Serial.write(c);
                    if (c == 13) {
                        Serial.write(10);
                    }
                }
            }
            if (input.endsWith("\n") || input.endsWith("\r")) {
                eol = true;
            }
        }
        int retCode = lilka::lua_repl_input(input);
        if (retCode) {
            lilka::serial_log("lua: return code: %d", retCode);
        }
    }

    retCode = lilka::lua_repl_stop();
    if (retCode) {
        lilka::ui_alert("Lua", String("Увага!\nКод завершення: ") + retCode);
    }
}

void system_utils_menu() {
    String menu[] = {
        "Перезавантаження", "Версія ESP-IDF", "Інфо про пристрій", "Таблиця розділів", "<< Назад",
    };
    int cursor = 0;
    int count = sizeof(menu) / sizeof(menu[0]);
    while (1) {
        cursor = lilka::ui_menu("Системні утиліти", menu, count, cursor);
        if (cursor == 0) {
            esp_restart();
        } else if (cursor == 1) {
            lilka::ui_alert("Версія ESP-IDF", "Версія: " + String(ESP.getSdkVersion()));
        } else if (cursor == 2) {
            char buf[256];
            sprintf(
                buf,
                "Модель: %s\n"
                "Ревізія: %d\n"
                "Версія SDK: %s\n"
                "Версія ESP-IDF: %s\n"
                "Частота: %d МГц\n"
                "Кількість ядер: %d\n",
                ESP.getChipModel(), ESP.getChipRevision(), ESP.getSdkVersion(), esp_get_idf_version(),
                ESP.getCpuFreqMHz(), ESP.getChipCores()
            );
            lilka::ui_alert("Інфо про пристрій", buf);
        } else if (cursor == 3) {
            String labels[16];
            int labelCount = lilka::sys.get_partition_labels(labels);
            labels[labelCount++] = "<< Назад";
            int partitionCursor = 0;
            while (1) {
                partitionCursor = lilka::ui_menu("Таблиця розділів", labels, labelCount, partitionCursor);
                if (partitionCursor == labelCount - 1) {
                    break;
                }
                lilka::ui_alert(
                    labels[partitionCursor],
                    String("Адреса: 0x") +
                        String(lilka::sys.get_partition_address(labels[partitionCursor].c_str()), HEX) + "\n" +
                        "Розмір: 0x" + String(lilka::sys.get_partition_size(labels[partitionCursor].c_str()), HEX)
                );
            }
        } else if (cursor == count - 1) {
            return;
        }
    }
}

void dev_menu() {
    String menu[] = {
        "Live Lua",
        "Lua REPL",
        "<< Назад",
    };
    int cursor = 0;
    int count = sizeof(menu) / sizeof(menu[0]);
    while (1) {
        cursor = lilka::ui_menu("Розробка", menu, count, cursor);
        if (cursor == 0) {
            live_lua();
        } else if (cursor == 1) {
            lua_repl();
        } else if (cursor == count - 1) {
            return;
        }
    }
}

void loop() {
    String menu[] = {
        "Демо", "Браузер SD-карти", "Браузер SPIFFS", "Розробка", "Системні утиліти", "Про систему",
    };
    const menu_icon_t *icons[] = {
        &demos, &sdcard, &memory, &dev, &settings, &info,
    };
    const uint16_t colors[] = {
        lilka::display.color565(255, 200, 200), lilka::display.color565(255, 255, 200),
        lilka::display.color565(200, 255, 200), lilka::display.color565(255, 224, 128),
        lilka::display.color565(255, 200, 224), lilka::display.color565(200, 224, 255),
    };
    int cursor = 0;
    int count = sizeof(menu) / sizeof(menu[0]);
    while (1) {
        cursor = lilka::ui_menu("Головне меню", menu, count, cursor, icons, colors);
        if (cursor == 0) {
            demos_menu();
        } else if (cursor == 1) {
            sd_browser_menu("/");
        } else if (cursor == 2) {
            spiffs_browser_menu();
        } else if (cursor == 3) {
            dev_menu();
        } else if (cursor == 4) {
            system_utils_menu();
        } else if (cursor == 5) {
            lilka::ui_alert("Лілка Main OS", "by Андерсон\n& friends");
        }
    }
}
