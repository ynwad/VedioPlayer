
/**
 * 叶海辉
 * QQ群121376426
 * http://blog.yundiantech.com/
 */

#include <QApplication>
#include <QTextCodec>

#include "VideoPlayer/video_player.h"

#undef main
int main(int argc, char *argv[])
{
    VideoPlayer *mPlayer = new VideoPlayer();
//    mPlayer->setVideoPlayerCallBack(this);

    mPlayer->startPlay("F:/test.rmvb");

    return 0;
}

