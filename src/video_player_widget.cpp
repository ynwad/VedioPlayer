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
    : DragAbleWidget(parent)
    , ui(new Ui::VideoPlayerWidget)
{
//    ui->setupUi(this);

    FunctionTransfer::init(QThread::currentThreadId());

    m_player = new VideoPlayer();
    m_player->setVideoPlayerCallBack(this);

    m_animationControlWidget = new QPropertyAnimation(m_videoControllerWidget, "geometry");

    m_showVideoWidget = new ShowVideoWidget(this);

    m_videoControllerWidget = new VideoControllerWIdget(this);
    m_videoControllerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_videoControllerWidget->setFixedHeight(40);

    initUI();

    initShortCut();
}

VideoPlayerWidget::~VideoPlayerWidget()
{
    delete ui;
}

void VideoPlayerWidget::initUI(){

    initMenuBar();

    QVBoxLayout* vLayout = new QVBoxLayout;

    vLayout->addWidget(m_menuBar);
    vLayout->addWidget(m_showVideoWidget);
//    vLayout->addWidget(m_videoControllerWidget);

    m_showVideoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->setSpacing(0);

    setLayout(vLayout);

    m_videoControllerWidget->move(0, height() - m_videoControllerWidget->height());
    resize(200, 150);
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

void VideoPlayerWidget::showOutControlWidget(){
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
    m_animationControlWidget->setTargetObject(m_videoControllerWidget);
    m_animationControlWidget->setDuration(300);

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
void VideoPlayerWidget::onDisplayVideo(VideoFrame::ptr videoFrame)
{
    m_showVideoWidget->inputOneFrame(videoFrame);
}
