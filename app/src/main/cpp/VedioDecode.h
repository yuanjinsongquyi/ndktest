//
// Created by 袁劲松 on 18/9/12.
//

#ifndef DNNDKCOURSE_VEDIODECODE_H
#define DNNDKCOURSE_VEDIODECODE_H
extern "C"{
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
};
#include "safe_queue.h"
#include "AudioDecode.h"

typedef void (*callPlay)(uint8_t *,int,int,int);
class VedioDecode{
public:
    VedioDecode(int type,AVCodecContext *context,int fps,AVRational timebase);
    ~VedioDecode();
    void play();
    void doDecode();
    void doPlay();
    void setCallPlay(callPlay callplay);
    void setAudioChannel(AudioChannel *audioChannel);

public:
    int type;
    SafeQueue<AVPacket*> queue;
    SafeQueue<AVFrame*> queue_frame;
    AVCodecContext *context;
    bool isplaying = 0;
    pthread_t thread_play;
    pthread_t thread_decode;
    SwsContext* swsContext = 0;
    int fps=0;
    callPlay callplay;
    AudioChannel *audioChannel;
    AVRational timebase;

};
#endif //DNNDKCOURSE_VEDIODECODE_H
