local SIK2D = require("SIK2D")

SIK2D:init(800, 600, true)

local house = SIK2D:newRect("house", 100, 100, 200, 150, false)
local roofLeft = SIK2D:newLine("roofLeft", 80, 100, 180, 50)
local roofRight = SIK2D:newLine("roofRight", 180, 50, 280, 100)
local window = SIK2D:newRect("window", 150, 150, 40, 40, true)
local door = SIK2D:newRect("door", 200, 180, 40, 70, false)
local sun = SIK2D:newCircle("sun", 500, 80, 40)
local title = SIK2D:newText("title", 300, 30, "MY HOUSE")

house.char = '#'
window.char = 'O'
door.char = '='
sun.char = '@'
roofLeft.char = '/'
roofRight.char = '\\'

SIK2D:draw()

local myWindow = SIK2D:find("window")
if myWindow then
    myWindow:move(30, 0)
end

local grass = SIK2D:newLine("grass", 50, 280, 750, 280)
grass.char = '_'

print("\n=== After moving window ===\n")
SIK2D:draw()

SIK2D:save("mydom.lua")

print("\n=== Scene objects ===")
for _, obj in ipairs(SIK2D.objects) do
    print(obj.tag .. ": " .. obj.name .. " (" .. obj.x .. "," .. obj.y .. ")")
end