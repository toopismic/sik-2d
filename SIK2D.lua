local SIK2D = {
    objects = {},
    width = 60,
    height = 20,
    canvas = {},
    mouse = {x = 1, y = 1, button = false},
    keys = {}
}

function SIK2D:init(w, h)
    self.width = w or 60
    self.height = h or 20
    self:clear()
    return self
end

function SIK2D:clear()
    self.objects = {}
    for y = 1, self.height do
        self.canvas[y] = {}
        for x = 1, self.width do
            self.canvas[y][x] = ' '
        end
    end
end

function SIK2D:addRect(x, y, w, h, char)
    table.insert(self.objects, {
        type = "rect",
        x = x, y = y, w = w, h = h,
        char = char or '#'
    })
    return self.objects[#self.objects]
end

function SIK2D:addCircle(x, y, r, char)
    table.insert(self.objects, {
        type = "circle",
        x = x, y = y, r = r,
        char = char or 'O'
    })
    return self.objects[#self.objects]
end

function SIK2D:addText(x, y, txt)
    table.insert(self.objects, {
        type = "text",
        x = x, y = y,
        text = txt
    })
    return self.objects[#self.objects]
end

function SIK2D:addLine(x1, y1, x2, y2, char)
    table.insert(self.objects, {
        type = "line",
        x1 = x1, y1 = y1,
        x2 = x2, y2 = y2,
        char = char or '*'
    })
    return self.objects[#self.objects]
end

function SIK2D:draw()
    os.execute("cls")
    
    for y = 1, self.height do
        for x = 1, self.width do
            self.canvas[y][x] = ' '
        end
    end
    
    for _, obj in ipairs(self.objects) do
        if obj.type == "rect" then
            for i = obj.x, obj.x + obj.w - 1 do
                for j = obj.y, obj.y + obj.h - 1 do
                    if i >= 1 and i <= self.width and j >= 1 and j <= self.height then
                        self.canvas[j][i] = obj.char
                    end
                end
            end
        elseif obj.type == "circle" then
            for dy = -obj.r, obj.r do
                for dx = -obj.r, obj.r do
                    if dx * dx + dy * dy <= obj.r * obj.r then
                        local x = obj.x + dx
                        local y = obj.y + dy
                        if x >= 1 and x <= self.width and y >= 1 and y <= self.height then
                            self.canvas[y][x] = obj.char
                        end
                    end
                end
            end
        elseif obj.type == "text" then
            for i = 1, #obj.text do
                local x = obj.x + i - 1
                if x >= 1 and x <= self.width and obj.y >= 1 and obj.y <= self.height then
                    self.canvas[obj.y][x] = string.sub(obj.text, i, i)
                end
            end
        elseif obj.type == "line" then
            local dx = math.abs(obj.x2 - obj.x1)
            local dy = math.abs(obj.y2 - obj.y1)
            local sx = obj.x1 < obj.x2 and 1 or -1
            local sy = obj.y1 < obj.y2 and 1 or -1
            local err = dx - dy
            local x, y = obj.x1, obj.y1
            
            while true do
                if x >= 1 and x <= self.width and y >= 1 and y <= self.height then
                    self.canvas[y][x] = obj.char
                end
                if x == obj.x2 and y == obj.y2 then break end
                local e2 = 2 * err
                if e2 > -dy then
                    err = err - dy
                    x = x + sx
                end
                if e2 < dx then
                    err = err + dx
                    y = y + sy
                end
            end
        end
    end
    
    print("┌" .. string.rep("─", self.width) .. "┐")
    for y = 1, self.height do
        print("│" .. table.concat(self.canvas[y], "") .. "│")
    end
    print("└" .. string.rep("─", self.width) .. "┘")
end

function SIK2D:getKey(key)
    return false
end

function SIK2D:wait(t)
    local start = os.clock()
    repeat until os.clock() - start >= t
end

function SIK2D:export(gameName)
    gameName = gameName or "game"
    
    local commands = {}
    for _, obj in ipairs(self.objects) do
        if obj.type == "rect" then
            table.insert(commands, string.format('    rect(%d, %d, %d, %d, \'%s\');', 
                obj.x, obj.y, obj.w, obj.h, obj.char))
        elseif obj.type == "circle" then
            table.insert(commands, string.format('    circle(%d, %d, %d, \'%s\');', 
                obj.x, obj.y, obj.r, obj.char))
        elseif obj.type == "text" then
            table.insert(commands, string.format('    text(%d, %d, "%s");', 
                obj.x, obj.y, obj.text))
        elseif obj.type == "line" then
            table.insert(commands, string.format('    line(%d, %d, %d, %d, \'%s\');', 
                obj.x1, obj.y1, obj.x2, obj.y2, obj.char))
        end
    end
    
    local c_code = string.format([[
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define WIDTH %d
#define HEIGHT %d

void rect(int x, int y, int w, int h, char ch) {
    printf("Rect: (%%d,%%d) %%dx%%d '%%c'\n", x, y, w, h, ch);
}

void circle(int x, int y, int r, char ch) {
    printf("Circle: (%%d,%%d) r=%%d '%%c'\n", x, y, r, ch);
}

void text(int x, int y, const char* txt) {
    printf("Text: '%%s' at (%%d,%%d)\n", txt, x, y);
}

void line(int x1, int y1, int x2, int y2, char ch) {
    printf("Line: (%%d,%%d)-(%%d,%%d) '%%c'\n", x1, y1, x2, y2, ch);
}

void game() {
%s
}

int main() {
    system("cls");
    printf("=== %%s ===\n", "%s");
    game();
    printf("\nPress Enter...");
    getchar();
    return 0;
}
]], self.width, self.height, table.concat(commands, "\n"), gameName)
    
    local file = io.open(gameName .. ".c", "w")
    file:write(c_code)
    file:close()
    
    print("Exported to " .. gameName .. ".c")
    print("Compile with: gcc -o " .. gameName .. ".exe " .. gameName .. ".c")
end

function rect(x, y, w, h, char)
    return SIK2D:addRect(x, y, w, h, char)
end

function circle(x, y, r, char)
    return SIK2D:addCircle(x, y, r, char)
end

function text(x, y, txt)
    return SIK2D:addText(x, y, txt)
end

function line(x1, y1, x2, y2, char)
    return SIK2D:addLine(x1, y1, x2, y2, char)
end

function show()
    SIK2D:draw()
end

function cls()
    SIK2D:clear()
end

function key(k)
    return SIK2D:getKey(k)
end

function wait(t)
    SIK2D:wait(t)
end

return SIK2D