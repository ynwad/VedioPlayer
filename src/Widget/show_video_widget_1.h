#ifndef XVIDEOWIDGET_H
#define XVIDEOWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>

#include "VideoPlayer/Video/video_frame.h"

class XVideoWidget:public QOpenGLWidget,protected QOpenGLFunctions
{
    Q_OBJECT

public:
    XVideoWidget(QWidget *parent = nullptr);
    ~XVideoWidget();

    void inputOneFrame(VideoFrame::ptr videoFrame);

protected:
    //刷新显示
    void paintGL() override;
    //初始化GL
    void initializeGL() override;
    //窗口尺寸变化
    void resizeGL(int width,int height) override;

private:
    //shader程序
    QOpenGLShaderProgram mProgram;
    //shader中yuv变量地址
    GLuint unis[3] = {0};
    //opengl的texture地址
    GLuint texs[3] = {0};

    //材质内存空间
    unsigned char *datas[3] = {0};


    int width{320};
    int height{240};

    VideoFrame::ptr m_videoFrame;
};

#endif // XVIDEOWIDGET_H
