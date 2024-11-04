#include "video_player_widget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    VideoPlayerWidget w;
    w.show();
    return a.exec();
}
