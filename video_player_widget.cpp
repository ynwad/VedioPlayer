#include "video_player_widget.h"
#include "ui_video_player_widget.h"

VideoPlayerWidget::VideoPlayerWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::VideoPlayerWidget)
{
    ui->setupUi(this);
}

VideoPlayerWidget::~VideoPlayerWidget()
{
    delete ui;
}

