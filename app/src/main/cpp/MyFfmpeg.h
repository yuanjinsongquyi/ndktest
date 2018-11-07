//
// Created by 袁劲松 on 18/9/11.
//

#ifndef DNNDKCOURSE_MYFFMPEG_H
#define DNNDKCOURSE_MYFFMPEG_H
extern  "C" {
#include <libavformat/avformat.h>
}
#include "JniHelper.h"
#include "VedioDecode.h"
#include "AudioDecode.h"

class MyFfmpeg{
public:
    MyFfmpeg(JniHelper *jniHelper,const char* source);
    ~MyFfmpeg();
    void prepare();
    void _prepare();
    void start();
    void _start();
    void setvideoplaycallback(callPlay callplay);

public:
    char* temp;
    pthread_t pid;
    pthread_t pid_start;
    JniHelper *jniHelper;
    int isPlaying = 0;
    //初始化为0防止指针指向以前的地址
    VedioDecode *vedio = 0;
    AudioChannel *audio = 0;
    callPlay callplay;
    AVFormatContext *formatContext = 0;

};
#endif //DNNDKCOURSE_MYFFMPEG_H
