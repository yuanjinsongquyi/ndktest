#include "JniHelper.h"
#include "macro.h"
//
// Created by 袁劲松 on 18/9/12.
//
JniHelper::JniHelper(JavaVM *vm, JNIEnv *env, jobject object) {
    this->vm=vm;
    this->env=env;
    this->object=env->NewGlobalRef(object);
    jclass errorClass = env->GetObjectClass(object);
    this->methodID=env->GetMethodID(errorClass,"onError","(I)V");
    this->methodPrepareID = env->GetMethodID(errorClass,"onPrepare","()V");
}
JniHelper::~JniHelper() {
    env->DeleteGlobalRef(object);
}
void JniHelper::onError(int thread, int error) {
    if (thread==THREAD_MAIN){
        env->CallVoidMethod(object,methodID,error);
    }else{
        JNIEnv *jniEnv;
        vm->AttachCurrentThread(&jniEnv,0);
        jniEnv->CallVoidMethod(object,methodID,error);
        vm->DetachCurrentThread();
    }
}
void JniHelper::onPrepareFinsh(int thread) {
    if (thread==THREAD_MAIN){
        env->CallVoidMethod(object,methodPrepareID);
    }else{
        JNIEnv *jniEnv;
        vm->AttachCurrentThread(&jniEnv,0);
        jniEnv->CallVoidMethod(object,methodPrepareID);
        vm->DetachCurrentThread();
    }
}
