#include "video_controller_widget.h"
#include <QHBoxLayout>
#include <QVariant>
#include "video_player_widget.h"
#include "spdlog/spdlog.h"

#define PlayStatus "playStatus"
#define Play "play"
#define Pause "pause"

VideoControllerWidget::VideoControllerWidget(QWidget *parent)
    : QWidget(parent){
//    setStyleSheet("background-color: blue;");
    setAttribute(Qt::WA_TranslucentBackground);  // 设置背景透明
//    setAttribute(Qt::WA_StyledBackground, true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

#if 1
    m_videoSlider = new VideoSlider();
    m_audioSlider = new QSlider();
    m_audioSlider->setSliderPosition(Qt::Vertical);
    m_audioSlider->setMinimum(0);
    m_audioSlider->setMaximum(100);

    m_sliderTotalLable = new QLabel("00:00");
    m_sliderTotalLable->setMinimumWidth(50);
    m_sliderTotalLable->setStyleSheet("color: rgb(214, 214, 214);");

    m_sliderCurTimeLable = new QLabel("00:00");
    m_sliderCurTimeLable->setMinimumWidth(50);
    m_sliderCurTimeLable->setStyleSheet("color: rgb(214, 214, 214);");

    m_btnPlayPause = new QPushButton();
    QIcon pauseIcon(":/image/resource/image/pause.png");
    m_btnPlayPause->setIcon(pauseIcon);
    m_btnPlayPause->setIconSize(QSize(40, 40));  // 设置图片大小
    m_btnPlayPause->setProperty(PlayStatus, Pause);
    // 使用样式表设置背景图片
    m_btnPlayPause->setStyleSheet(
        "QPushButton {"
        "   border: none;"  // 移除边框
        "}"
        );
    m_btnPlayPause->setFixedSize(45, 45);

    m_btnFastForward = new QPushButton();  // 快进
    QIcon fastForwardIcon(":/image/resource/image/fast_forward.png");
    m_btnFastForward->setIcon(fastForwardIcon);
    m_btnFastForward->setIconSize(QSize(40, 40));  // 设置图片大小
    m_btnFastForward->setStyleSheet(
        "QPushButton {"
        "   border: none;"  // 移除边框
        "}"
        );
    m_btnFastForward->setFixedSize(45, 45);

    m_btnFastBackward = new QPushButton(); // 快退
    m_btnFastBackward->resize(140, 140);
    m_btnFastBackward->setStyleSheet(
        "QPushButton {"
        "   border: none;"  // 移除边框
        "}"
        );
    QIcon fastBackwardIcon(":/image/resource/image/fast_backward.png");
    m_btnFastBackward->setIcon(fastBackwardIcon);
    m_btnFastBackward->setIconSize(QSize(40, 40));  // 设置图片大小
    m_btnFastBackward->setFixedSize(45, 45);

    m_btnStop = new QPushButton();
    QIcon stopIcon(":/image/resource/image/stop.png");
    m_btnStop->setIcon(stopIcon);
    m_btnStop->setIconSize(QSize(40, 40));  // 设置图片大小
    m_btnStop->setStyleSheet("QPushButton {"
        "   border: none;"  // 移除边框
        "}"
        );

    m_btnStop->setFixedSize(45, 45);

    initUI();
#endif
    connect(m_audioSlider, SIGNAL(valueChanged(int)), parent, SLOT(slotAudioSliderMoved(int)));
    connect(m_videoSlider, SIGNAL(signal_valueChanged(int)), parent, SLOT(slotVideoSliderMoved(int)));

    connect(m_btnPlayPause, &QPushButton::clicked, this, &VideoControllerWidget::slot_btnPlayPauseClicked);
    connect(m_btnFastForward, SIGNAL(clicked(bool)), parent, SLOT(slotFastForward(bool)));
    connect(m_btnFastBackward, SIGNAL(clicked(bool)), parent, SLOT(slotFastBackward(bool)));
    connect(m_btnStop, SIGNAL(clicked(bool)), parent, SLOT(slotStop(bool)));
}

VideoControllerWidget::~VideoControllerWidget(){

}

void VideoControllerWidget::initUI(){
    QHBoxLayout* sliderLayout = new QHBoxLayout();
    sliderLayout->addWidget(m_videoSlider);
    //    QLabel* m_sliderTotalLable;
    //    QLabel* m_sliderCurTimeLable;
    sliderLayout->addWidget(m_sliderCurTimeLable);
    QLabel* labelSeparator = new QLabel("/");
    labelSeparator->setStyleSheet("color: rgb(214, 214, 214);");
    sliderLayout->addWidget(labelSeparator);
    sliderLayout->addWidget(m_sliderTotalLable);

    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->addWidget(m_btnStop);
    hLayout->addWidget(m_btnFastBackward);
    hLayout->addWidget(m_btnPlayPause);
    hLayout->addWidget(m_btnFastForward);
    hLayout->addStretch();
    hLayout->addWidget(m_audioSlider);
    hLayout->setContentsMargins(40, 0, 40, 0);
    hLayout->setSpacing(40);

    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->addLayout(sliderLayout);
    mainLayout->addLayout(hLayout);

    setLayout(mainLayout);
}

void VideoControllerWidget::resetUI(){
    resetButtons();
    resetSlider();
}

void VideoControllerWidget::resetButtons(){
    QIcon pauseIcon(":/image/resource/image/pause.png");
    m_btnPlayPause->setIcon(pauseIcon);
    m_btnPlayPause->setIconSize(QSize(40, 40));  // 设置图片大小
    m_btnPlayPause->setProperty(PlayStatus, Pause);
    // 使用样式表设置背景图片
    m_btnPlayPause->setStyleSheet(
        "QPushButton {"
        "   border: none;"  // 移除边框
        "}"
        );
}

void VideoControllerWidget::resetSlider(){
    m_videoSlider->setRange(0, 0);
    setVideoSliderCurTime("00:00");
    setVideoSliderTotalTime("00:00");
}

void VideoControllerWidget::setVideoSliderRange(int nMinVal, int nMaxVal){
    m_videoSlider->setRange(nMinVal, nMaxVal);
}

void VideoControllerWidget::setVideoSliderValue(int nCurVal){
    m_videoSlider->setValue(nCurVal);
}

void VideoControllerWidget::setVideoSliderTotalTime(QString strTotalTime){
    m_sliderTotalLable->setText(strTotalTime);
}

void VideoControllerWidget::setVideoSliderCurTime(QString strCurTime){
    m_sliderCurTimeLable->setText(strCurTime);
}

void VideoControllerWidget::setPlayStatus(){
    QIcon pauseIcon(":/image/resource/image/playing.png");
    m_btnPlayPause->setIcon(pauseIcon);
    m_btnPlayPause->setIconSize(QSize(40, 40));  // 设置图片大小
    m_btnPlayPause->setProperty(PlayStatus, Play);
}

void VideoControllerWidget::setPauseStatus(){
    QIcon pauseIcon(":/image/resource/image/pause.png");
    m_btnPlayPause->setIcon(pauseIcon);
    m_btnPlayPause->setIconSize(QSize(40, 40));  // 设置图片大小
    m_btnPlayPause->setProperty(PlayStatus, Pause);
}

void VideoControllerWidget::slot_btnPlayPauseClicked(bool bChecked){
    QString strCurStatus = m_btnPlayPause->property(PlayStatus).toString();

    if(strCurStatus == Play){
        QMetaObject::invokeMethod(parent(), "slotPause");
    }
    else if(strCurStatus == Pause){
        QMetaObject::invokeMethod(parent(), "slotPlay");
    }
}

void VideoControllerWidget::mouseMoveEvent(QMouseEvent *event){
    QWidget::mouseMoveEvent(event);
}

void VideoControllerWidget::enterEvent(QEvent *event){
//    emit notifyMouseEnter();
    QWidget::enterEvent(event);
}

void VideoControllerWidget::leaveEvent(QEvent *event){
//    emit notifyMouseLeave();
    QWidget::leaveEvent(event);
}
