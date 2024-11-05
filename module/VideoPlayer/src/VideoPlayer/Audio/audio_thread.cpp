/**
 * 叶海辉
 * QQ群121376426
 * http://blog.yundiantech.com/
 */

#include "VideoPlayer/video_player.h"

#include "pcm_volume_control.h"

#include <stdio.h>

void VideoPlayer::sdlAudioCallBackFunc(void *userdata, Uint8 *stream, int len){
    VideoPlayer* player = (VideoPlayer*)userdata;
    player->sdlAudioCallBack(stream, len);
}

void VideoPlayer::sdlAudioCallBack(Uint8 *stream, int len){

}

void VideoPlayer::decodeAudioFrame(bool isBlock){

}
