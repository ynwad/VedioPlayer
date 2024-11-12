/**
 * 叶海辉
 * QQ群121376426
 * http://blog.yundiantech.com/
 */

#include "VideoPlayer/video_player.h"

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
                //主要是 跳转的时候 我们把video_clock设置成 0了
                //因此这里需要更新video_pts
                //否则当从后面跳转到前面的时候 会卡在这里
                video_pts = m_video_clock;

                if (video_pts <= audio_pts) break;

                int delayTime = (video_pts - audio_pts) * 1000;

                delayTime = delayTime > 5 ? 5 : delayTime;
//                SPDLOG_INFO("delayTime: {}", delayTime);
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
