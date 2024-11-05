/**
 * 叶海辉
 * QQ群121376426
 * http://blog.yundiantech.com/
 */

#include "VideoPlayer/video_player.h"

//static const struct TextureFormatEntry {
//    enum AVPixelFormat format;
//    int texture_fmt;
//} sdl_texture_format_map[] = {
//    { AV_PIX_FMT_RGB8,           SDL_PIXELFORMAT_RGB332 },
//    { AV_PIX_FMT_RGB444,         SDL_PIXELFORMAT_RGB444 },
//    { AV_PIX_FMT_RGB555,         SDL_PIXELFORMAT_RGB555 },
//    { AV_PIX_FMT_BGR555,         SDL_PIXELFORMAT_BGR555 },
//    { AV_PIX_FMT_RGB565,         SDL_PIXELFORMAT_RGB565 },
//    { AV_PIX_FMT_BGR565,         SDL_PIXELFORMAT_BGR565 },
//    { AV_PIX_FMT_RGB24,          SDL_PIXELFORMAT_RGB24 },
//    { AV_PIX_FMT_BGR24,          SDL_PIXELFORMAT_BGR24 },
//    { AV_PIX_FMT_0RGB32,         SDL_PIXELFORMAT_RGB888 },
//    { AV_PIX_FMT_0BGR32,         SDL_PIXELFORMAT_BGR888 },
//    { AV_PIX_FMT_NE(RGB0, 0BGR), SDL_PIXELFORMAT_RGBX8888 },
//    { AV_PIX_FMT_NE(BGR0, 0RGB), SDL_PIXELFORMAT_BGRX8888 },
//    { AV_PIX_FMT_RGB32,          SDL_PIXELFORMAT_ARGB8888 },
//    { AV_PIX_FMT_RGB32_1,        SDL_PIXELFORMAT_RGBA8888 },
//    { AV_PIX_FMT_BGR32,          SDL_PIXELFORMAT_ABGR8888 },
//    { AV_PIX_FMT_BGR32_1,        SDL_PIXELFORMAT_BGRA8888 },
//    { AV_PIX_FMT_YUV420P,        SDL_PIXELFORMAT_IYUV },
//    { AV_PIX_FMT_YUYV422,        SDL_PIXELFORMAT_YUY2 },
//    { AV_PIX_FMT_UYVY422,        SDL_PIXELFORMAT_UYVY },
//    { AV_PIX_FMT_NONE,           SDL_PIXELFORMAT_UNKNOWN },
//};

#if CONFIG_AVFILTER
int VideoPlayer::configure_filtergraph(AVFilterGraph *graph, const char *filtergraph, AVFilterContext *source_ctx, AVFilterContext *sink_ctx)
{
    int ret, i;
    int nb_filters = graph->nb_filters;
    AVFilterInOut *outputs = NULL, *inputs = NULL;

    if (filtergraph) {
        outputs = avfilter_inout_alloc();
        inputs  = avfilter_inout_alloc();
        if (!outputs || !inputs) {
            ret = AVERROR(ENOMEM);
            goto fail;
        }

        outputs->name       = av_strdup("in");
        outputs->filter_ctx = source_ctx;
        outputs->pad_idx    = 0;
        outputs->next       = NULL;

        inputs->name        = av_strdup("out");
        inputs->filter_ctx  = sink_ctx;
        inputs->pad_idx     = 0;
        inputs->next        = NULL;

        if ((ret = avfilter_graph_parse_ptr(graph, filtergraph, &inputs, &outputs, NULL)) < 0)
            goto fail;
    } else {
        if ((ret = avfilter_link(source_ctx, 0, sink_ctx, 0)) < 0)
            goto fail;
    }

    /* Reorder the filters to ensure that inputs of the custom filters are merged first */
    for (i = 0; i < graph->nb_filters - nb_filters; i++)
        FFSWAP(AVFilterContext*, graph->filters[i], graph->filters[i + nb_filters]);

    ret = avfilter_graph_config(graph, NULL);
fail:
    avfilter_inout_free(&outputs);
    avfilter_inout_free(&inputs);
    return ret;
}

double get_rotation(AVStream *st)
{
    uint8_t* displaymatrix = av_stream_get_side_data(st,
                                                     AV_PKT_DATA_DISPLAYMATRIX, NULL);
    double theta = 0;
    if (displaymatrix)
        theta = -av_display_rotation_get((int32_t*) displaymatrix);

    theta -= 360*floor(theta/360 + 0.9/360);

    if (fabs(theta - 90*round(theta/90)) > 2)
        av_log(NULL, AV_LOG_WARNING, "Odd rotation angle.\n"
               "If you want to help, upload a sample "
               "of this file to ftp://upload.ffmpeg.org/incoming/ "
               "and contact the ffmpeg-devel mailing list. (ffmpeg-devel@ffmpeg.org)");

    return theta;
}

int VideoPlayer::configure_video_filters(AVFilterGraph *graph, const char *vfilters, AVFrame *frame)
{
//    enum AVPixelFormat pix_fmts[FF_ARRAY_ELEMS(sdl_texture_format_map)];
    char sws_flags_str[512] = "";
    char buffersrc_args[256];
    int ret;
    AVFilterContext *filt_src = NULL, *filt_out = NULL, *last_filter = NULL;
    AVCodecParameters *codecpar = this->mVideoStream->codecpar;
    AVRational fr = av_guess_frame_rate(this->pFormatCtx, this->mVideoStream, NULL);
    AVDictionaryEntry *e = NULL;
    int nb_pix_fmts = 0;

//    int i, j;
//    for (i = 0; i < renderer_info.num_texture_formats; i++)
//    {
//        for (j = 0; j < FF_ARRAY_ELEMS(sdl_texture_format_map) - 1; j++)
//        {
//            if (renderer_info.texture_formats[i] == sdl_texture_format_map[j].texture_fmt)
//            {
//                pix_fmts[nb_pix_fmts++] = sdl_texture_format_map[j].format;
//                break;
//            }
//        }
//    }
//    pix_fmts[nb_pix_fmts] = AV_PIX_FMT_NONE;

//    while ((e = av_dict_get(sws_dict, "", e, AV_DICT_IGNORE_SUFFIX)))
//    {
//        if (!strcmp(e->key, "sws_flags"))
//        {
//            av_strlcatf(sws_flags_str, sizeof(sws_flags_str), "%s=%s:", "flags", e->value);
//        }
//        else
//        {
//            av_strlcatf(sws_flags_str, sizeof(sws_flags_str), "%s=%s:", e->key, e->value);
//        }
//    }

    if (strlen(sws_flags_str))
        sws_flags_str[strlen(sws_flags_str)-1] = '\0';

    graph->scale_sws_opts = av_strdup(sws_flags_str);

    snprintf(buffersrc_args, sizeof(buffersrc_args),
             "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
             frame->width, frame->height, frame->format,
             this->mVideoStream->time_base.num, this->mVideoStream->time_base.den,
             codecpar->sample_aspect_ratio.num, FFMAX(codecpar->sample_aspect_ratio.den, 1));

    if (fr.num && fr.den)
        av_strlcatf(buffersrc_args, sizeof(buffersrc_args), ":frame_rate=%d/%d", fr.num, fr.den);

    if ((ret = avfilter_graph_create_filter(&filt_src,
                                            avfilter_get_by_name("buffer"),
                                            "ffplay_buffer", buffersrc_args, NULL,
                                            graph)) < 0)
    {
        goto fail;
    }

    ret = avfilter_graph_create_filter(&filt_out,
                                       avfilter_get_by_name("buffersink"),
                                       "ffplay_buffersink", NULL, NULL, graph);
    if (ret < 0)
        goto fail;

//    if ((ret = av_opt_set_int_list(filt_out, "pix_fmts", pix_fmts,  AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN)) < 0)
//        goto fail;

    last_filter = filt_out;

/* Note: this macro adds a filter before the lastly added filter, so the
 * processing order of the filters is in reverse */
#define INSERT_FILT(name, arg) do {                                          \
    AVFilterContext *filt_ctx;                                               \
                                                                             \
    ret = avfilter_graph_create_filter(&filt_ctx,                            \
                                       avfilter_get_by_name(name),           \
                                       "ffplay_" name, arg, NULL, graph);    \
    if (ret < 0)                                                             \
        goto fail;                                                           \
                                                                             \
    ret = avfilter_link(filt_ctx, 0, last_filter, 0);                        \
    if (ret < 0)                                                             \
        goto fail;                                                           \
                                                                             \
    last_filter = filt_ctx;                                                  \
} while (0)

    if (autorotate) {
        double theta  = get_rotation(this->mVideoStream);
//        theta = 90.0f; //测试用
fprintf(stderr, "%s get_rotation：%d \n", __FUNCTION__, theta);
        if (fabs(theta - 90) < 1.0) {
            INSERT_FILT("transpose", "clock");
        } else if (fabs(theta - 180) < 1.0) {
            INSERT_FILT("hflip", NULL);
            INSERT_FILT("vflip", NULL);
        } else if (fabs(theta - 270) < 1.0) {
            INSERT_FILT("transpose", "cclock");
        } else if (fabs(theta) > 1.0) {
            char rotate_buf[64];
            snprintf(rotate_buf, sizeof(rotate_buf), "%f*PI/180", theta);
            INSERT_FILT("rotate", rotate_buf);
        }
    }

    if ((ret = configure_filtergraph(graph, vfilters, filt_src, last_filter)) < 0)
        goto fail;

    this->in_video_filter  = filt_src;
    this->out_video_filter = filt_out;

fail:
    return ret;
}

#endif  /* CONFIG_AVFILTER */

void VideoPlayer::decodeVideoThread(){
    SPDLOG_INFO("decode Video Thread start");

    m_bIsVideoThreadFinished = false;

    int videoWidth = 0;
    int videoHeight = 0;

    double video_pts = 0;  // 当前视频的pts(显示时间戳)
    double audio_pts = 0;  // 音频pts

    /// 解码视频相关
    AVFrame *pFrame = nullptr;
    AVFrame *pFrameYUV = nullptr;
    uint8_t *yuv420pBuffer = nullptr; // 解码后的yuv数据
    struct SwsContext *imgConvertCtx = nullptr;  // 用于解码后的视频格式转换

    //    AVCodecContext *pCodecCtx = m_videoStream->codecpar;

    pFrame = av_frame_alloc();

    while(true){
        if(m_bIsQuit){
            clearVideoQuene();  // 清空队列
            break;
        }
        if(m_bIsPause == true){
            mSleep(10);
            continue;
        }
        mConditon_Video->Lock();
        if(m_videoPacktList.size() <= 0){
            mConditon_Video->Unlock();
            if(m_bIsReadFinished){
                break;
            }
            else{
                // 队列只是暂时没有数据
                mSleep(1);
                continue;
            }
        }

        AVPacket pkt1 = m_videoPacktList.front();
        m_videoPacktList.pop_front();
        mConditon_Video->Unlock();

        AVPacket *packet = &pkt1;

        // 收到这个数据，说明刚执行过跳转， 现在需要把解码器的数据清除一下
        if(strcmp((char*)packet->data, FLUSH_DATA) == 0){
            avcodec_flush_buffers(pCodecCtx);
            av_packet_unref(packet);
            continue;
        }

        if(avcodec_send_packet(pCodecCtx, packet) != 0){
            SPDLOG_ERROR("input AVPacket to decoder failed!");
            av_packet_unref(packet);
            continue;
        }

        while(0 == avcodec_receive_frame(pCodecCtx, pFrame)){
            if(packet->dts == AV_NOPTS_VALUE && pFrame->opaque && *(uint64_t*) pFrame->opaque != AV_NOPTS_VALUE){
                video_pts = *(uint64_t *) pFrame->opaque;
            }
            else if(packet->dts != AV_NOPTS_VALUE){
                video_pts = packet->dts;
            }
            else{
                video_pts = 0;
            }

            video_pts *= av_q2d(m_videoStream->time_base);
            m_video_clock = video_pts;
            if(m_seek_flag_video){
                // 发生了跳转 则跳过关键帧到目的时间的这几帧
                if(video_pts < m_seek_time){
                    av_packet_unref(packet);
                    continue;
                }
                else{
                    m_seek_flag_video = 0;
                }
            }

            ///音视频同步，实现的原理就是，判断是否到显示此帧图像的时间了，没到则休眠5ms，然后继续判断
            while(true){
                if(m_bIsQuit){
                    break;
                }
                if(m_audioStream != nullptr && !m_bIsAudioThreadFinished){
                    if(m_bIsReadFinished && m_audioPacktList.size() <= 0){
                        break;
                    }
                    audio_pts = m_audio_clock;
                }
                else{
                    audio_pts = (av_gettime() - m_videoStartTime) / 1000000.0;
                    m_audio_clock = audio_pts;
                }
                //主要是 跳转的时候 我们把video_clock设置成0了
                //因此这里需要更新video_pts
                //否则当从后面跳转到前面的时候 会卡在这里
                video_pts = m_video_clock;

                if (video_pts <= audio_pts) break;

                int delayTime = (video_pts - audio_pts) * 1000;

                delayTime = delayTime > 5 ? 5:delayTime;

                if (!m_bIsNeedPause)
                {
                    mSleep(delayTime);
                }
            }

            if(pFrame->width != videoWidth || pFrame->height != videoHeight){
                videoWidth = pFrame->width;
                videoHeight = pFrame->height;

                if(pFrameYUV != nullptr){
                    av_free(pFrameYUV);
                }
                if(yuv420pBuffer != nullptr){
                    av_free(yuv420pBuffer);
                }
                if (imgConvertCtx != nullptr)
                {
                    sws_freeContext(imgConvertCtx);
                }

                pFrameYUV = av_frame_alloc();

                int yuvSize = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, videoWidth, videoHeight, 1);

                unsigned int numBytes = static_cast<unsigned int>(yuvSize);
                yuv420pBuffer = static_cast<uint8_t*>(av_malloc(numBytes * sizeof(uint8_t)));
                av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, yuv420pBuffer, AV_PIX_FMT_YUV420P, videoWidth, videoHeight, 1);;

                ///由于解码后的数据不一定都是yuv420p，因此需要将解码后的数据统一转换成YUV420P
                imgConvertCtx = sws_getContext(videoWidth, videoHeight,
                                               (AVPixelFormat)pFrame->format, videoWidth, videoHeight,
                                               AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
            }

            sws_scale(imgConvertCtx,
                      (uint8_t const * const *) pFrame->data,
                      pFrame->linesize, 0, videoHeight, pFrameYUV->data,
                      pFrameYUV->linesize);

            doDisplayVideo(yuv420pBuffer, videoWidth, videoHeight);

#if CONFIG_AVFILTER
            //            if (is->videoq.serial != is->viddec.pkt_serial)
            //                break;
        }
#endif
        if (m_bIsNeedPause)
        {
            m_bIsPause = true;
            m_bIsNeedPause = false;
        }
    }
        av_packet_unref(packet);
    }

    av_free(pFrame);

    if (pFrameYUV != nullptr)
    {
        av_free(pFrameYUV);
    }

    if (yuv420pBuffer != nullptr)
    {
        av_free(yuv420pBuffer);
    }

    if (imgConvertCtx != nullptr)
    {
        sws_freeContext(imgConvertCtx);
    }

    if (!m_bIsQuit)
    {
        m_bIsQuit = true;
    }

    m_bIsVideoThreadFinished = true;

    SPDLOG_INFO("decode Video Thread finished");

    return;
}
