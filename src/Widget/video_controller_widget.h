#ifndef VIDEOCONTROLLERWIDGET_H
#define VIDEOCONTROLLERWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QSlider>
#include "video_slider.h"

class VideoControllerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VideoControllerWidget(QWidget *parent = nullptr);

    ~VideoControllerWidget();

    void initUI();

signals:

private:
    VideoSlider *m_videoSlider;
    QSlider *m_audioSlider;

    QPushButton *m_btnPlayPause;
    QPushButton *m_btnFastForward;  // 快进
    QPushButton *M_btnFastBackward; // 快退
    QPushButton *m_btnStop;
};

#endif // VIDEOCONTROLLERWIDGET_H
