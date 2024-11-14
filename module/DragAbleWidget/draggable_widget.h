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
enum Direction { UP=0, DOWN, LEFT, RIGHT, LEFTTOP, LEFTBOTTOM, RIGHTBOTTOM, RIGHTTOP, NONE };


class DragAbleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DragAbleWidget(QWidget *parent = 0);

    ~DragAbleWidget();

    QWidget *getContainWidget();

protected:
    virtual void dragEnterEvent(QDragEnterEvent *event) override;

    virtual void dropEvent(QDropEvent *event) override;
};

#endif // DRAGABLEWIDGET_H
