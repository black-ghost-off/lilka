print("Printing stuff to console, yay!")

local face = resources.load_image("face.bmp", display.color565(0, 0, 0))

print('Face size: ' .. face.width .. 'x' .. face.height)

for i = 10, 1, -1 do
    display.fill_rect(0, 0, display.width, display.height, math.random(0xFFFF))
    display.set_cursor(64, 64)
    display.print("Start in " .. i .. "...")

    local x = math.random(240 - 64)
    local y = math.random(280 - 64)
    display.draw_image(face, x, y)

    display.render()

    util.sleep(0.25)
end

-- Now, we want to draw random lines & faces really fast directly to display, without any buffering!
display.set_buffered(false)

local key = controller.get_state()
while not key.a.just_pressed do
    local x1 = math.random(240)
    local y1 = math.random(280)
    local x2 = math.random(240)
    local y2 = math.random(280)
    local color = math.random(0xFFFF)
    display.draw_line(x1, y1, x2, y2, color)

    local x = math.random(240 - 64)
    local y = math.random(280 - 64)
    display.draw_image(face, x, y)

    key = controller.get_state()
end
