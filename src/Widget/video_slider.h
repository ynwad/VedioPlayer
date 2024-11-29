#ifndef __VIDEO_SLIDER_H__
#define __VIDEO_SLIDER_H__

#include <QSlider>

class VideoSlider : public QSlider
{
    Q_OBJECT
public:
    explicit VideoSlider(QWidget *parent = 0);

    ~VideoSlider();

    void setValue(int nValue);

signals:
    void signal_valueChanged(int nValue);

protected:
    void resizeEvent(QResizeEvent *);

    void mousePressEvent(QMouseEvent *event);

    void mouseMoveEvent(QMouseEvent *event);

    void mouseReleaseEvent(QMouseEvent *event);

    void enterEvent(QEvent *);

    void leaveEvent(QEvent *);

private:
    int m_nPosX;
};

#endif // __VIDEO_SLIDER_H__
