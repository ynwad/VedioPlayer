#include "draggable_widget.h"
#include <QDragEnterEvent>
#include <QMimeData>
#include "spdlog/spdlog.h"

DragAbleWidget::DragAbleWidget(QWidget *parent)
    : QWidget(parent){
    // 设置窗口接受拖放
    setAcceptDrops(true);
}

DragAbleWidget::~DragAbleWidget(){

}

void DragAbleWidget::toggleFullScreen(){
    if (isFullScreen()) {
        // 退出全屏，恢复尺寸
        showNormal();
        resize(m_originalSize);  // 恢复到原始尺寸
    } else {
        // 记录当前尺寸并切换到全屏模式
        m_originalSize = size();
        showFullScreen();
    }
}

void DragAbleWidget::dragEnterEvent(QDragEnterEvent *event){
    //如果为文件，则支持拖放
    if (event->mimeData()->hasFormat("text/uri-list"))
        event->acceptProposedAction();
}

void DragAbleWidget::dropEvent(QDropEvent *event){
    //注意：这里如果有多文件存在，意思是用户一下子拖动了多个文件，而不是拖动一个目录
    //如果想读取整个目录，则在不同的操作平台下，自己编写函数实现读取整个目录文件名
    QList<QUrl> urls = event->mimeData()->urls();
    if(urls.isEmpty())
        return;

    //往文本框中追加文件名
    foreach(QUrl url, urls) {
        QString strFilePath = url.toLocalFile();
        emit fileEntered(strFilePath);
    }
}

void DragAbleWidget::mousePressEvent(QMouseEvent *event){
    if (event->type() == QEvent::MouseButtonDblClick){
        if (event->button() == Qt::LeftButton){{
                toggleFullScreen();
            }
        }
    }
}
