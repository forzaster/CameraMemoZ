//
// Created by n-naka on 2016/12/30.
//

#include <jni.h>
#include <string>
#include "graphics/GLMain.h"

extern "C" {
JNIEXPORT void JNICALL Java_com_forzaster_cameramemoz_GLMain_init(JNIEnv * env, jobject obj,  jint width, jint height);
JNIEXPORT jboolean JNICALL Java_com_forzaster_cameramemoz_GLMain_resize(JNIEnv * env, jobject obj,  jint width, jint height);
JNIEXPORT void JNICALL Java_com_forzaster_cameramemoz_GLMain_draw(JNIEnv * env, jobject obj);
JNIEXPORT jint JNICALL Java_com_forzaster_cameramemoz_GLMain_genTexture(JNIEnv * env, jobject obj);
JNIEXPORT void JNICALL Java_com_forzaster_cameramemoz_GLMain_setVideoAspect(JNIEnv * env, jobject obj, jfloat aspect);
JNIEXPORT jstring JNICALL Java_com_forzaster_cameramemoz_GLMain_stringFromJNI(JNIEnv *env, jobject obj);
};

JNIEXPORT void JNICALL Java_com_forzaster_cameramemoz_GLMain_init(JNIEnv * env, jobject obj,  jint width, jint height)
{
    GLMain::instance().init(width, height);
}

JNIEXPORT jboolean JNICALL Java_com_forzaster_cameramemoz_GLMain_resize(JNIEnv * env, jobject obj,  jint width, jint height)
{
    return GLMain::instance().resize(width, height);
}

JNIEXPORT void JNICALL Java_com_forzaster_cameramemoz_GLMain_draw(JNIEnv * env, jobject obj)
{
    GLMain::instance().draw();
}

JNIEXPORT jint JNICALL Java_com_forzaster_cameramemoz_GLMain_genTexture(JNIEnv * env, jobject obj)
{
    return GLMain::instance().genTexture();
}

JNIEXPORT void JNICALL Java_com_forzaster_cameramemoz_GLMain_setVideoAspect(JNIEnv * env, jobject obj, jfloat aspect)
{
    return GLMain::instance().setVideoAspect(aspect);
}

JNIEXPORT jstring JNICALL Java_com_forzaster_cameramemoz_GLMain_stringFromJNI(JNIEnv *env, jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
