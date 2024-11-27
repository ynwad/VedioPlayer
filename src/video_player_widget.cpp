#include "video_player_widget.h"

#include <QMessageBox>
#include <QTimer>
#include <QVBoxLayout>
#include <QShortcut>
#include <QMenuBar>
#include <QFileDialog>

#include "Base/define.h"
#include "Base/function_transfer.h"
#include "ui_video_player_widget.h"
#include "Widget/selected_media_widget.h"

#define _ST(str) QString::fromLocal8Bit(str)

VideoPlayerWidget::VideoPlayerWidget(QWidget *parent)
    : DragAbleWidget(parent){
//    ui->setupUi(this);

    FunctionTransfer::init(QThread::currentThreadId());

    m_player = new VideoPlayer();
    m_player->setVideoPlayerCallBack(this);

    m_showVideoWidget = new ShowVideoWidget(this);
    m_showVideoWidget->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    m_showVideoWidget->setMouseTracking(true);

    setMouseTracking(true);
    setAttribute(Qt::WA_TransparentForMouseEvents, true);

    m_videoControllerWidget = new VideoControllerWidget(this);

    m_animationControlWidget = new QPropertyAnimation(m_videoControllerWidget, "geometry");

    m_progressTimer = new QTimer(this);
    connect(m_progressTimer, &QTimer::timeout, this, &VideoPlayerWidget::slotProgressTimeOut);
    m_progressTimer->setInterval(500);

    initUI();

    initShortCut();

    initSigSlots();
}

VideoPlayerWidget::~VideoPlayerWidget(){
    delete ui;
}

void VideoPlayerWidget::initUI(){

    initMenuBar();

    QVBoxLayout* vLayout = new QVBoxLayout;

    vLayout->addWidget(m_menuBar);
    vLayout->addWidget(m_showVideoWidget);

    m_showVideoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->setSpacing(0);

    setLayout(vLayout);

    resize(400, 250);
}

void VideoPlayerWidget::initShortCut(){
    // 创建一个快捷键 (Ctrl + S)
    QShortcut *shortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    // 连接快捷键信号到槽函数
    connect(shortcut, &QShortcut::activated, [&](){
        if(isFullScreen()){
            // 退出全屏，恢复尺寸
            showNormal();
            resize(m_originalSize);  // 恢复到原始尺寸
        }
    });
}

void VideoPlayerWidget::initMenuBar(){
    // 菜单栏
    m_menuBar = new QMenuBar(this);
    QMenu* menuMedia = m_menuBar->addMenu(_ST("媒体(M)"));

//    menuMedia->addAction(QIcon(QPixmap("d:\\\\qt-logo.png")), "&New", this, SLOT(slotNew()), QKeySequence(tr("CTRL+N")));
    menuMedia->addAction(_ST("打开文件"), this, &VideoPlayerWidget::onActionOpenFile);
    menuMedia->addAction(_ST("打开网络串流"), [&](){
        onActionOpenSelectedWidget(0);
    });
}

void VideoPlayerWidget::initSigSlots(){
//    void notifyMouseEnter();
//    on_PlaplayMedia

//    void notifyMouseLeave();

}

void VideoPlayerWidget::showOutControlWidget(){
    if(m_bVideoCtlWidgetShow){
        return;
    }
    m_bVideoCtlWidgetShow = true;
    m_animationControlWidget->setDuration(1000);

    int w = m_videoControllerWidget->width();
    int h = m_videoControllerWidget->height();
    int x = 0;
    int y = height() - m_videoControllerWidget->height();

    if(m_videoControllerWidget->isHidden()){
        m_videoControllerWidget->show();
    }
    m_animationControlWidget->setStartValue(m_videoControllerWidget->geometry());

    m_animationControlWidget->setEndValue(QRect(x, y, w, h));
    m_animationControlWidget->setEasingCurve(QEasingCurve::Linear);

    m_animationControlWidget->start();
}

void VideoPlayerWidget::hideControlWidget(){
    if(!m_bVideoCtlWidgetShow){
        return;
    }
    m_bVideoCtlWidgetShow = false;
    m_animationControlWidget->setTargetObject(m_videoControllerWidget);
    m_animationControlWidget->setDuration(1000);

    int w = m_videoControllerWidget->width();
    int h = m_videoControllerWidget->height();
    int x = 0;
    int y = height() + h;

    m_animationControlWidget->setStartValue(m_videoControllerWidget->geometry());
    m_animationControlWidget->setEndValue(QRect(x, y, w, h));
    m_animationControlWidget->setEasingCurve(QEasingCurve::Linear);

    m_animationControlWidget->start();
}

void VideoPlayerWidget::onActionOpenFile(){
    // 设置文件过滤器，支持常见的视频和音频格式
    QString filter = "Media Files (*.mp4 *.avi *.mkv *.mov *.mp3 *.wav *.flac *.aac);;"
                     "Video Files (*.mp4 *.avi *.mkv *.mov);;"
                     "Audio Files (*.mp3 *.wav *.flac *.aac);;"
                     "All Files (*.*)";

    // 打开文件选择对话框
    QString fileName = QFileDialog::getOpenFileName(
        nullptr,
        "Select a Media File",
        QDir::homePath(), // 默认目录
        filter // 应用过滤器
        );

    // 如果选择了文件，打印文件路径
    if (fileName.isEmpty()) {
        SPDLOG_INFO("No file selected{}");
        return;
    }
    SPDLOG_INFO("Selected File: {}", fileName.toStdString());

    m_player->startPlay(fileName.toLocal8Bit().data());
}

void VideoPlayerWidget::onActionOpenSelectedWidget(int nTableWidgetIndex){
    if(nTableWidgetIndex < TableWidget_File_Index || nTableWidgetIndex > TableWidget_Capture_Index){
        SPDLOG_ERROR("TabWidget 下标设置错误");
        return;
    }
    SelectedMediaWidget* selectedWidget = new SelectedMediaWidget(this);
    selectedWidget->setAttribute(Qt::WA_DeleteOnClose);
    selectedWidget->setCurrentIndex(nTableWidgetIndex);

    connect(selectedWidget, &SelectedMediaWidget::signal_playMedia, this, &VideoPlayerWidget::on_PlaplayMedia);
}

void VideoPlayerWidget::on_PlaplayMedia(QStringList lstPath){
    qDebug() << "播放列表： " << lstPath;
    m_player->startPlay(lstPath[0].toStdString());
}

void VideoPlayerWidget::slotVideoSliderMoved(int nValue){
    m_player->seek((qint64)nValue * 1000000);
}

void VideoPlayerWidget::slotAudioSliderMoved(int nValue){
    m_player->setVolume(nValue / 100.0);
}

void VideoPlayerWidget::slotProgressTimeOut(){
    qint64 Sec = m_player->getCurrentTime();

    m_videoControllerWidget->setVideoSliderValue(Sec);

    QString curTime;
    QString hStr = QString("0%1").arg(Sec / 3600);
    QString mStr = QString("0%1").arg(Sec / 60 % 60);
    QString sStr = QString("0%1").arg(Sec % 60);
    if (hStr == "00")
    {
        curTime = QString("%1:%2").arg(mStr.right(2)).arg(sStr.right(2));
    }
    else
    {
        curTime = QString("%1:%2:%3").arg(hStr).arg(mStr.right(2)).arg(sStr.right(2));
    }

    m_videoControllerWidget->setVideoSliderCurTime(curTime);
}

QMenuBar* VideoPlayerWidget::menuBar(){
    return m_menuBar;
}

void VideoPlayerWidget::resizeEvent(QResizeEvent *event){
    m_videoControllerWidget->setFixedHeight(100);
    m_videoControllerWidget->setFixedWidth(width());
    if(m_bVideoCtlWidgetShow){
        m_videoControllerWidget->move(0, height() - m_videoControllerWidget->height());
    }
    else{
        m_videoControllerWidget->move(0, height());
    }

    QWidget::resizeEvent(event);
}

void VideoPlayerWidget::mouseMoveEvent(QMouseEvent *event){
    int nMouseY = event->y();
    int nVideoControlWidgetY = height() - m_videoControllerWidget->height();
    if(nMouseY > nVideoControlWidgetY){
        showOutControlWidget();
    }
    else{
//        QTimer::singleShot(1000, [&](){
            hideControlWidget();
//        });
    }
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

        m_videoControllerWidget->setVideoSliderRange(0, Sec);

        QString totalTime;
        QString hStr = QString("0%1").arg(Sec / 3600);
        QString mStr = QString("0%1").arg(Sec / 60 % 60);
        QString sStr = QString("0%1").arg(Sec % 60);
        if (hStr == "00")
        {
            totalTime = QString("%1:%2").arg(mStr.right(2)).arg(sStr.right(2));
        }
        else
        {
            totalTime = QString("%1:%2:%3").arg(hStr).arg(mStr.right(2)).arg(sStr.right(2));
        }

        m_videoControllerWidget->setVideoSliderTotalTime(totalTime);
    });
}

///播放器状态改变的时候回调此函数
void VideoPlayerWidget::onPlayerStateChanged(const VideoPlayerState &state, const bool &hasVideo, const bool &hasAudio)
{
    FunctionTransfer::runInMainThread([=](){
        if(state == VideoPlayer_Stop){
            m_progressTimer->stop();
        }
        else if(state == VideoPlayer_Playing){
            m_progressTimer->start();
        }
        else if(state == VideoPlayer_Pause){

        }
    });
}

///显示视频数据，此函数不宜做耗时操作，否则会影响播放的流畅性。
void VideoPlayerWidget::onDisplayVideo(VideoFrame::ptr videoFrame)
{
    m_showVideoWidget->inputOneFrame(videoFrame);
}
