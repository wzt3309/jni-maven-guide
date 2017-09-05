#include "learn_jni.h"
#include <string.h>

JNIEXPORT jint JNICALL Java_learn_jni_Sample01_square
  (JNIEnv *env, jobject obj, jint n)
  {
    return n*n;
  }

  JNIEXPORT jboolean JNICALL Java_learn_jni_Sample01_aBool
    (JNIEnv *env, jobject obj, jboolean boolean)
    {
        return !boolean;
    }

  JNIEXPORT jstring JNICALL Java_learn_jni_Sample01_text
     (JNIEnv *env, jobject obj, jstring string)
     {
        const char *str = (*env)->GetStringUTFChars(env, string, 0);
        char cap[128];
        strcpy(cap, str);
        (*env)->ReleaseStringUTFChars(env, string, str);
        return (*env)->NewStringUTF(env, cap);
     }
  JNIEXPORT jint JNICALL Java_learn_jni_Sample01_sum
    (JNIEnv *env, jobject obj, jintArray array)
    {
        int i, sum = 0;
        jsize len = (*env)->GetArrayLength(env, array);
        jint *body = (*env)->GetIntArrayElements(env, array, 0);
        for (i = 0;i < len;i++)
        {
            sum += body[i];
        }
        (*env)->ReleaseIntArrayElements(env, array, body, 0);
        return sum;
    }

  int main(){}
