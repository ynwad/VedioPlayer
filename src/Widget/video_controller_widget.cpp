#include "video_controller_widget.h"
#include <QHBoxLayout>

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
    // 使用样式表设置背景图片
    m_btnPlayPause->setStyleSheet(
        "QPushButton {"
        "   border: none;"  // 移除边框
        "   background-image: url(:/image/resource/image/pause.png);"  // 设置背景图片
        "   background-repeat: no-repeat;"
        "   background-position: center;"  // 居中显示
        "}"
        );
    m_btnPlayPause->setFixedSize(45, 45);

    m_btnFastForward = new QPushButton();  // 快进

    m_btnFastForward->setStyleSheet(
        "QPushButton {"
        "   border: none;"  // 移除边框
        "   background-image: url(:/image/resource/image/next.png);"  // 设置背景图片
        "   background-repeat: no-repeat;"
        "   background-position: center;"  // 居中显示
        "}"
        );
    m_btnFastForward->setFixedSize(45, 45);

    M_btnFastBackward = new QPushButton(); // 快退
    M_btnFastBackward->resize(140, 140);
    M_btnFastBackward->setStyleSheet(
        "QPushButton {"
        "   border: none;"  // 移除边框
        "   background-image: url(:/image/resource/image/prior.png);"  // 设置背景图片
        "   background-repeat: no-repeat;"
        "   background-position: center;"  // 居中显示
        "}"
        );
    M_btnFastBackward->setFixedSize(45, 45);

    m_btnStop = new QPushButton();
    m_btnStop->setStyleSheet(
        "QPushButton {"
        "   border: none;"  // 移除边框
        "   background-image: url(:/image/resource/image/pause.png);"  // 设置背景图片
        "   background-repeat: no-repeat;"
        "   background-position: center;"  // 居中显示
        "}"
        );
    m_btnStop->setFixedSize(45, 45);

    initUI();
#endif
    connect(m_audioSlider, SIGNAL(valueChanged(int)), parent, SLOT(slotAudioSliderMoved(int)));
    connect(m_videoSlider, SIGNAL(valueChanged(int)), parent, SLOT(slotVideoSliderMoved(int)));
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
    hLayout->addWidget(M_btnFastBackward);
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
