#include "video_player_widget.h"

#include <QMessageBox>
#include <QTimer>

#include "Base/function_transfer.h"
#include "ui_video_player_widget.h"

VideoPlayerWidget::VideoPlayerWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::VideoPlayerWidget)
{
    ui->setupUi(this);

    FunctionTransfer::init(QThread::currentThreadId());

    m_player = new VideoPlayer();
    m_player->setVideoPlayerCallBack(this);

    QTimer::singleShot(1000, [&](){
        m_player->startPlay("E:/test.mp4");
    });
}

VideoPlayerWidget::~VideoPlayerWidget()
{
    delete ui;
}



///打开文件失败
void VideoPlayerWidget::onOpenVideoFileFailed(const int &code)
{
    FunctionTransfer::runInMainThread([=](){
        QMessageBox::critical(NULL, "tips", QString("open file failed %1").arg(code));
    });
}

///打开SDL失败的时候回调此函数
void VideoPlayerWidget::onOpenSdlFailed(const int &code)
{
    FunctionTransfer::runInMainThread([=](){
        QMessageBox::critical(NULL, "tips", QString("open Sdl failed %1").arg(code));
    });
}

///获取到视频时长的时候调用此函数
void VideoPlayerWidget::onTotalTimeChanged(const int64_t &uSec)
{
    FunctionTransfer::runInMainThread([=](){
        qint64 Sec = uSec/1000000;

//        ui->horizontalSlider->setRange(0,Sec);

        QString totalTime;
        QString hStr = QString("0%1").arg(Sec/3600);
        QString mStr = QString("0%1").arg(Sec / 60 % 60);
        QString sStr = QString("0%1").arg(Sec % 60);
        if (hStr == "00")
        {
//            otalTime = QString("%1:%2").arg(mStr.right(2)).arg(sStr.right(2));
        }
        else
        {
          totalTime = QString("%1:%2:%3").arg(hStr).arg(mStr.right(2)).arg(sStr.right(2));
        }

//        ui->label_totaltime->setText(totalTime);
    });
}

///播放器状态改变的时候回调此函数
void VideoPlayerWidget::onPlayerStateChanged(const VideoPlayerState &state, const bool &hasVideo, const bool &hasAudio)
{

}

///显示视频数据，此函数不宜做耗时操作，否则会影响播放的流畅性。
void VideoPlayerWidget::onDisplayVideo(std::shared_ptr<VideoFrame> videoFrame)
{
    FunctionTransfer::runInMainThread([=](){
        QImage img = videoFrame->YUV420pToQImage();
        ui->label->setPixmap(QPixmap::fromImage(img));
//        QMessageBox::critical(NULL, "tips", QString("open Sdl failed %1").arg(code));
    });
//    ui->widget_videoPlayer->inputOneFrame(videoFrame);
}
