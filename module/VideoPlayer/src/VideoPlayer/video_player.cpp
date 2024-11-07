#include "video_player.h"

VideoPlayer::VideoPlayer(){
    mConditon_Video = new Cond;
    mConditon_Audio = new Cond;

    m_playerState = VideoPlayer_Stop;

    m_videoPlayerCallBack = nullptr;

    m_audioDevID = 0;
    m_bIsMute = false;
    m_bIsNeedPause = false;

    m_fVolume = 1.0;
}

VideoPlayer::~VideoPlayer(){

}

bool VideoPlayer::initPlayer(){
    static int bIsInited = false;
    if(!bIsInited){
//        av_register_all();
        avformat_network_init();
    }
    return true;
}

void VideoPlayer::readVideoFile(){
    ///SDL初始化需要放入子线程中，否则有些电脑会有问题。
    if (SDL_Init(SDL_INIT_AUDIO))
    {
        doOpenSdlFailed(-100);
        fprintf(stderr, "Could not initialize SDL - %s. \n", SDL_GetError());
        return;
    }

    m_bIsReadThreadFinished = false;
    m_bIsReadFinished = false;

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

    int audioStream = -1;
    int videoStream = -1;

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

    ///循环查找视频中包含的流信息，
    for (int i = 0; i < pFormatCtx->nb_streams; i++)
    {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            videoStream = i;
        }
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO  && audioStream < 0)
        {
            audioStream = i;
        }
    }

    doTotalTimeChanged(getTotalTime());

    /// 打开视频编码器, 并启动视频线程
    if(videoStream >= 0){
        pCodecCtx = avcodec_alloc_context3(NULL);
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
        aCodecCtx = avcodec_alloc_context3(NULL);
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

            //重采样设置选项-----------------------------------------------------------start
            aFrame_ReSample = nullptr;

            //frame->16bit 44100 PCM 统一音频采样格式与采样率
            swrCtx = nullptr;

            //输入的声道布局
            int in_ch_layout;

            //输出的声道布局
            out_ch_layout = av_get_default_channel_layout(audio_tgt_channels); ///AV_CH_LAYOUT_STEREO

            out_ch_layout &= ~AV_CH_LAYOUT_STEREO_DOWNMIX;

            /// 这里音频播放使用了固定的参数
            /// 强制将音频重采样成44100 双声道  AV_SAMPLE_FMT_S16
            /// SDL播放中也是用了同样的播放参数
            //重采样设置选项----------------
            //输入的采样格式
            in_sample_fmt = aCodecCtx->sample_fmt;
            //输出的采样格式 16bit PCM
            out_sample_fmt = AV_SAMPLE_FMT_S16;
            //输入的采样率
            in_sample_rate = aCodecCtx->sample_rate;
            //输入的声道布局
            in_ch_layout = aCodecCtx->channel_layout;

            //输出的采样率
            //            out_sample_rate = 44100;
            out_sample_rate = aCodecCtx->sample_rate;

            //输出的声道布局

            audio_tgt_channels = 2; ///av_get_channel_layout_nb_channels(out_ch_layout);
            out_ch_layout = av_get_default_channel_layout(audio_tgt_channels); ///AV_CH_LAYOUT_STEREO

            out_ch_layout &= ~AV_CH_LAYOUT_STEREO_DOWNMIX;

            /// 2019-5-13添加
            /// wav/wmv 文件获取到的aCodecCtx->channel_layout为0会导致后面的初始化失败，因此这里需要加个判断。
            if (in_ch_layout <= 0)
            {
                in_ch_layout = av_get_default_channel_layout(aCodecCtx->channels);
            }

            swrCtx = swr_alloc_set_opts(nullptr, out_ch_layout, out_sample_fmt, out_sample_rate,
                                        in_ch_layout, in_sample_fmt, in_sample_rate, 0, nullptr);

            /** Open the resampler with the specified parameters. */
            int ret = swr_init(swrCtx);
            if (ret < 0)
            {
                char buff[128]={0};
                av_strerror(ret, buff, 128);

                fprintf(stderr, "Could not open resample context %s\n", buff);
                swr_free(&swrCtx);
                swrCtx = nullptr;
                doOpenVideoFileFailed();
                goto end;
            }

            //存储pcm数据
            int out_linesize = out_sample_rate * audio_tgt_channels;

            //        out_linesize = av_samples_get_buffer_size(NULL, audio_tgt_channels, av_get_bytes_per_sample(out_sample_fmt), out_sample_fmt, 1);
            out_linesize = AVCODEC_MAX_AUDIO_FRAME_SIZE;


            /// 打开SDL播放声音
            int nCode = openSDL();
            if(nCode == 0){
                SDL_LockAudioDevice(m_audioDevID);
                SDL_PauseAudioDevice(m_audioDevID, 0);
                SDL_UnlockAudioDevice(m_audioDevID);\

                m_bIsAudioThreadFinished = false;
            }
            else{
                doOpenSdlFailed(nCode);
            }
        }
    }

    m_playerState = VideoPlayer_Playing;

    doPlayerStateChanged(VideoPlayer_Playing, m_videoStream != nullptr, m_audioStream != nullptr);

    m_videoStartTime = av_gettime();

    SPDLOG_INFO("m_bIsQuit={}  m_bIsPause={} ", m_bIsQuit, m_bIsPause);

    while(true){
        if(m_bIsQuit){
            // 停止播放
            break;
        }
        if(m_seek_req){
            // 跳转播放
        }
        //这里做了个限制  当队列里面的数据超过某个大小的时候 就暂停读取  防止一下子就把视频读完了，导致的空间分配不足
        //这个值可以稍微写大一些
        if(m_audioPacktList.size() > MAX_AUDIO_SIZE || m_videoPacktList.size() > MAX_VIDEO_SIZE){
            mSleep(10);
            continue;
        }
        if(m_bIsPause){
            mSleep(10);
            continue;
        }
        AVPacket packet;
        if(av_read_frame(pFormatCtx, &packet) < 0){
            m_bIsReadFinished = true;
            if(m_bIsQuit){
                break;
            }
            mSleep(10);
            continue;
        }
        if(packet.stream_index == videoStream){
            // 将数据存入队列 因此不调用 av_free_packet 释放
            inputVideoQuene(packet);
        }
        else if(packet.stream_index == audioStream ){
            if (m_bIsAudioThreadFinished){
                ///SDL没有打开，则音频数据直接释放
                av_packet_unref(&packet);
            }
            else{
                inputAudioQuene(packet);
                //这里我们将数据存入队列 因此不调用 av_free_packet 释放
            }
        }
        else{
            // Free the packet that was allocated by av_read_frame
            av_packet_unref(&packet);
        }
    }

    /// 文件读取结束 跳出循环的情况
    /// 等待播放完毕
    while(!m_bIsQuit){
        mSleep(100);
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

    avformat_close_input(&pFormatCtx);
    avformat_free_context(pFormatCtx);

    SDL_Quit();

    doPlayerStateChanged(VideoPlayer_Stop, m_videoStream != nullptr, m_audioStream != nullptr);

    m_bIsReadThreadFinished = true;

    SPDLOG_INFO("readFile finished");
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
    m_bIsReadFinished = false;
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
    AVPacket new_pkt;
    if (av_packet_ref(&new_pkt, &pkt) < 0){
        return false;
    }
    mConditon_Video->Lock();
    m_videoPacktList.push_back(new_pkt);
    mConditon_Video->Signal();
    mConditon_Video->Unlock();
    return true;
}

void VideoPlayer::clearVideoQuene(){
    mConditon_Video->Lock();
    for (AVPacket &pkt : m_videoPacktList){
        av_packet_unref(&pkt);
    }
    m_videoPacktList.clear();
    mConditon_Video->Unlock();
}

bool VideoPlayer::inputAudioQuene(const AVPacket &pkt){
    AVPacket new_pkt;
    if (av_packet_ref(&new_pkt, &pkt) < 0){
        return false;
    }
    mConditon_Audio->Lock();
    m_audioPacktList.push_back(new_pkt);
    mConditon_Audio->Signal();
    mConditon_Audio->Unlock();
    return true;
}

void VideoPlayer::clearAudioQuene(){
    mConditon_Audio->Lock();
    for (AVPacket &pkt : m_audioPacktList){
        av_packet_unref(&pkt);
    }
    m_audioPacktList.clear();
    mConditon_Audio->Unlock();
}

int VideoPlayer::openSDL(){
    ///打开SDL，并设置播放的格式为:AUDIO_S16LSB 双声道，44100hz
    ///后期使用ffmpeg解码完音频后，需要重采样成和这个一样的格式，否则播放会有杂音
    SDL_AudioSpec wanted_spec, spec;
    int wanted_nb_channels = 2;
    int sample_rate = out_sample_rate;

    wanted_spec.channels = wanted_nb_channels;
    wanted_spec.samples = FFMAX(512, 2 << av_log2(wanted_spec.freq / 30));
    wanted_spec.freq = sample_rate;
    wanted_spec.format = AUDIO_S16SYS; // 具体含义请查看“SDL宏定义”部分
    wanted_spec.silence = 0;            // 0指示静音
    //    wanted_spec.samples = SDL_AUDIO_BUFFER_SIZE;  // 自定义SDL缓冲区大小
    wanted_spec.callback = sdlAudioCallBackFunc;  // 回调函数
    wanted_spec.userdata = this;                  // 传给上面回调函数的外带数据

    int num = SDL_GetNumAudioDevices(0);
    for (int i=0;i<num;i++)
    {
        m_audioDevID = SDL_OpenAudioDevice(SDL_GetAudioDeviceName(i,0), false, &wanted_spec, &spec,0);
        if (m_audioDevID > 0)
        {
            break;
        }
    }

    /* 检查实际使用的配置（保存在spec,由SDL_OpenAudio()填充） */
    //    if (spec.format != AUDIO_S16SYS)
    if (m_audioDevID <= 0)
    {
        m_bIsAudioThreadFinished = true;
        return -1;
    }
    SPDLOG_INFO("m_audioDevID = %d\n", m_audioDevID);
    return 0;
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
//    SPDLOG_INFO("Player State Changed, VideoPlayerState: {}, hasVideo: {}, hasAudio: {}"
//                , state, hasVideo, hasAudio);

    if (m_videoPlayerCallBack != nullptr)
    {
        m_videoPlayerCallBack->onPlayerStateChanged(state, hasVideo, hasAudio);
    }
}

///显示视频数据，此函数不宜做耗时操作，否则会影响播放的流畅性。
void VideoPlayer::doDisplayVideo(const uint8_t *yuv420Buffer, const int &width, const int &height){
    if (m_videoPlayerCallBack != nullptr)
    {
        SPDLOG_INFO("do once DisplayVideo");
        VideoFrame::ptr pVideoFrame = std::make_shared<VideoFrame>();

        pVideoFrame->initBuffer(width, height);
        pVideoFrame->setYUVbuf(yuv420Buffer);

        m_videoPlayerCallBack->onDisplayVideo(pVideoFrame);
    }
}
