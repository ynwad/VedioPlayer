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
    mHegiht = height;

    mYuv420Buffer = (uint8_t*)malloc(width * height * 3 / 2);

}

void VideoFrame::setYUVbuf(const uint8_t *buf)
{
    int Ysize = mWidth * mHegiht;
    memcpy(mYuv420Buffer, buf, Ysize * 3 / 2);
}

void VideoFrame::setYbuf(const uint8_t *buf)
{
    int Ysize = mWidth * mHegiht;
    memcpy(mYuv420Buffer, buf, Ysize);
}

void VideoFrame::setUbuf(const uint8_t *buf)
{
    int Ysize = mWidth * mHegiht;
    memcpy(mYuv420Buffer + Ysize, buf, Ysize / 4);
}

void VideoFrame::setVbuf(const uint8_t *buf)
{
    int Ysize = mWidth * mHegiht;
    memcpy(mYuv420Buffer + Ysize + Ysize / 4, buf, Ysize / 4);
}

QImage VideoFrame::YUV420pToQImage() {
//    uint8_t *yuvData = mYuv420Buffer;
    int width = mWidth;
    int height = mHegiht;
    // 创建 SwsContext 用于转换像素格式
    SwsContext *swsCtx = sws_getContext(
        width, height, AV_PIX_FMT_YUV420P,   // 输入格式
        width, height, AV_PIX_FMT_RGB24,     // 输出格式
        SWS_BILINEAR, NULL, NULL, NULL
        );

    if (!swsCtx) {
        qWarning("Failed to create SwsContext");
        return QImage();
    }

    // 分配 RGB 数据缓冲区
    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, width, height, 1);
    uint8_t *rgbData = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));
    if (!rgbData) {
        qWarning("Failed to allocate RGB buffer");
        sws_freeContext(swsCtx);
        return QImage();
    }

    // 设置源和目标数据指针
    uint8_t *srcSlice[AV_NUM_DATA_POINTERS] = { mYuv420Buffer, nullptr, nullptr, nullptr };
    int srcStride[AV_NUM_DATA_POINTERS] = { width, width / 2, width / 2, 0 };

    uint8_t *dstSlice[AV_NUM_DATA_POINTERS] = { rgbData, nullptr, nullptr, nullptr };
    int dstStride[AV_NUM_DATA_POINTERS] = { 3 * width, 0, 0, 0 };

    // 进行像素格式转换
    sws_scale(
        swsCtx,
        srcSlice,
        srcStride,
        0,
        height,
        dstSlice,
        dstStride
        );

    // 创建 QImage，使用 RGB 数据
    QImage img(rgbData, width, height, QImage::Format_RGB888);

    // 由于 QImage 不会复制数据，确保在 QImage 生命周期内数据有效
    // 可以使用 QImage::copy() 来复制数据到新的缓冲区
    QImage copiedImg = img.copy();

    // 释放资源
    av_free(rgbData);
    sws_freeContext(swsCtx);

    return copiedImg;
}
