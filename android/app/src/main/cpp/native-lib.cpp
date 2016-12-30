#include <jni.h>
#include <string>

#if 0
extern "C"
jstring
Java_com_forzaster_cameramemoz_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
#endif
