//
// Created by 袁劲松 on 18/9/12.
//

#ifndef DNNDKCOURSE_JNIHELPER_H
#define DNNDKCOURSE_JNIHELPER_H

#include <jni.h>

class JniHelper{
public:
    JniHelper(JavaVM *vm,JNIEnv *env,jobject object);
    ~JniHelper();
    void onError(int thread,int error);
    void onPrepareFinsh(int thread);

private:
    JavaVM *vm;
    JNIEnv *env;
    jobject object;
    jmethodID methodID;
    jmethodID methodPrepareID;

};
#endif //DNNDKCOURSE_JNIHELPER_H
