---@meta

---@class util
util = {}

function util.time() end

---Затримує виконання скрипта на вказану кількість секунд.
---@usage
---@param sec number кількість секунд, на яку потрібно затримати виконання програми
--- display.set_cursor(0, 32)
--- display.print("Зачекайте півсекунди...")
--- util.sleep(0.5) -- Затримує виконання програми на півсекунди.
--- display.print("Готово!")
function util.sleep(sec) end

---Завершує виконання програми.
---@return nil
---@usage
--- function lilka._update(delta)
---     state = controller.get_state()
---     if state.a.pressed then
---         util.exit() -- Завершує виконання програми.
---     end
---     print('Staying alive!')
--- end
function util.exit() end

return util
