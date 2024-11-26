#include "video_slider.h"

VideoSlider::VideoSlider(QWidget *parent)
    : QSlider(parent){
    setOrientation(Qt::Horizontal);  // 设置水平方向
    setMinimum(0);
    setMaximum(100);
    setValue(50);  // 设置初始值

    m_timer = new QTimer(this);
}

VideoSlider::~VideoSlider(){

}

void VideoSlider::resizeEvent(QResizeEvent *){

}

void VideoSlider::mousePressEvent(QMouseEvent *event){
    QSlider::mousePressEvent(event);
}

void VideoSlider::mouseMoveEvent(QMouseEvent *event){

}

void VideoSlider::mouseReleaseEvent(QMouseEvent *event){

}

void VideoSlider::enterEvent(QEvent *){
    m_timer->stop();
}

void VideoSlider::leaveEvent(QEvent *){
    m_timer->start();
}

void VideoSlider::onTimerTimeOut(){

}
