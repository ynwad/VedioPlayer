#include "video_slider.h"

VideoSlider::VideoSlider(QWidget *parent)
    : QSlider(parent){
    setOrientation(Qt::Horizontal);  // 设置水平方向
    setMinimum(0);
    setMaximum(100);
    setValue(50);  // 设置初始值

//    m_timer = new QTimer(this);
}

VideoSlider::~VideoSlider(){

}

void VideoSlider::setValue(int nValue){
    QSlider::setValue(nValue);
}

void VideoSlider::resizeEvent(QResizeEvent *event){
    QSlider::resizeEvent(event);
}

void VideoSlider::mousePressEvent(QMouseEvent *event){
    QSlider::mousePressEvent(event);
}

void VideoSlider::mouseMoveEvent(QMouseEvent *event){
    QSlider::mouseMoveEvent(event);
}

void VideoSlider::mouseReleaseEvent(QMouseEvent *event){
    QSlider::mouseReleaseEvent(event);
}

void VideoSlider::enterEvent(QEvent *e){
//    m_timer->stop();
    QSlider::enterEvent(e);
}

void VideoSlider::leaveEvent(QEvent *e){
//    m_timer->start();
    QSlider::leaveEvent(e);
}

