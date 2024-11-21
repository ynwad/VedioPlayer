/**
 * 叶海辉
 * QQ群121376426
 * http://blog.yundiantech.com/
 */

#ifndef DRAGABLEWIDGET_H
#define DRAGABLEWIDGET_H

#include <QWidget>
#include <QTimer>

//鼠标实现改变窗口大小
#define PADDING 6

class DragAbleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DragAbleWidget(QWidget *parent = 0);

    ~DragAbleWidget();

    QWidget *getContainWidget();

    void toggleFullScreen();

signals:
    void fileEntered(QString strFilePath);

protected:
    virtual void dragEnterEvent(QDragEnterEvent *event) override;

    virtual void dropEvent(QDropEvent *event) override;

    virtual void mousePressEvent(QMouseEvent *event) override;

protected:
    QString m_strLastEnteredFilePath;

    QSize m_originalSize;  // 用于保存窗口原始尺寸
};

#endif // DRAGABLEWIDGET_H
