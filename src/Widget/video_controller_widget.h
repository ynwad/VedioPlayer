#ifndef VIDEOCONTROLLERWIDGET_H
#define VIDEOCONTROLLERWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include "video_slider.h"

class VideoControllerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VideoControllerWidget(QWidget *parent = nullptr);

    ~VideoControllerWidget();

    void initUI();

    void setVideoSliderRange(int nMinVal, int nMaxVal);

    void setVideoSliderValue(int nCurVal);

    void setVideoSliderTotalTime(QString strTotalTime);

    void setVideoSliderCurTime(QString strCurTime);

    // 设置为播放状态
    void setPlayStatus();

    // 设置为暂停状态
    void setPauseStatus();

public slots:
    void slot_btnPlayPauseClicked(bool bChecked);

protected:
    virtual void mouseMoveEvent(QMouseEvent *event) override;

    virtual void enterEvent(QEvent *event) override;

    virtual void leaveEvent(QEvent *event) override;
signals:
//    void notifyMouseEnter();

//    void notifyMouseLeave();
private:
    VideoSlider *m_videoSlider;
    QSlider *m_audioSlider;
    QLabel* m_sliderTotalLable;
    QLabel* m_sliderCurTimeLable;

    QPushButton *m_btnPlayPause;
    QPushButton *m_btnFastForward;  // 快进
    QPushButton *M_btnFastBackward; // 快退
    QPushButton *m_btnStop;
};

#endif // VIDEOCONTROLLERWIDGET_H
