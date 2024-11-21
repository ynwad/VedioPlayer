#include "video_player_widget.h"

#include <QMessageBox>
#include <QTimer>
#include <QVBoxLayout>
#include <QShortcut>
#include <QMenuBar>
#include <QFileDialog>

#include "Base/function_transfer.h"
#include "ui_video_player_widget.h"

#define _ST(str) QString::fromLocal8Bit(str)

VideoPlayerWidget::VideoPlayerWidget(QWidget *parent)
    : DragAbleWidget(parent)
    , ui(new Ui::VideoPlayerWidget)
{
//    ui->setupUi(this);

    FunctionTransfer::init(QThread::currentThreadId());

    m_player = new VideoPlayer();
    m_player->setVideoPlayerCallBack(this);

    m_showVideoWidget = new ShowVideoWidget(this);

    initUI();

    initShortCut();
}

void VideoPlayerWidget::initUI(){

    initMenuBar();

    QVBoxLayout* vLayout = new QVBoxLayout;

    m_showVideoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->setSpacing(0);

    vLayout->addWidget(m_menuBar);
    vLayout->addWidget(m_showVideoWidget);

    setLayout(vLayout);

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
    menuMedia->addAction(_ST("打开网络串流"));
//    menuMedia->addAction(_ST(""));
}

VideoPlayerWidget::~VideoPlayerWidget()
{
    delete ui;
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
