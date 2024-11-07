#include "video_frame.h"

VideoFrame::VideoFrame()
{
    mYuv420Buffer = nullptr;
}

VideoFrame::~VideoFrame()
{
    if (mYuv420Buffer != nullptr)
    {
        free(mYuv420Buffer);
        mYuv420Buffer = nullptr;
    }
}

void VideoFrame::initBuffer(const int &width, const int &height)
{
    if (mYuv420Buffer != nullptr)
    {
        free(mYuv420Buffer);
        mYuv420Buffer = nullptr;
    }

    mWidth  = width;
    mHeight = height;

    mYuv420Buffer = (uint8_t*)malloc(width * height * 3 / 2);

}

void VideoFrame::setYUVbuf(const uint8_t *buf)
{
    int Ysize = mWidth * mHeight;
    memcpy(mYuv420Buffer, buf, Ysize * 3 / 2);
}

void VideoFrame::setYbuf(const uint8_t *buf)
{
    int Ysize = mWidth * mHeight;
    memcpy(mYuv420Buffer, buf, Ysize);
}

void VideoFrame::setUbuf(const uint8_t *buf)
{
    int Ysize = mWidth * mHeight;
    memcpy(mYuv420Buffer + Ysize, buf, Ysize / 4);
}

void VideoFrame::setVbuf(const uint8_t *buf)
{
    int Ysize = mWidth * mHeight;
    memcpy(mYuv420Buffer + Ysize + Ysize / 4, buf, Ysize / 4);
}

QImage VideoFrame::YUV420pToQImage() {
    // RGB 缓冲区大小
    int rgbBufferSize = mWidth * mHeight * 3; // RGB24 每个像素3字节
    uint8_t *rgbBuffer = new uint8_t[rgbBufferSize]; // 为RGB数据分配内存

    // 初始化SwsContext
    SwsContext *swsCtx = sws_getContext(
        mWidth, mHeight, AV_PIX_FMT_YUV420P,  // 源格式
        mWidth, mHeight, AV_PIX_FMT_RGB24,    // 目标格式
        SWS_BILINEAR, nullptr, nullptr, nullptr
        );

    if (!swsCtx) {
        fprintf(stderr, "无法初始化 sws context\n");
        delete[] rgbBuffer;
        return QImage(); // 返回空图像表示失败
    }

//    AVFrame* pFrameYUV = av_frame_alloc();
//    av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, mYuv420Buffer, AV_PIX_FMT_YUV420P, mWidth, mHeight, 1);

    // 设置源数据和行步长
    uint8_t *srcData[AV_NUM_DATA_POINTERS] = { mYuv420Buffer, mYuv420Buffer + mWidth * mHeight, mYuv420Buffer + mWidth * mHeight * 5 / 4 };
    int srcLineSize[AV_NUM_DATA_POINTERS] = { mWidth, mWidth / 2, mWidth / 2};

    // 设置目标数据和行步长
//    AVFrame* pFrameRGB = av_frame_alloc();
//    av_image_fill_arrays(pFrameRGB->data, pFrameRGB->linesize, rgbBuffer, AV_PIX_FMT_RGB24, mWidth, mHeight, 1);

    uint8_t *dstData[AV_NUM_DATA_POINTERS] = { rgbBuffer };
    int dstLineSize[AV_NUM_DATA_POINTERS] = { mWidth * 3 };

    // 执行格式转换
    sws_scale(swsCtx, srcData, srcLineSize, 0, mHeight, dstData, dstLineSize);

    // 将转换后的RGB数据转换为QImage
    QImage image = QImage(rgbBuffer, mWidth, mHeight, QImage::Format_RGB888).copy();

    // 清理
    sws_freeContext(swsCtx);
    delete[] rgbBuffer; // 注意此时 image 应该已经拷贝数据，rgbBuffer 可删除

    // 返回 QImage
    return image;
}
