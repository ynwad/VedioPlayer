
#include "show_video_widget_1.h"
#include <QDebug>
#include <QTimer>
#include "spdlog/spdlog.h"
#include "Base/function_transfer.h"

//自动加双引号
#define GET_STR(x) #x
#define A_VER 3
#define T_VER 4

//        FILE *fp  =NULL;

//顶点shader
const char *vString = GET_STR(
    attribute vec4 vertexIn;
    attribute vec2 textureIn;
    varying vec2 textureOut;
    void main(void){
        gl_Position = vertexIn;
        textureOut = textureIn;
    }
    );
//片元shader
const char *tString = GET_STR(
    varying vec2 textureOut;
    uniform sampler2D text_y;
    uniform sampler2D text_u;
    uniform sampler2D text_v;
    void main(void){
        vec3 yuv;
        vec3 rgb;
        yuv.x = texture2D(text_y,textureOut).r;
        yuv.y = texture2D(text_u,textureOut).r-0.5;
        yuv.z = texture2D(text_v,textureOut).r-0.5;
        rgb = mat3(1.0, 1.0, 1.0,
                   0.0, -0.39465, 2.03211,
                   1.13983, -0.58060, 0.0)*yuv;
        gl_FragColor = vec4(rgb,1.0);

    }
    );



//准备yuv数据
//ffmpeg -i v1080.mp4 -t 10 -s 240x128 -pix_fmt yuv420p  out240x128.yuv
XVideoWidget::XVideoWidget(QWidget *parent):QOpenGLWidget(parent)
{

}

void XVideoWidget::inputOneFrame(VideoFrame::ptr videoFrame){
    FunctionTransfer::runInMainThread([=](){

        int nWidth = videoFrame->width();
        int nHeight = videoFrame->height();

//        if (width <= 0 || height <= 0 || width != nWidth || height != nHeight)
//        {
            // setVideoWidth(width, height);
            width = nWidth;
            height = nHeight;
//        }

        // mLastGetFrameTime = QDateTime::currentMSecsSinceEpoch();

        m_videoFrame.reset();
        m_videoFrame = videoFrame;

        update(); //调用update将执行 paintEvent函数
    });
}

//初始化GL
void XVideoWidget::initializeGL(){
    qDebug()<<"初始化GL";
    //初始化OpenGL QOpenGLFunctions继承来的函数
    initializeOpenGLFunctions();

    //创建Program

    //program加载shader（顶点和片元脚本）
    //片元
    bool ret = mProgram.addShaderFromSourceCode(QOpenGLShader::Fragment,tString);
    if(!ret){
        qDebug()<<"片元着色器加载失败";
    }
    //顶点
    ret  = mProgram.addShaderFromSourceCode(QOpenGLShader::Vertex,vString);
    if(!ret){
        qDebug()<<"顶点着色器加载失败";
    }
    //设置顶点坐标的变量
    mProgram.bindAttributeLocation("vertexIn",A_VER);
    //设置材质坐标变量
    mProgram.bindAttributeLocation("textureIn",T_VER);

    //编译shader
    ret = mProgram.link();
    if(!ret){
        qDebug()<<"mProgram.link failed!!";
    }
    ret = mProgram.bind();
    if(!ret){
        qDebug()<<"mProgram.bind() Failed!!";
    }

    //传递顶点和材质坐标
    //顶点
    static const GLfloat ver[] = {
        -1.0f,-1.0f,
        1.0f,-1.0f,
        -1.0f, 1.0f,
        1.0f,1.0f
    };
    //材质
    static const GLfloat tex[] = {
        0.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f
    };

    //顶点
    glVertexAttribPointer(A_VER,2,GL_FLOAT,0,0,ver);
    glEnableVertexAttribArray(A_VER);
    //材质
    glVertexAttribPointer(T_VER,2,GL_FLOAT,0,0,tex);
    glEnableVertexAttribArray(T_VER);


    //从shader获取材质
    unis[0] = mProgram.uniformLocation("text_y");
    unis[1] = mProgram.uniformLocation("text_u");
    unis[2] = mProgram.uniformLocation("text_v");

    //创建材质(yuv3个)
    glGenTextures(3,texs);

    //绑定材质yuv
    //y
    glBindTexture(GL_TEXTURE_2D, texs[0]);
    //放大过滤，线性插值   GL_NEAREST(效率高，但马赛克严重)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //创建材质显卡空间
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
    
    //u
    glBindTexture(GL_TEXTURE_2D, texs[1]);
    //放大过滤，线性插值
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //创建材质显卡空间
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width/2, height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
    
    //v
    glBindTexture(GL_TEXTURE_2D, texs[2]);
    //放大过滤，线性插值
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //创建材质显卡空间
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width / 2, height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

    // ///分配材质内存空间
    // datas[0] = new unsigned char[width*height];        //Y
    // datas[1] = new unsigned char[width*height/4];    //U
    // datas[2] = new unsigned char[width*height/4];    //V

//    fp = fopen("F:/TMP/新建文件夹 (3)/output.yuv","rb");
//    if(!fp){
//        qDebug()<<"out240x128.yuv open failed!!";
//    }


//    //启动定时器
//    QTimer *timer = new QTimer(this);
//    connect(timer,&QTimer::timeout,this,[=](){update();});
//    timer->start(40);

}


//刷新显示
void XVideoWidget::paintGL(){
//    qDebug()<<"刷新显示GL";
//    if(feof(fp)){
//        fseek(fp,0,SEEK_SET);
//    }
//    fread(datas[0],1,width*height,fp);
//    fread(datas[1],1,width*height/4,fp);
//    fread(datas[2],1,width*height/4,fp);
    if(m_videoFrame == nullptr){
        return;
    }
    uint8_t *m_pBufYuv420p = m_videoFrame->buffer();
    //激活材质
    glActiveTexture(GL_TEXTURE0);
    //绑定材质到Y
    glBindTexture(GL_TEXTURE_2D,texs[0]);
    //修改材质内容，显存
    glTexSubImage2D(GL_TEXTURE_2D,0,0,0,width,height,GL_RED, GL_UNSIGNED_BYTE, m_pBufYuv420p);
    //与shader uni变量关联
    glUniform1i(unis[0], 0);

    glActiveTexture(GL_TEXTURE0+1);
    glBindTexture(GL_TEXTURE_2D, texs[1]); //1层绑定到U材质
    //修改材质内容(复制内存内容)
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width/2, height / 2, GL_RED, GL_UNSIGNED_BYTE, (char*)m_pBufYuv420p+width*height);
    //与shader uni变量关联
    glUniform1i(unis[1],1);


    glActiveTexture(GL_TEXTURE0+2);
    glBindTexture(GL_TEXTURE_2D, texs[2]); //2层绑定到V材质
    //修改材质内容(复制内存内容)
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width / 2, height / 2, GL_RED, GL_UNSIGNED_BYTE, (char*)m_pBufYuv420p+width*height*5/4);
    //与shader uni变量关联
    glUniform1i(unis[2], 2);

    //绘制
    glDrawArrays(GL_TRIANGLE_STRIP,0,4);

    qDebug()<<"paint end";
}


//窗口尺寸变化
void XVideoWidget::resizeGL(int width,int height){
    qDebug()<<"窗口尺寸变化GL";
}

XVideoWidget::~XVideoWidget(){


}
