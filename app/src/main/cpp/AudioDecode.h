//
// Created by 袁劲松 on 18/9/12.
//

#ifndef DNNDKCOURSE_AUDIODECODE_H
#define DNNDKCOURSE_AUDIODECODE_H
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include "safe_queue.h"

extern  "C"{
#include <libswresample/swresample.h>
#include <libavcodec/avcodec.h>
};

class AudioChannel {
public:
    AudioChannel(int id,AVCodecContext *avCodecContext,AVRational timebase);
    ~AudioChannel();
    void play();

    void decode();

    void _play();

    int getPcm();

public:
    uint8_t *data = 0;
    int out_channels;
    int out_samplesize;
    int out_sample_rate;
    int type;
    AVCodecContext *avCodecContext;
    double clock ;
    AVRational timebase;
public:
    SafeQueue<AVPacket*> queue;
    SafeQueue<AVFrame*> queue_frame;
    bool isplaying = 0;
    pthread_t  pid_audio_decode;
    pthread_t  pid_audio_play;
    /**
     * OpenSL ES
     */
    // 引擎与引擎接口
    SLObjectItf engineObject = 0;
    SLEngineItf engineInterface = 0;
    //混音器
    SLObjectItf outputMixObject = 0;
    //播放器
    SLObjectItf bqPlayerObject = 0;
    //播放器接口
    SLPlayItf bqPlayerInterface = 0;

    SLAndroidSimpleBufferQueueItf bqPlayerBufferQueueInterface =0;


    //重采样
    SwrContext *swrContext = 0;


};

#endif //DNNDKCOURSE_AUDIODECODE_H
