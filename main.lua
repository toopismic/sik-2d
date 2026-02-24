-- SiD2D Example Game

-- Создаем объекты
player = newRect("player", 100, 100, 50)
enemy = newCircle("enemy", 300, 200, 30)
bonus = newTriangle("bonus", 500, 400, 40)

-- Создаем файл с данными
newFile("save.txt", "Score: 0|Level: 1")

-- Читаем файл
getFile("save.txt", "gameData")

-- Переменные
local score = 0
local health = 100
local gameOver = false
local enemies = createList()
enemies[1] = enemy

-- Сообщение при старте
newS("Game Started!")

-- Функция старта
function start()
    print("Game initialized")
end

-- Функция обновления
function update()
    if gameOver then return end
    
    -- Движение игрока за мышкой
    if getMouse() == 1 then
        player:setPosition(mouseX, mouseY)
    end
    
    -- Проверка столкновений
    if checkCollision(player, enemy) then
        health = health - 10
        newS("Health: " .. health)
        
        if health <= 0 then
            gameOver = true
            newS("GAME OVER! Score: " .. score)
        end
    end
    
    -- Обновление счета
    score = score + 1
end

-- Обработка тапов
tap("player", function()
    newS("Player tapped! Health: " .. health)
end)

tap("enemy", function()
    newS("Enemy tapped!")
    enemy:setPosition(400, 300)
    score = score + 50
end)

tap("bonus", function()
    newS("Bonus collected! +100")
    score = score + 100
    bonus:stop()  -- Прячем бонус
    
    -- Показываем через 3 секунды
    wait(3, function()
        bonus:start()
    end)
end)

-- Вспомогательные функции
function checkCollision(obj1, obj2)
    local x1,y1 = obj1:getPosition()
    local x2,y2 = obj2:getPosition()
    local dx = math.abs(x1 - x2)
    local dy = math.abs(y1 - y2)
    return dx < 50 and dy < 50
end

function wait(seconds, callback)
    local start = os.clock()
    while os.clock() - start < seconds do
        -- Простая задержка
    end
    callback()
end

-- Сохранение игры
function saveGame()
    newFile("save.txt", "Score: " .. score .. "|Health: " .. health)
    newS("Game saved!")
end