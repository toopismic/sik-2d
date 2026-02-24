#ifndef SID2D_H
#define SID2D_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifdef _WIN32
    #include <windows.h>
    #include <conio.h>
#elif __linux__
    #include <unistd.h>
    #include <termios.h>
    #include <fcntl.h>
    #include <sys/select.h>
#elif __ANDROID__
    #include <jni.h>
    #include <android/log.h>
#endif

// Lua
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

// Константы
#define MAX_OBJECTS 256
#define MAX_FILES 64
#define MAX_NAME 64
#define MAX_PATH 256
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define FPS 60

// Типы объектов
typedef enum {
    OBJ_RECT,
    OBJ_CIRCLE,
    OBJ_TRIANGLE
} ObjectType;

// Структура объекта
typedef struct {
    char name[MAX_NAME];
    ObjectType type;
    float x, y;
    float w, h;
    int visible;
    int tapHandler; // индекс Lua функции
} GameObject;

// Структура файла
typedef struct {
    char name[MAX_NAME];
    char content[1024];
} FileData;

// Главная структура движка
typedef struct {
    lua_State* L;
    GameObject objects[MAX_OBJECTS];
    int objectCount;
    FileData files[MAX_FILES];
    int fileCount;
    int running;
    float dt;
    char platform[16];
    void* platformData;
} SiD2D_Engine;

// Глобальный экземпляр
static SiD2D_Engine engine;

// Прототипы функций
void SiD2D_Init(const char* platform);
void SiD2D_Run();
void SiD2D_Shutdown();

// Lua функции
static int lua_newRect(lua_State* L);
static int lua_newCircle(lua_State* L);
static int lua_newTriangle(lua_State* L);
static int lua_newFile(lua_State* L);
static int lua_getFile(lua_State* L);
static int lua_newS(lua_State* L);
static int lua_stop(lua_State* L);
static int lua_start(lua_State* L);
static int lua_tap(lua_State* L);
static int lua_getMouse(lua_State* L);
static int lua_setPosition(lua_State* L);
static int lua_getPosition(lua_State* L);
static int lua_createList(lua_State* L);

#endif