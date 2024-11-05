#ifndef VIDEOPLAYERWIDGET_H
#define VIDEOPLAYERWIDGET_H

#include <QWidget>

#include "VideoPlayer/video_player.h"

QT_BEGIN_NAMESPACE
namespace Ui { class VideoPlayerWidget; }
QT_END_NAMESPACE

class VideoPlayerWidget : public QWidget, public VideoPlayerCallBack
{
    Q_OBJECT

public:
    VideoPlayerWidget(QWidget *parent = nullptr);
    ~VideoPlayerWidget();

protected:
    ///打开文件失败
    void onOpenVideoFileFailed(const int &code);

    ///打开sdl失败的时候回调此函数
    void onOpenSdlFailed(const int &code);

    ///获取到视频时长的时候调用此函数
    void onTotalTimeChanged(const int64_t &uSec);

    ///播放器状态改变的时候回调此函数
    void onPlayerStateChanged(const VideoPlayerState &state, const bool &hasVideo, const bool &hasAudio);

    ///显示视频数据，此函数不宜做耗时操作，否则会影响播放的流畅性。
    void onDisplayVideo(VideoFrame::ptr videoFrame);

private:
    Ui::VideoPlayerWidget *ui;

    VideoPlayer *m_player;

};
#endif // VIDEOPLAYERWIDGET_H
