#ifndef VIDEOPLAYERWIDGET_H
#define VIDEOPLAYERWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class VideoPlayerWidget; }
QT_END_NAMESPACE

class VideoPlayerWidget : public QWidget
{
    Q_OBJECT

public:
    VideoPlayerWidget(QWidget *parent = nullptr);
    ~VideoPlayerWidget();

private:
    Ui::VideoPlayerWidget *ui;
};
#endif // VIDEOPLAYERWIDGET_H
