#ifndef __VIDEO_PLAYER_WIDGET_H__
#define __VIDEO_PLAYER_WIDGET_H__

#include <QWidget>
#include <QMenuBar>
#include <QPropertyAnimation>

#include "VideoPlayer/video_player.h"
#include "Widget/show_video_widget.h"
#include "draggable_widget.h"
#include "Widget/video_controller_widget.h"


class VideoPlayerWidget : public DragAbleWidget, public VideoPlayerCallBack
{
    Q_OBJECT

public:
    VideoPlayerWidget(QWidget *parent = nullptr);
    ~VideoPlayerWidget();

    void initUI();

    void initShortCut();

    void initMenuBar();

    void initSigSlots();

private:
    void showOutControlWidget(); //显示底部控制控件

    void hideControlWidget();    //隐藏底部控制控件

public slots:
    void onActionOpenFile();

    void onActionOpenSelectedWidget(int nTableWidgetIndex);
//    void signal_playMedia(QStringList lstPath)
    void on_PlaplayMedia(QStringList lstPath);

    // 暂停/播放
    void slotPlay();

    void slotPause();

    void slotStop(bool checked = false);

    void slotFastForward(bool checked = false);

    void slotFastBackward(bool checked = false);

    void slotVideoSliderMoved(int nValue);

    void slotAudioSliderMoved(int nValue);

    void slotProgressTimeOut();

protected:
    virtual QMenuBar* menuBar() override;

    virtual void resizeEvent(QResizeEvent *event) override;

    virtual void mouseMoveEvent(QMouseEvent *event) override;
    ///打开文件失败
    virtual void onOpenVideoFileFailed(const int &code) override;

    ///打开sdl失败的时候回调此函数
    virtual void onOpenSdlFailed(const int &code) override;

    ///获取到视频时长的时候调用此函数
    virtual void onTotalTimeChanged(const int64_t &uSec) override;

    ///播放器状态改变的时候回调此函数
    virtual void onPlayerStateChanged(const VideoPlayerState &state, const bool &hasVideo, const bool &hasAudio) override;

    ///显示视频数据，此函数不宜做耗时操作，否则会影响播放的流畅性。
    virtual void onDisplayVideo(VideoFrame::ptr videoFrame) override;

private:

    VideoPlayer *m_player;
    ShowVideoWidget *m_showVideoWidget;
    VideoControllerWidget *m_videoControllerWidget;
    QPropertyAnimation *m_animationControlWidget;   //控制底部控制控件的出现和隐藏

    QMenuBar* m_menuBar;
    bool m_bVideoCtlWidgetShow{false};
    QTimer *m_progressTimer;
    QLabel* m_labelLogo;    // 暂停时显示LOGO
};

#endif // __VIDEO_PLAYER_WIDGET_H__
