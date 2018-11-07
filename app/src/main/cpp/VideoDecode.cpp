//
// Created by 袁劲松 on 18/9/12.
//

extern  "C" {
#include <libavutil/imgutils.h>
#include <libavutil/time.h>
}

#include "VedioDecode.h"
#include "macro.h"

void releaseAvPacket(AVPacket** packet) {
    if (packet) {
        av_packet_free(packet);
        //为什么用指针的指针？
        // 指针的指针能够修改传递进来的指针的指向
        *packet = 0;
    }
}
void releaseAvFrame(AVFrame** frame) {
    if (frame) {
        av_frame_free(frame);
        //为什么用指针的指针？
        // 指针的指针能够修改传递进来的指针的指向
        *frame = 0;
    }
}
/**
 * 丢包 直到下一个关键帧
 * BIP帧
 * @param q
 */

void dropAvPacket(queue<AVPacket *> &q) {
    while (!q.empty()) {
        AVPacket *packet = q.front();
        //如果不属于 I 帧
        if (packet->flags != AV_PKT_FLAG_KEY) {
            releaseAvPacket(&packet);
            q.pop();
        } else {
            break;
        }
    }
}

/**
 * 丢已经解码的图片
 * @param q
 */
void dropAvFrame(queue<AVFrame *> &q) {
    if (!q.empty()) {
        AVFrame *frame = q.front();
        releaseAvFrame(&frame);
        q.pop();
    }
}
void* palyTask(void* arg){
    VedioDecode *vedioDecode = static_cast<VedioDecode*> (arg);
    vedioDecode->doPlay();
    return 0;
}
void* decodeTask(void* arg){
    VedioDecode *vedioDecode = static_cast<VedioDecode*> (arg);
    vedioDecode->doDecode();
    return 0;
}
VedioDecode::VedioDecode(int type,AVCodecContext *context,int fps,AVRational timebase) {
    this->type = type;
    this->context = context;
    queue_frame.setReleaseCallback(releaseAvFrame);
    this->fps = fps;
    this->timebase = timebase;
}
VedioDecode::~VedioDecode() {
    queue_frame.clear();
}
void VedioDecode::play() {
    isplaying=1;
    //创建两个线程一个用于播放，一个用于解码
    queue_frame.setWork(1);
    queue.setWork(1);
    pthread_create(&thread_decode,0,decodeTask,this);
    pthread_create(&thread_play,0,palyTask,this);

}
void VedioDecode::doDecode() {
    AVPacket* avPacket = 0;
    int ret = 0;
    while (isplaying) {
        ret = queue.pop(avPacket);
        if (!isplaying) {
            break;
        }
        if (!ret){
            continue;
        }
        //将队列里待解码的包传给解码器
        ret=avcodec_send_packet(context,avPacket);
        //释放packet
        releaseAvPacket(&avPacket);
        if(ret!=0){
            break;
        }
        //获得一个包的具体图像
        AVFrame* frame = av_frame_alloc();
        ret=avcodec_receive_frame(context,frame);
        if (ret==AVERROR(EAGAIN)){
            //需要更多的packet才能够完整的展现一个frame画面
            continue;
        }else if (ret!=0){
            break;
        }
        //开一个线程去播放frame
        queue_frame.push(frame);
    }
    releaseAvPacket(&avPacket);

}
void VedioDecode::doPlay() {
    //将frame的yua格式的数据转换为RGBf格式画到surfaceview上
    swsContext = sws_getContext(
            context->width, context->height,context->pix_fmt,
            context->width, context->height,AV_PIX_FMT_RGBA,
            SWS_BILINEAR,0,0,0);
    AVFrame* frame = 0;
    //指针数组
    uint8_t *dst_data[4];
    int dst_linesize[4];
    av_image_alloc(dst_data, dst_linesize,
                   context->width, context->height,AV_PIX_FMT_RGBA, 1);
    double frame_delays = 1.0 / fps;
    while (isplaying){
        int ret = queue_frame.pop(frame);
        if (!isplaying){
            break;
        }
        //src_linesize: 表示每一行存放的 字节长度
        sws_scale(swsContext, reinterpret_cast<const uint8_t *const *>(frame->data),
                  frame->linesize, 0,
                  context->height,
                  dst_data,
                  dst_linesize);
#if 1
        //获得 当前这一个画面 播放的相对的时间
        double clock = frame->best_effort_timestamp * av_q2d(timebase);
        //额外的间隔时间
        double extra_delay = frame->repeat_pict / (2 * fps);
        // 真实需要的间隔时间
        double delays = extra_delay + frame_delays;
        if (!audioChannel) {
            //休眠
//        //视频快了
//        av_usleep(frame_delays*1000000+x);
//        //视频慢了
//        av_usleep(frame_delays*1000000-x);
            av_usleep(delays * 1000000);
        } else {
            if (clock == 0) {
                av_usleep(delays * 1000000);
            } else {
                //比较音频与视频
                double audioClock = audioChannel->clock;
                //间隔 音视频相差的间隔
                double diff = clock - audioClock;
                if (diff > 0) {
                    //大于0 表示视频比较快
                    LOGE("视频快了：%lf",diff);
                    av_usleep((delays + diff) * 1000000);
                } else if (diff < 0) {
                    //小于0 表示音频比较快
                    LOGE("音频快了：%lf",diff);
                    // 视频包积压的太多了 （丢包）
                    if (fabs(diff) >= 0.05) {
                        releaseAvFrame(&frame);
                        //丢包
                        queue_frame.sync();
                        continue;
                    }else{
                        //不睡了 快点赶上 音频
                    }
                }
            }
        }
#endif

        //回调出去进行播放
        callplay(dst_data[0],dst_linesize[0],context->width, context->height);
        releaseAvFrame(&frame);
    }
    av_freep(&dst_data[0]);
    releaseAvFrame(&frame);
}
void VedioDecode::setCallPlay(callPlay callplay) {
    this->callplay = callplay;
}
void VedioDecode::setAudioChannel(AudioChannel *audioChannel) {
    this->audioChannel = audioChannel;
}


