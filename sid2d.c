#include "sid2d.h"

// Инициализация
void SiD2D_Init(const char* platform) {
    memset(&engine, 0, sizeof(SiD2D_Engine));
    strcpy(engine.platform, platform);
    engine.running = 1;
    
    // Создаем Lua состояние
    engine.L = luaL_newstate();
    luaL_openlibs(engine.L);
    
    // Регистрируем функции
    lua_register(engine.L, "newRect", lua_newRect);
    lua_register(engine.L, "newCircle", lua_newCircle);
    lua_register(engine.L, "newTriangle", lua_newTriangle);
    lua_register(engine.L, "newFile", lua_newFile);
    lua_register(engine.L, "getFile", lua_getFile);
    lua_register(engine.L, "newS", lua_newS);
    lua_register(engine.L, "stop", lua_stop);
    lua_register(engine.L, "start", lua_start);
    lua_register(engine.L, "tap", lua_tap);
    lua_register(engine.L, "getMouse", lua_getMouse);
    lua_register(engine.L, "createList", lua_createList);
    
    // Добавляем методы для объектов
    luaL_dostring(engine.L, 
        "local mt = {}\n"
        "function mt:setPosition(x,y) setPosition(self.name,x,y) end\n"
        "function mt:getPosition() return getPosition(self.name) end\n"
        "function mt:stop() stop(self.name) end\n"
        "function mt:start() start(self.name) end\n"
        "function newObject(name) return {name=name, setPosition=mt.setPosition, getPosition=mt.getPosition, stop=mt.stop, start=mt.start} end\n"
    );
    
    // Платформозависимая инициализация
    if (strcmp(platform, "windows") == 0) {
        #ifdef _WIN32
        // Windows консоль
        system("cls");
        printf("SiD2D Engine v1.0 - Windows\n");
        printf("Screen: %dx%d\n", SCREEN_WIDTH, SCREEN_HEIGHT);
        #endif
    }
    else if (strcmp(platform, "android") == 0) {
        #ifdef __ANDROID__
        __android_log_print(ANDROID_LOG_INFO, "SiD2D", "Engine initialized");
        #endif
    }
}

// Поиск объекта по имени
GameObject* findObject(const char* name) {
    for (int i = 0; i < engine.objectCount; i++) {
        if (strcmp(engine.objects[i].name, name) == 0) {
            return &engine.objects[i];
        }
    }
    return NULL;
}

// Создание прямоугольника
static int lua_newRect(lua_State* L) {
    const char* name = luaL_checkstring(L, 1);
    float x = (float)luaL_checknumber(L, 2);
    float y = (float)luaL_checknumber(L, 3);
    float size = (float)luaL_checknumber(L, 4);
    
    if (engine.objectCount >= MAX_OBJECTS) return 0;
    
    GameObject* obj = &engine.objects[engine.objectCount++];
    strcpy(obj->name, name);
    obj->type = OBJ_RECT;
    obj->x = x;
    obj->y = y;
    obj->w = size;
    obj->h = size;
    obj->visible = 1;
    obj->tapHandler = 0;
    
    // Возвращаем объект из Lua
    lua_getglobal(L, "newObject");
    lua_pushstring(L, name);
    lua_call(L, 1, 1);
    
    return 1;
}

// Создание круга
static int lua_newCircle(lua_State* L) {
    const char* name = luaL_checkstring(L, 1);
    float x = (float)luaL_checknumber(L, 2);
    float y = (float)luaL_checknumber(L, 3);
    float radius = (float)luaL_checknumber(L, 4);
    
    if (engine.objectCount >= MAX_OBJECTS) return 0;
    
    GameObject* obj = &engine.objects[engine.objectCount++];
    strcpy(obj->name, name);
    obj->type = OBJ_CIRCLE;
    obj->x = x;
    obj->y = y;
    obj->w = radius * 2;
    obj->h = radius * 2;
    obj->visible = 1;
    
    lua_getglobal(L, "newObject");
    lua_pushstring(L, name);
    lua_call(L, 1, 1);
    
    return 1;
}

// Создание треугольника
static int lua_newTriangle(lua_State* L) {
    const char* name = luaL_checkstring(L, 1);
    float x = (float)luaL_checknumber(L, 2);
    float y = (float)luaL_checknumber(L, 3);
    float size = (float)luaL_checknumber(L, 4);
    
    if (engine.objectCount >= MAX_OBJECTS) return 0;
    
    GameObject* obj = &engine.objects[engine.objectCount++];
    strcpy(obj->name, name);
    obj->type = OBJ_TRIANGLE;
    obj->x = x;
    obj->y = y;
    obj->w = size;
    obj->h = size;
    obj->visible = 1;
    
    lua_getglobal(L, "newObject");
    lua_pushstring(L, name);
    lua_call(L, 1, 1);
    
    return 1;
}

// Создание файла
static int lua_newFile(lua_State* L) {
    const char* filename = luaL_checkstring(L, 1);
    const char* content = luaL_checkstring(L, 2);
    
    if (engine.fileCount >= MAX_FILES) return 0;
    
    FileData* file = &engine.files[engine.fileCount++];
    strcpy(file->name, filename);
    strcpy(file->content, content);
    
    // Сохраняем на диск
    FILE* f = fopen(filename, "w");
    if (f) {
        fprintf(f, "%s", content);
        fclose(f);
    }
    
    return 0;
}

// Чтение файла
static int lua_getFile(lua_State* L) {
    const char* filename = luaL_checkstring(L, 1);
    const char* varName = luaL_checkstring(L, 2);
    
    char content[1024] = "";
    
    // Ищем в памяти
    for (int i = 0; i < engine.fileCount; i++) {
        if (strcmp(engine.files[i].name, filename) == 0) {
            strcpy(content, engine.files[i].content);
            break;
        }
    }
    
    // Если нет в памяти, читаем с диска
    if (strlen(content) == 0) {
        FILE* f = fopen(filename, "r");
        if (f) {
            fread(content, 1, sizeof(content)-1, f);
            fclose(f);
        }
    }
    
    // Создаем глобальную переменную
    lua_pushstring(L, content);
    lua_setglobal(L, varName);
    
    return 0;
}

// Показать сообщение
static int lua_newS(lua_State* L) {
    const char* msg = luaL_checkstring(L, 1);
    
    #ifdef _WIN32
    MessageBoxA(NULL, msg, "SiD2D", MB_OK);
    #elif __ANDROID__
    __android_log_print(ANDROID_LOG_INFO, "SiD2D", "%s", msg);
    // Тут можно вызвать Java Toast через JNI
    #else
    printf("\n📱 %s\n", msg);
    #endif
    
    return 0;
}

// Остановить объект (спрятать)
static int lua_stop(lua_State* L) {
    const char* name = luaL_checkstring(L, 1);
    GameObject* obj = findObject(name);
    if (obj) obj->visible = 0;
    return 0;
}

// Запустить объект (показать)
static int lua_start(lua_State* L) {
    const char* name = luaL_checkstring(L, 1);
    GameObject* obj = findObject(name);
    if (obj) obj->visible = 1;
    return 0;
}

// Установить позицию
static int lua_setPosition(lua_State* L) {
    const char* name = luaL_checkstring(L, 1);
    float x = (float)luaL_checknumber(L, 2);
    float y = (float)luaL_checknumber(L, 3);
    
    GameObject* obj = findObject(name);
    if (obj) {
        obj->x = x;
        obj->y = y;
    }
    
    return 0;
}

// Получить позицию
static int lua_getPosition(lua_State* L) {
    const char* name = luaL_checkstring(L, 1);
    
    GameObject* obj = findObject(name);
    if (obj) {
        lua_pushnumber(L, obj->x);
        lua_pushnumber(L, obj->y);
        return 2;
    }
    
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 0);
    return 2;
}

// Получить состояние мыши
static int lua_getMouse(lua_State* L) {
    #ifdef _WIN32
    lua_pushboolean(L, GetAsyncKeyState(VK_LBUTTON) & 0x8000);
    #else
    // Для простоты всегда 0
    lua_pushboolean(L, 0);
    #endif
    return 1;
}

// Создать список
static int lua_createList(lua_State* L) {
    lua_newtable(L);
    return 1;
}

// Обработка тапа
static int lua_tap(lua_State* L) {
    const char* objName = luaL_checkstring(L, 1);
    
    // Сохраняем функцию тапа для объекта
    if (lua_isfunction(L, 2)) {
        GameObject* obj = findObject(objName);
        if (obj) {
            // Сохраняем ссылку на функцию
            lua_pushvalue(L, 2);
            obj->tapHandler = luaL_ref(L, LUA_REGISTRYINDEX);
        }
    }
    
    return 0;
}

// Проверка тапа по объекту
void checkTap(float x, float y) {
    for (int i = 0; i < engine.objectCount; i++) {
        GameObject* obj = &engine.objects[i];
        if (!obj->visible) continue;
        
        int hit = 0;
        
        switch (obj->type) {
            case OBJ_RECT:
                hit = (x >= obj->x && x <= obj->x + obj->w &&
                       y >= obj->y && y <= obj->y + obj->h);
                break;
                
            case OBJ_CIRCLE: {
                float dx = x - (obj->x + obj->w/2);
                float dy = y - (obj->y + obj->h/2);
                float r = obj->w/2;
                hit = (dx*dx + dy*dy) <= r*r;
                break;
            }
            
            case OBJ_TRIANGLE: {
                // Простая проверка
                float cx = obj->x + obj->w/2;
                float cy = obj->y + obj->h/2;
                float dx = fabs(x - cx);
                float dy = fabs(y - cy);
                hit = (dx < obj->w/2 && dy < obj->h/2);
                break;
            }
        }
        
        if (hit && obj->tapHandler) {
            // Вызываем функцию тапа из Lua
            lua_rawgeti(engine.L, LUA_REGISTRYINDEX, obj->tapHandler);
            lua_call(engine.L, 0, 0);
            break;
        }
    }
}

// Главный цикл
void SiD2D_Run() {
    // Загружаем main.lua
    if (luaL_dofile(engine.L, "main.lua") != LUA_OK) {
        const char* err = lua_tostring(engine.L, -1);
        printf("Lua Error: %s\n", err);
        return;
    }
    
    // Вызываем start если есть
    lua_getglobal(engine.L, "start");
    if (lua_isfunction(engine.L, -1)) {
        lua_call(engine.L, 0, 0);
    }
    
    // Главный цикл
    while (engine.running) {
        engine.dt = 1.0f / FPS;
        
        // Обработка ввода
        #ifdef _WIN32
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            engine.running = 0;
        }
        
        // Симуляция тапа мышью
        if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
            POINT p;
            GetCursorPos(&p);
            checkTap((float)p.x, (float)p.y);
        }
        #endif
        
        // Вызываем update
        lua_getglobal(engine.L, "update");
        if (lua_isfunction(engine.L, -1)) {
            lua_call(engine.L, 0, 0);
        }
        
        // Отрисовка (консольная для примера)
        system("cls");
        printf("SiD2D - FPS: %d\n", FPS);
        printf("Objects: %d\n", engine.objectCount);
        
        for (int i = 0; i < engine.objectCount; i++) {
            GameObject* obj = &engine.objects[i];
            if (obj->visible) {
                printf("  %s: %s at (%.0f,%.0f)\n", 
                    obj->name,
                    obj->type == OBJ_RECT ? "⬜" : 
                    obj->type == OBJ_CIRCLE ? "⚪" : "▲",
                    obj->x, obj->y);
            }
        }
        
        // Задержка для 60 FPS
        #ifdef _WIN32
        Sleep(16);
        #endif
    }
}

// Завершение
void SiD2D_Shutdown() {
    lua_close(engine.L);
    printf("SiD2D shutdown\n");
}