#ifndef LILKA_RESOURCES_H
#define LILKA_RESOURCES_H

#include <Arduino.h>
#include "display.h"

namespace lilka {

/// Клас для роботи з ресурсами - зображенням, файлами даних тощо.
class Resources {
public:
    /// Завантажити зображення в форматі BMP з файлу.
    ///
    /// \param filename Шлях до файлу.
    /// \param transparentColor 16-бітний колір (5-6-5), який буде прозорим. За замовчуванням -1 (прозорість відсутня).
    /// \return Вказівник на зображення.
    ///
    /// \warning Пам'ять для зображення виділяється динамічно. Після використання зображення, його потрібно видалити за допомогою `delete`.
    ///
    /// Приклад:
    ///
    /// \code
    /// lilka::Image *image = lilka::resources.loadImage("image.bmp", lilka::display.color565(255, 255, 0)); //
    /// Жовтий колір буде прозорим if (!image) {
    ///     Serial.println("Failed to load image");
    ///     return;
    /// }
    /// // Відобразити зображення на екрані
    /// lilka::display.draw16bitRGBBitmapWithTranColor(50, 100, image->pixels, image->transparentColor, image->width, image->height);
    /// // Звільнити пам'ять
    /// delete image;
    /// \endcode
    Image *loadImage(String filename, int32_t transparentColor = -1);
    /// Прочитати вміст файлу.
    ///
    /// TODO: Update sdcard/filesystem stuff
    ///
    /// \param filename Шлях до файлу.
    /// \return 0, якщо читання успішне; -1, якщо файл не знайдено
    /// \warning Не використовуйте цей метод для читання великих файлів, оскільки весь вміст файлу зберігається в
    /// пам'яті. Для великих файлів використовуйте методи `sdcard` та `filesystem`.
    int readFile(String filename, String &fileContent);
    /// Записати вміст файлу.
    /// \param filename Шлях до файлу.
    /// \param fileContent Вміст файлу.
    /// \return 0, якщо запис успішний; -1, якщо запис не вдався
    int writeFile(String filename, String fileContent);
};

/// Екземпляр класу `Resources`, який можна використовувати для завантаження ресурсів.
/// Вам не потрібно інстанціювати `Resources` вручну.
extern Resources resources;

} // namespace lilka

#endif // LILKA_RESOURCES_H
