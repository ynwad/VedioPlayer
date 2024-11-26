#include "video_controller_widget.h"
#include <QHBoxLayout>

VideoControllerWidget::VideoControllerWIdget(QWidget *parent)
    : QWidget(parent){
//    setStyleSheet("background-color: black;");
    setAttribute(Qt::WA_TranslucentBackground);  // 设置背景透明
//    setAttribute(Qt::WA_StyledBackground, true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

#if 1
    m_videoSlider = new VideoSlider();
    m_audioSlider = new QSlider();
    m_audioSlider->setSliderPosition(Qt::Vertical);
    m_audioSlider->setMinimum(0);
    m_audioSlider->setMaximum(100);

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
        "   background-image: url(:/image/resource/image/video.png);"  // 设置背景图片
        "   background-repeat: no-repeat;"
        "   background-position: center;"  // 居中显示
        "}"
        );
    m_btnStop->setStyleSheet("QPushButton { margin: 0; padding: 0; }"); // 清除边距和内边距
    m_btnStop->setFixedSize(45, 45);

    initUI();
#endif
}

VideoControllerWIdget::~VideoControllerWIdget(){

}

void VideoControllerWIdget::initUI(){
    QHBoxLayout* mainLayout = new QHBoxLayout();

    mainLayout->addWidget(m_btnStop);
    mainLayout->addWidget(M_btnFastBackward);
    mainLayout->addWidget(m_btnPlayPause);
    mainLayout->addWidget(m_btnFastForward);
    mainLayout->addWidget(m_videoSlider);
    mainLayout->addWidget(m_audioSlider);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    setLayout(mainLayout);
}
