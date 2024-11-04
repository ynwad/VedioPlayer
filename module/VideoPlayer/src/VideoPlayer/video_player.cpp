#include "video_player.h"
#include "spdlog/spdlog.h"

VideoPlayer::VideoPlayer(){

}

VideoPlayer::~VideoPlayer(){

}

bool VideoPlayer::initPlayer(){
    return false;
}

void VideoPlayer::readVideoFile(){
    const char * file_path = m_strFilePath.c_str();

    pFormatCtx = nullptr;
    pCodecCtx = nullptr;
    pCodec = nullptr;

    aCodecCtx = nullptr;
    aCodec = nullptr;
    aFrame = nullptr;

    // Allocate an AVFormatContext.
    pFormatCtx = avformat_alloc_context();

    AVDictionary* opts = nullptr;
    av_dict_set(&opts, "rtsp_transport", "tcp", 0); // 设置tcp or udp，默认一般优先tcp再尝试udp
    av_dict_set(&opts, "stimeout", "60000000", 0);  // 设置超时3秒

    if(avformat_open_input(&pFormatCtx, file_path, nullptr, &opts) != 0){
        SPDLOG_ERROR("can't open the file.");
        doOpenVideoFileFailed();
        goto end;
    }

    if (avformat_find_stream_info(pFormatCtx, nullptr) < 0){
        fprintf(stderr, "Could't find stream infomation.\n");
        doOpenVideoFileFailed();
        goto end;
    }

    int audioStream = -1;
    int videoStream = -1;

    ///循环查找视频中包含的流信息，
    for (int i = 0; i < pFormatCtx->nb_streams; i++)
    {
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            videoStream = i;
        }
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO  && audioStream < 0)
        {
            audioStream = i;
        }
    }

    doTotalTimeChanged(getTotalTime());

    /// 打开视频编码器, 并启动视频线程
    if(videoStream > 0){
        ///查找视频解码器
        if(avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[videoStream]->codecpar) < 0){
            SPDLOG_ERROR("Failed to copy codec parameters from video_stream->codecpar to codec_ctx.");
            goto end;
        }
        pCodec = avcodec_find_decoder(pCodecCtx->codec_id);

        if(pCodec == nullptr){
            SPDLOG_ERROR("PCodec not found.");
            doOpenVideoFileFailed();
            goto end;
        }

        ///打开视频解码器
        if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
        {
            SPDLOG_ERROR("Could not open video codec.");

            doOpenVideoFileFailed();
            goto end;
        }

        m_videoStream = pFormatCtx->streams[videoStream];

        /// 创建一个线程专门用来解码视频
        std::thread([&](){
            decodeVideoThread();
        }).detach();
    }

    /// 打开音频解码器
    if(audioStream >= 0){
        /// 查找音频解码器
        if (avcodec_parameters_to_context(aCodecCtx, pFormatCtx->streams[audioStream]->codecpar) < 0) {
            SPDLOG_ERROR("Failed to copy codec parameters from audio_stream->codecpar to codec_ctx.");
            goto end;
        }

        aCodec = avcodec_find_decoder(aCodecCtx->codec_id);

        if (aCodec == nullptr)
        {
            SPDLOG_ERROR("ACodec not found.");
            audioStream = -1;
        }
        else{
            if(avcodec_open2(aCodecCtx, aCodec, nullptr) < 0){
                SPDLOG_ERROR("Could not open audio codec.");
                doOpenVideoFileFailed();
                goto end;
            }

            /// 解码音频相关
            aFrame = av_frame_alloc();

            m_audioStream = pFormatCtx->streams[audioStream];

            /// 打开SDL播放声音
            int nCode = openSDL();
            if(nCode == 0){
                SDL_LockAudioDevice(m_audioDevID);
                SDL_PauseAudioDevice(m_audioDevID, 0);
                SDL_UnlockAudioDevice(m_audioDevID);\

                mIsAudioThreadFinished = false;
            }
            else{
                doOpenSdlFailed(nCode);
            }
        }
    }
end:
    clearAudioQuene();
    clearVideoQuene();

//    if (swrCtx != nullptr){
//        swr_free(&swrCtx);
//        swrCtx = nullptr;
//    }

    if (aFrame != nullptr){
        av_frame_free(&aFrame);
        aFrame = nullptr;
    }

//    if (aFrame_ReSample != nullptr){
//        av_frame_free(&aFrame_ReSample);
//        aFrame_ReSample = nullptr;
//    }

    if (aCodecCtx != nullptr){
        avcodec_close(aCodecCtx);
        aCodecCtx = nullptr;
    }

    if (pCodecCtx != nullptr){
        avcodec_close(pCodecCtx);
        pCodecCtx = nullptr;
    }

    SPDLOG_INFO("readFile finished");
}

void VideoPlayer::decodeVideoThread(){

}

int64_t VideoPlayer::getTotalTime(){
    return pFormatCtx->duration;
}

bool VideoPlayer::startPlay(const std::string &strFilePath){
    if(m_playerState != VideoPlayer_Stop){
        return false;
    }

    m_bIsQuit = false;
    m_bIsPause = false;
    if(!strFilePath.empty()){
        m_strFilePath = strFilePath;
    }

    //启动新的线程实现读取视频文件
    std::thread([&](VideoPlayer *pointer){
        pointer->readVideoFile();
    }, this).detach();

    return true;
}

bool VideoPlayer::inputVideoQuene(const AVPacket &pkt){

}

void VideoPlayer::clearVideoQuene(){

}

bool VideoPlayer::inputAudioQuene(const AVPacket &pkt){

}

void VideoPlayer::clearAudioQuene(){

}

int VideoPlayer::openSDL(){


}

void VideoPlayer::closeSDL(){

}

/// 回调
void VideoPlayer::doOpenVideoFileFailed(const int &code){
    SPDLOG_ERROR("Open Video File Failed, errorCode: {}", code);

    if (m_videoPlayerCallBack != nullptr)
    {
        m_videoPlayerCallBack->onOpenVideoFileFailed(code);
    }
}

///打开sdl失败的时候回调此函数
void VideoPlayer::doOpenSdlFailed(const int &code){
    SPDLOG_ERROR("Open Sdl Failed, errocode: {}", code);

    if (m_videoPlayerCallBack != nullptr)
    {
        m_videoPlayerCallBack->onOpenVideoFileFailed(code);
    }
}

///获取到视频时长的时候调用此函数
void VideoPlayer::doTotalTimeChanged(const int64_t &uSec){
    SPDLOG_INFO("Total Time Changed, totalTime: {}", uSec);

    if (m_videoPlayerCallBack != nullptr)
    {
        m_videoPlayerCallBack->onTotalTimeChanged(uSec);
    }
}

///播放器状态改变的时候回调此函数
void VideoPlayer::doPlayerStateChanged(const VideoPlayerState &state, const bool &hasVideo, const bool &hasAudio){
    SPDLOG_INFO("Player State Changed, VideoPlayerState: {}, hasVideo: {}, hasAudio: {}"
                , state, hasVideo, hasAudio);

    if (m_videoPlayerCallBack != nullptr)
    {
        m_videoPlayerCallBack->onPlayerStateChanged(state, hasVideo, hasAudio);
    }
}

///显示视频数据，此函数不宜做耗时操作，否则会影响播放的流畅性。
void VideoPlayer::doDisplayVideo(const uint8_t *yuv420Buffer, const int &width, const int &height){
    if (m_videoPlayerCallBack != nullptr)
    {
        VideoFrame::ptr pVideoFrame = std::make_shared<VideoFrame>();

        pVideoFrame->initBuffer(width, height);
        pVideoFrame->setYUVbuf(yuv420Buffer);

        m_videoPlayerCallBack->onDisplayVideo(pVideoFrame);
    }
}
