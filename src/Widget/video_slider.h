#ifndef VIDEOSLIDER_H
#define VIDEOSLIDER_H

#include <QSlider>
#include <QTimer>

class VideoSlider : public QSlider
{
    Q_OBJECT
public:
    explicit VideoSlider(QWidget *parent = 0);

    ~VideoSlider();

signals:

protected:
    void resizeEvent(QResizeEvent *);

    void mousePressEvent(QMouseEvent *event);

    void mouseMoveEvent(QMouseEvent *event);

    void mouseReleaseEvent(QMouseEvent *event);

    void enterEvent(QEvent *);

    void leaveEvent(QEvent *);

private slots:
    void onTimerTimeOut();

private:
    int m_nPosX;

    QTimer *m_timer;
};

#endif // VIDEOSLIDER_H
