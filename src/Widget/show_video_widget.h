#ifndef __SHOW_VIDEO_WIDGET_H__
#define __SHOW_VIDEO_WIDGET_H__

#include <QWidget>
#include <QPaintEvent>
#include <QResizeEvent>

#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QFile>

#include "VideoPlayer/Video/video_frame.h"

struct FaceInfoNode
{
    QRect faceRect;
};

#define ATTRIB_VERTEX 3
#define ATTRIB_TEXTURE 4

///显示视频用的widget（使用OPENGL绘制YUV420P数据）
///这个仅仅是显示视频画面的控件

class ShowVideoWidget : public QOpenGLWidget,protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit ShowVideoWidget(QWidget *parent = 0);
    ~ShowVideoWidget();

    void inputOneFrame(VideoFrame::ptr videoFrame);

    void setVideoWidth(int nWidth, int nHeight);

protected:
    //刷新显示
    virtual void paintGL() override;
    //初始化GL
    virtual void initializeGL() override;
    //窗口尺寸变化
    virtual void resizeGL(int width,int height) override;
private:
    void resetTexturePara();

private:
    //shader中yuv变量地址
    GLuint unis[3] = {0};

    //opengl的texture地址
    GLuint texs[3] = {0};

    // shader程序
    QOpenGLShaderProgram *m_pShaderProgram;

//    GLfloat *m_vertexVertices; // 顶点矩阵

    int m_nVideoW{1}; //视频分辨率宽
    int m_nVideoH{1}; //视频分辨率高

    VideoFrame::ptr m_videoFrame;

    ///OpenGL用于绘制矩形
//    bool mIsShowFaceRect;
//    GLuint m_posAttr;
//    GLuint m_colAttr;
//    QOpenGLShaderProgram *m_program;

};

#endif // __SHOW_VIDEO_WIDGET_H__
