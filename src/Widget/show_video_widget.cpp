#include "show_video_widget.h"
#include "spdlog/spdlog.h"
#include "Base/function_transfer.h"

#define GetChar(str) #str

#if 0
// 顶点着色器代码
const char *vsrc = GetChar(
        attribute vec4 vertexIn; \
        attribute vec2 textureIn; \
        varying vec2 textureOut;  \
        void main(void)           \
        {                         \
                gl_Position = vertexIn; \
                textureOut = textureIn; \
        }
    );

// 片段着色器代码
const char *fsrc = GetChar(
    #if defined(WIN32)
        #ifdef GL_ES
        precision mediump float;
        #endif
    #else
    #endif
        varying vec2 textureOut;
        uniform sampler2D tex_y;
        uniform sampler2D tex_u;
        uniform sampler2D tex_v;
        void main(void)
        {
                vec3 yuv;
                vec3 rgb;
                yuv.x = texture2D(tex_y, textureOut).r;
                yuv.y = texture2D(tex_u, textureOut).r - 0.5;
                yuv.z = texture2D(tex_v, textureOut).r - 0.5;
                rgb = mat3( 1,       1,         1,
                       0,       -0.39465,  2.03211,
                       1.13983, -0.58060,  0) * yuv;
                gl_FragColor = vec4(rgb, 1);
        }
    );

#endif

//顶点着色器源码
const char *vsrc = "attribute vec4 vertexIn; \
    attribute vec2 textureIn; \
    varying vec2 textureOut;  \
    void main(void)           \
{                         \
        gl_Position = vertexIn; \
        textureOut = textureIn; \
}";

const char *fsrc =
#if defined(WIN32)
    "#ifdef GL_ES\n"
    "precision mediump float;\n"
    "#endif\n"
#else
#endif
    "varying vec2 textureOut; \
    uniform sampler2D tex_y; \
    uniform sampler2D tex_u; \
    uniform sampler2D tex_v; \
    void main(void) \
{ \
        vec3 yuv; \
        vec3 rgb; \
        yuv.x = texture2D(tex_y, textureOut).r; \
        yuv.y = texture2D(tex_u, textureOut).r - 0.5; \
        yuv.z = texture2D(tex_v, textureOut).r - 0.5; \
        rgb = mat3( 1,       1,         1, \
               0,       -0.39465,  2.03211, \
               1.13983, -0.58060,  0) * yuv; \
        gl_FragColor = vec4(rgb, 1); \
}";

ShowVideoWidget::ShowVideoWidget(QWidget *parent)
    : QOpenGLWidget(parent){
    m_vertexVertices = new GLfloat[8];

    resize(1920, 1080);
}

ShowVideoWidget::~ShowVideoWidget(){

}

void ShowVideoWidget::inputOneFrame(VideoFrame::ptr videoFrame){
    FunctionTransfer::runInMainThread([=](){

        int width = videoFrame.get()->width();
        int height = videoFrame.get()->height();

        if (m_nVideoW <= 0 || m_nVideoH <= 0 || m_nVideoW != width || m_nVideoH != height)
        {
            // setVideoWidth(width, height);
            m_nVideoW = width;
            m_nVideoH = height;
        }

        // mLastGetFrameTime = QDateTime::currentMSecsSinceEpoch();

        m_videoFrame.reset();
        m_videoFrame = videoFrame;

        update(); //调用update将执行 paintEvent函数
    });
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

    // 初始化顶点着色器对象
    m_pVShader = new QOpenGLShader(QOpenGLShader::Vertex, this);
    bool bCompile = m_pVShader->compileSourceCode(vsrc);

    if(!bCompile){
        SPDLOG_ERROR("顶点着色器源代码编译失败");
    }

    m_pFShader = new QOpenGLShader(QOpenGLShader::Fragment, this);
    bCompile = m_pFShader->compileSourceCode(fsrc);

    if(!bCompile){
        SPDLOG_ERROR("片段着色器源代码编译失败");
    }

    // 创建着色器程序容器
    m_pShaderProgram = new QOpenGLShaderProgram;
    // 将着色器添加到程序容器
    m_pShaderProgram->addShader(m_pVShader);
    m_pShaderProgram->addShader(m_pFShader);
    // 绑定属性到指定位置
    m_pShaderProgram->bindAttributeLocation("verTextIn", ATTRIB_VERTEX);
    m_pShaderProgram->bindAttributeLocation("textureIn", ATTRIB_TEXTURE);
    // 链接所有的着色器程序
    m_pShaderProgram->link();
    // 激活所有链接
    m_pShaderProgram->bind();

    //读取着色器中的数据变量tex_y, tex_u, tex_v的位置,这些变量的声明可以在
    //片段着色器源码中可以看到
    textureUniformY = m_pShaderProgram->uniformLocation("tex_y");
    textureUniformU =  m_pShaderProgram->uniformLocation("tex_u");
    textureUniformV =  m_pShaderProgram->uniformLocation("tex_v");

    // 顶点矩阵
    const GLfloat vertexVertices[] = {
        -1.0f, -1.0f,
        1.0f, -1.0f,
        -1.0f, 1.0f,
        1.0f, 1.0f,
    };

    memcpy(m_vertexVertices, vertexVertices, sizeof(vertexVertices));

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
    glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, 0, 0, m_vertexVertices);
    //设置属性ATTRIB_TEXTURE的纹理矩阵值以及格式
    glVertexAttribPointer(ATTRIB_TEXTURE, 2, GL_FLOAT, 0, 0, textureVertices);
    //启用ATTRIB_VERTEX属性的数据,默认是关闭的
    glEnableVertexAttribArray(ATTRIB_VERTEX);
    //启用ATTRIB_TEXTURE属性的数据,默认是关闭的
    glEnableVertexAttribArray(ATTRIB_TEXTURE);
    //分别创建y,u,v纹理对象
    m_pTextureY = new QOpenGLTexture(QOpenGLTexture::Target2D);
    m_pTextureU = new QOpenGLTexture(QOpenGLTexture::Target2D);
    m_pTextureV = new QOpenGLTexture(QOpenGLTexture::Target2D);
    m_pTextureY->create();
    m_pTextureU->create();
    m_pTextureV->create();
    //获取返回y分量的纹理索引值
    id_y = m_pTextureY->textureId();
    //获取返回u分量的纹理索引值
    id_u = m_pTextureU->textureId();
    //获取返回v分量的纹理索引值
    id_v = m_pTextureV->textureId();
    // glClearColor(0.0,0.0,0.0,0.0);//设置背景色-黑色

}

void ShowVideoWidget::paintGL(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    VideoFrame * videoFrame = m_videoFrame.get();
    SPDLOG_INFO("绘制");
    if (videoFrame != nullptr)
    {
        uint8_t *m_pBufYuv420p = videoFrame->buffer();

        if (m_pBufYuv420p != NULL)
        {
            m_pShaderProgram->bind();

            //加载y数据纹理
            //激活纹理单元GL_TEXTURE0
            glActiveTexture(GL_TEXTURE0);
            //使用来自y数据生成纹理
            glBindTexture(GL_TEXTURE_2D, id_y);
            //使用内存中m_pBufYuv420p数据创建真正的y数据纹理
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_nVideoW, m_nVideoH, 0, GL_RED, GL_UNSIGNED_BYTE, m_pBufYuv420p);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            //加载u数据纹理
            glActiveTexture(GL_TEXTURE1);//激活纹理单元GL_TEXTURE1
            glBindTexture(GL_TEXTURE_2D, id_u);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_nVideoW/2, m_nVideoH/2, 0, GL_RED, GL_UNSIGNED_BYTE, (char*)m_pBufYuv420p+m_nVideoW*m_nVideoH);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            //加载v数据纹理
            glActiveTexture(GL_TEXTURE2);//激活纹理单元GL_TEXTURE2
            glBindTexture(GL_TEXTURE_2D, id_v);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_nVideoW/2, m_nVideoH/2, 0, GL_RED, GL_UNSIGNED_BYTE, (char*)m_pBufYuv420p+m_nVideoW*m_nVideoH*5/4);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            //指定y纹理要使用新值 只能用0,1,2等表示纹理单元的索引，这是opengl不人性化的地方
            //0对应纹理单元GL_TEXTURE0 1对应纹理单元GL_TEXTURE1 2对应纹理的单元
            glUniform1i(textureUniformY, 0);
            //指定u纹理要使用新值
            glUniform1i(textureUniformU, 1);
            //指定v纹理要使用新值
            glUniform1i(textureUniformV, 2);
            //使用顶点数组方式绘制图形
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            m_pShaderProgram->release();

        }
    }
}

void ShowVideoWidget::resizeGL(int width,int height){

    if(width == 0)// 防止被零除
    {
        width = 1;// 将高设为1
    }
    //设置视口
    glViewport(0, 0, width, height);

    // resetGLVertex(width, height);
}
