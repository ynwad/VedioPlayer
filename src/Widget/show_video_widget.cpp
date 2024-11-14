#include "show_video_widget.h"
#include "spdlog/spdlog.h"
#include "Base/function_transfer.h"

#define GET_STR(x) #x

///用于绘制矩形
static const char *vertexShaderSource =
    "attribute highp vec4 posAttr;\n"
    "attribute lowp vec4 colAttr;\n"
    "varying lowp vec4 col;\n"
    "uniform highp mat4 matrix;\n"
    "void main() {\n"
    "   col = colAttr;\n"
    "   gl_Position = posAttr;\n"
    "}\n";

static const char *fragmentShaderSource =
    "varying lowp vec4 col;\n"
    "void main() {\n"
    "   gl_FragColor = col;\n"
    "}\n";

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
    uniform sampler2D tex_y;
    uniform sampler2D tex_u;
    uniform sampler2D tex_v;
    void main(void){
        vec3 yuv;
        vec3 rgb;
        yuv.x = texture2D(tex_y,textureOut).r;
        yuv.y = texture2D(tex_u,textureOut).r-0.5;
        yuv.z = texture2D(tex_v,textureOut).r-0.5;
        rgb = mat3(1.0, 1.0, 1.0,
                   0.0, -0.39465, 2.03211,
                   1.13983, -0.58060, 0.0)*yuv;
        gl_FragColor = vec4(rgb,1.0);

    }
    );


ShowVideoWidget::ShowVideoWidget(QWidget *parent)
    : QOpenGLWidget(parent){

}

ShowVideoWidget::~ShowVideoWidget(){

}

void ShowVideoWidget::setVideoWidth(int nWidth, int nHeight){
    if(nWidth == m_nVideoW && nHeight == m_nVideoH){
        return;
    }
    m_nVideoW = nWidth;
    m_nVideoH = nHeight;

    resetTexturePara();
}

void ShowVideoWidget::inputOneFrame(VideoFrame::ptr videoFrame){
    FunctionTransfer::runInMainThread([=](){

        int width = videoFrame->width();
        int height = videoFrame->height();

        m_videoFrame.reset();
        m_videoFrame = videoFrame;

        setVideoWidth(width, height);

        update(); //调用update将执行 paintEvent函数
    });
}


void ShowVideoWidget::resetTexturePara(){
    // 在程序结束或纹理不再使用时释放显存
    // 当调用 glTexImage2D 重新设置纹理大小或格式时，OpenGL 会
    // 自动释放之前分配的纹理空间，无需手动干预
    // glDeleteTextures(1, texs);
    // glDeleteTextures(1, texs+1);
    // glDeleteTextures(1, texs+2);

    glBindTexture(GL_TEXTURE_2D, texs[0]);
    //放大过滤，线性插值   GL_NEAREST(效率高，但马赛克严重)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //创建材质显卡空间
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_nVideoW, m_nVideoH, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

    //u
    glBindTexture(GL_TEXTURE_2D, texs[1]);
    //放大过滤，线性插值
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //创建材质显卡空间
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_nVideoW/2, m_nVideoH / 2, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

    //v
    glBindTexture(GL_TEXTURE_2D, texs[2]);
    //放大过滤，线性插值
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //创建材质显卡空间
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_nVideoW / 2, m_nVideoH / 2, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
}

void ShowVideoWidget::initializeGL(){
    initializeOpenGLFunctions();

    const GLubyte* vendor = glGetString(GL_VENDOR); // 返回实现当前OpenGL的厂商
    const GLubyte* renderer = glGetString(GL_RENDERER); // 返回一个渲染器标识符，通常是个硬件平台
    const GLubyte* version = glGetString(GL_VERSION); // 返回当前实现的OpenGL版本号

    SPDLOG_INFO("OpenGL info, vendor: {}, renderer: {}, version: {}",
                reinterpret_cast<const char*>(vendor),
                reinterpret_cast<const char*>(renderer),
                reinterpret_cast<const char*>(version));

    glEnable(GL_DEPTH_TEST);

    // 创建着色器程序容器
    m_pShaderProgram = new QOpenGLShaderProgram(this);

    //program加载shader（顶点和片元脚本）
    //片元
    bool ret = m_pShaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, tString);
    if(!ret){
        SPDLOG_ERROR("片段着色器源代码编译失败");
    }
    //顶点
    ret  = m_pShaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vString);
    if(!ret){
        SPDLOG_ERROR("顶点着色器源代码编译失败");
    }

    ///用于绘制矩形
//    m_program = new QOpenGLShaderProgram(this);
//    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
//    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
//    m_program->link();
//    m_posAttr = m_program->attributeLocation("posAttr");
//    m_colAttr = m_program->attributeLocation("colAttr");

    // 设置顶点坐标的变量
    m_pShaderProgram->bindAttributeLocation("vertexIn", ATTRIB_VERTEX);
    // 设置材质坐标变量
    m_pShaderProgram->bindAttributeLocation("textureIn", ATTRIB_TEXTURE);
    // 链接所有的着色器程序
    m_pShaderProgram->link();
    if(!ret){
        SPDLOG_ERROR("mProgram.link failed!!");
    }
    // 激活所有链接
    m_pShaderProgram->bind();
    if(!ret){
        SPDLOG_ERROR("mProgram.link failed!!");
    }

    // 顶点矩阵
    static const GLfloat vertexVertices[] = {
        -1.0f, -1.0f,
        1.0f, -1.0f,
        -1.0f, 1.0f,
        1.0f, 1.0f,
    };

//    memcpy(m_vertexVertices, vertexVertices, sizeof(vertexVertices));

    //纹理矩阵
    static const GLfloat textureVertices[] = {
        0.0f,  1.0f,
        1.0f,  1.0f,
        0.0f,  0.0f,
        1.0f,  0.0f,
    };

    ///设置读取的YUV数据为1字节对其，默认4字节对齐，
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    //设置属性ATTRIB_VERTEX的顶点矩阵值以及格式
    glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, 0, 0, vertexVertices);
    //启用ATTRIB_VERTEX属性的数据,默认是关闭的
    glEnableVertexAttribArray(ATTRIB_VERTEX);

    glVertexAttribPointer(ATTRIB_TEXTURE, 2, GL_FLOAT, 0, 0, textureVertices);
    glEnableVertexAttribArray(ATTRIB_TEXTURE);


    //读取着色器中的数据变量tex_y, tex_u, tex_v的位置,这些变量的声明可以在
    //片段着色器源码中可以看到
    unis[0] = m_pShaderProgram->uniformLocation("tex_y");
    unis[1] = m_pShaderProgram->uniformLocation("tex_u");
    unis[2] = m_pShaderProgram->uniformLocation("tex_v");

    //创建材质(yuv3个)
    glGenTextures(3, texs);
}

void ShowVideoWidget::paintGL(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//    m_program->bind();

//    glEnableVertexAttribArray(0);
//    glEnableVertexAttribArray(1);

//    glDisableVertexAttribArray(1);
//    glDisableVertexAttribArray(0);

//    m_program->release();

    if (m_videoFrame != nullptr)
    {
        uint8_t *m_pBufYuv420p = m_videoFrame->buffer();

        if (m_pBufYuv420p != NULL)
        {
            m_pShaderProgram->bind();

            //加载y数据纹理
            //激活纹理单元GL_TEXTURE0
            glActiveTexture(GL_TEXTURE0);
            //使用来自y数据生成纹理
            glBindTexture(GL_TEXTURE_2D, texs[0]);
            //使用内存中m_pBufYuv420p数据创建真正的y数据纹理
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_nVideoW, m_nVideoH, GL_RED, GL_UNSIGNED_BYTE, m_pBufYuv420p);

            //加载u数据纹理
            glActiveTexture(GL_TEXTURE1);//激活纹理单元GL_TEXTURE1
            glBindTexture(GL_TEXTURE_2D, texs[1]);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_nVideoW/2, m_nVideoH/2, GL_RED, GL_UNSIGNED_BYTE, (char*)m_pBufYuv420p+m_nVideoW*m_nVideoH);

            //加载v数据纹理
            glActiveTexture(GL_TEXTURE2);//激活纹理单元GL_TEXTURE2
            glBindTexture(GL_TEXTURE_2D, texs[2]);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_nVideoW/2, m_nVideoH/2, GL_RED, GL_UNSIGNED_BYTE, (char*)m_pBufYuv420p+m_nVideoW*m_nVideoH*5/4);

            //指定y纹理要使用新值 只能用0,1,2等表示纹理单元的索引，这是opengl不人性化的地方
            //0对应纹理单元GL_TEXTURE0 1对应纹理单元GL_TEXTURE1 2对应纹理的单元
            glUniform1i(unis[0], 0);
            //指定u纹理要使用新值
            glUniform1i(unis[1], 1);
            //指定v纹理要使用新值
            glUniform1i(unis[2], 2);

            //使用顶点数组方式绘制图形
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            m_pShaderProgram->release();
        }
    }
}


void ShowVideoWidget::resizeGL(int width,int height){

//    if(width == 0)// 防止被零除
//    {
//        width = 1;// 将高设为1
//    }
//    //设置视口
//    glViewport(0, 0, 500, 340);

    // resetGLVertex(width, height);
}
