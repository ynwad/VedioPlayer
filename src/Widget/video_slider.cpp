#include "video_slider.h"
#include <QMouseEvent>

VideoSlider::VideoSlider(QWidget *parent)
    : QSlider(parent){
    setOrientation(Qt::Horizontal);  // 设置水平方向
    setMinimum(0);
    setMaximum(0);
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
    if (event->button() == Qt::LeftButton) {
        // 计算鼠标点击位置占滑块总宽度的比例
        double ratio = static_cast<double>(event->pos().x()) / width();
        if (orientation() == Qt::Vertical) {
            ratio = static_cast<double>(height() - event->pos().y()) / height();
        }

        // 根据比例计算新值
        int newValue = minimum() + ratio * (maximum() - minimum());
        setValue(newValue); // 设置滑块的新值

        // 可选：发出 sliderMoved 信号
        emit signal_valueChanged(newValue);

        event->accept(); // 标记事件已处理
    } else {
        QSlider::mousePressEvent(event); // 默认处理其他鼠标事件
    }
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

