#include <jni.h>


JNIEXPORT jstring JNICALL Java_com_example_jnitest_MainActivity_StringFromC
  (JNIEnv *env, jobject obj)
{
	return env->NewStringUTF("Hello from C code!");
}
