#include <cstring>
#include <pthread.h>
#include "MyFfmpeg.h"
#include "macro.h"
#include "VedioDecode.h"
extern  "C" {
#include <libavformat/avformat.h>
}
//
// Created by 袁劲松 on 18/9/11.
//
void* threadTask(void* arg){
    MyFfmpeg *ffmpeg = static_cast<MyFfmpeg *> (arg);
    ffmpeg->_prepare();
    //线程函数必须返回一个值，否则会出错
    return 0;
}
void* threadStart(void* arg){
    MyFfmpeg *ffmpeg = static_cast<MyFfmpeg *> (arg);
    ffmpeg->_start();
    //线程函数必须返回一个值，否则会出错
    return 0;
}
MyFfmpeg::MyFfmpeg(JniHelper *jniHelper,const char* source) {
    //保存传过来的地址，防止被释放
    this->temp = new char[strlen(source)];
    strcpy(this->temp,source);
    this->jniHelper = jniHelper;
}
MyFfmpeg::~MyFfmpeg() {
    delete this->temp;
    temp=0;
}
void MyFfmpeg::prepare() {
    //打开视频流需要网络，子线程启用网络
    pthread_create(&pid,0,threadTask,this);
}
void MyFfmpeg::start() {
    //开始解码的Java函数有可能是主线程调用的
    isPlaying=1;

    if (audio){
       audio->queue.setWork(1);
        audio->play();
    }
    if (vedio){
//todo开始操作
        vedio->queue.setWork(1);
        if (audio){
            vedio->setAudioChannel(audio);
        }
        vedio->play();
    }
    pthread_create(&pid_start,0,threadStart,this);

}
void MyFfmpeg::_prepare() {
    //初始化FFmpeg网络
    avformat_network_init();
    formatContext=0;
    //打开流
    int ret = avformat_open_input(&formatContext,temp,0,0);
    if (ret!=0){
        jniHelper->onError(THREAD_CHILD,FFMPEG_CAN_NOT_OPEN_URL);
        return;
    }
    //查找媒体的音视频流
    ret=avformat_find_stream_info(formatContext,0);
    if (ret <0){
        jniHelper->onError(THREAD_CHILD,FFMPEG_CAN_NOT_FIND_STREAMS);
        return;
    }
    for (int i = 0; i < formatContext->nb_streams; ++i) {
        //获取某一个流
        AVStream *stream = formatContext->streams[i];
        //获取流的一些参数
        AVCodecParameters *parameters = stream->codecpar;
        //通过当前流查找流的解码器
        AVCodec *coder = avcodec_find_decoder(parameters->codec_id);
        if (coder==NULL){
            jniHelper->onError(THREAD_CHILD,FFMPEG_FIND_DECODER_FAIL);
            return;
        }
        //获得解码器上下文
        AVCodecContext *codecContext = avcodec_alloc_context3(coder);
        if (codecContext==NULL){
            jniHelper->onError(THREAD_CHILD,FFMPEG_ALLOC_CODEC_CONTEXT_FAIL);
            return;
        }
        //给上下文注册参数
        ret = avcodec_parameters_to_context(codecContext,parameters);
        if (ret<0){
            jniHelper->onError(THREAD_CHILD,FFMPEG_CODEC_CONTEXT_PARAMETERS_FAIL);
            return;
        }
        //打开解码器
        ret= avcodec_open2(codecContext,coder,0);
        if (ret<0){
            jniHelper->onError(THREAD_CHILD,FFMPEG_OPEN_DECODER_FAIL);
            return;
        }
        //开始解码
        AVRational timebase = stream->time_base;
        if (parameters->codec_type==AVMEDIA_TYPE_AUDIO){
            audio = new AudioChannel(i,codecContext,timebase);
        } else if (parameters->codec_type==AVMEDIA_TYPE_VIDEO){
            //帧率： 单位时间内 需要显示多少个图像
            AVRational frame_rate = stream->avg_frame_rate;
            int fps = av_q2d(frame_rate);
            vedio = new VedioDecode(i,codecContext,fps,timebase);
            vedio->setCallPlay(callplay);

        }

    }
    jniHelper->onPrepareFinsh(THREAD_CHILD);



}
void  MyFfmpeg::_start(){
    //1.读取媒体数据包，将数据包保存到视频的队列里
    int ret;
    while (isPlaying) {
        AVPacket *packet = av_packet_alloc();
        ret = av_read_frame(formatContext , packet);
        //=0成功 其他:失败
        if (ret == 0) {
            //stream_index 这一个流的一个序号
            if (audio && packet->stream_index == audio->type) {
                audio->queue.push(packet);
            } else if (vedio && packet->stream_index == vedio->type) {
                vedio->queue.push(packet);
            }
        } else if (ret == AVERROR_EOF) {
            //读取完成 但是可能还没播放完

        } else {
            //
        }

    }
}
void MyFfmpeg::setvideoplaycallback(callPlay callplay) {
    this->callplay = callplay;
}

