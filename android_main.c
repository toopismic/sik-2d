#include "sid2d.h"
#include <jni.h>

JNIEXPORT void JNICALL
Java_com_sid2d_GameActivity_nativeInit(JNIEnv* env, jobject thiz) {
    SiD2D_Init("android");
}

JNIEXPORT void JNICALL
Java_com_sid2d_GameActivity_nativeTap(JNIEnv* env, jobject thiz, jfloat x, jfloat y) {
    checkTap(x, y);
}

JNIEXPORT void JNICALL
Java_com_sid2d_GameActivity_nativeUpdate(JNIEnv* env, jobject thiz) {
    lua_getglobal(engine.L, "update");
    if (lua_isfunction(engine.L, -1)) {
        lua_call(engine.L, 0, 0);
    }
}