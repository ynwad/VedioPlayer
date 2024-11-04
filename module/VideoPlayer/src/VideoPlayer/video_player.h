#ifndef VIDEO_PLAYER_H
#define VIDEO_PLAYER_H

#include <QObject>
#include <QImage>

#include <QThread>

extern "C"{
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavutil/time.h>
    #include <libavutil/pixfmt.h>
    #include <libavutil/display.h>
    #include <libavutil/avstring.h>
    #include <libavutil/opt.h>
    #include <libswscale/swscale.h>
    #include <libswresample/swresample.h>
    #include <libavutil/imgutils.h>
    #include <libavfilter/avfilter.h>
    #include <libavfilter/buffersink.h>
    #include <libavfilter/buffersrc.h>

    #include <SDL.h>
    #include <SDL_audio.h>
    #include <SDL_types.h>
    #include <SDL_name.h>
    #include <SDL_main.h>
    #include <SDL_config.h>
}

#include "types.h"
#include "Mutex/cond.h"
#include "EventHandle/video_player_event_handle.h"

class VideoPlayer{
public:
    VideoPlayer();

    ~VideoPlayer();

    void setVideoPlayerCallBack(VideoPlayerCallBack *pointer){
        m_videoPlayerCallBack = pointer;
    }

    ///初始化播放器（必需要调用一次）
    static bool initPlayer();

    bool startPlay(const std::string &strFilePath);

    // 获取视频总长度
    int64_t getTotalTime(); //单位微秒

private:
    ///回调函数
    VideoPlayerCallBack *m_videoPlayerCallBack;

    ///打开文件失败
    void doOpenVideoFileFailed(const int &code = 0);

    ///打开sdl失败的时候回调此函数
    void doOpenSdlFailed(const int &code);

    ///获取到视频时长的时候调用此函数
    void doTotalTimeChanged(const int64_t &uSec);

    ///播放器状态改变的时候回调此函数
    void doPlayerStateChanged(const VideoPlayerState &state, const bool &hasVideo, const bool &hasAudio);

    ///显示视频数据，此函数不宜做耗时操作，否则会影响播放的流畅性。
    void doDisplayVideo(const uint8_t *yuv420Buffer, const int &width, const int &height);

protected:
    void readVideoFile();

    void decodeVideoThread();

private:

    /// 音视频帧队列相关操作
    bool inputVideoQuene(const AVPacket &pkt);

    void clearVideoQuene();


    bool inputAudioQuene(const AVPacket &pkt);

    void clearAudioQuene();

    /// 本播放器中SDL仅用于播放音频，不用做别的用途
    /// SDL播放音频相关
    int openSDL();

    void closeSDL();

private:
    std::string m_strFilePath;  // 视频文件路径

    VideoPlayerState m_playerState;    // 播放状态

    // 音量相关
    bool m_bIsMute;
    float m_fVolume;

    // 播放控制相关
    bool mIsVideoThreadFinished; //视频解码线程
    bool mIsAudioThreadFinished; //音频播放线程

    bool m_bIsPause;  //暂停标志
    bool m_bIsQuit;   //停止
    bool m_bIsReadFinished; //文件读取完毕

    AVStream *m_videoStream; //视频流
    AVStream *m_audioStream; //音频流

    ///视频相关
    AVFormatContext *pFormatCtx;
    AVCodecContext *pCodecCtx;
    AVCodec *pCodec;

    ///音频相关
    AVCodecContext *aCodecCtx;
    AVCodec *aCodec;
    AVFrame *aFrame;

    ///SDL播放音频设备ID
    SDL_AudioDeviceID m_audioDevID;

    ///视频帧队列
    Cond *mConditon_Video;
    std::list<AVPacket> mVideoPacktList;

    ///音频帧队列
    Cond *mConditon_Audio;
    std::list<AVPacket> mAudioPacktList;

};

#endif // VIDEO_PLAYER_H
