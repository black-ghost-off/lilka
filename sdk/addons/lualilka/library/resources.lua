---@meta

---@class resources
resources = {}

---Завантажує BMP-зображення і повертає таблицю з ідентифікатором зображення (а також з його розмірами), яку можна використовувати для малювання зображення на екрані.
---Ця таблиця містить наступні поля:
---
---  * ``width``: ширина зображення
---  * ``height``: висота зображення
---  * ``pointer``: внутрішній ідентифікатор зображення
---
---@param filename string шлях до файлу зображення .BMP (відносно місця знаходження скрипта, що виконується)
---@param transparent_color? integer колір, який буде використаний для прозорості (5-6-5). Якщо цей параметр не вказаний, зображення буде виводитись без прозорості
---@return table
---@usage
--- local face = resources.load_bitmap("face.bmp", display.color565(0, 0, 0))
--- print(face.width, face.height) -- Виведе розміри зображення
--- display.draw_bitmap(face, 50, 80) -- Виведе зображення на екран у позицію (50, 80)
function resources.load_bitmap(filename, transparent_color) end

return resources
