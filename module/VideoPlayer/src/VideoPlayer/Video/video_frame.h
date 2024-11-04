﻿#ifndef VIDEOFRAME_H
#define VIDEOFRAME_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <memory>

class VideoFrame
{
public:
    typedef std::shared_ptr<VideoFrame> ptr;

    VideoFrame();
    ~VideoFrame();

    void initBuffer(const int &width, const int &height);

    void setYUVbuf(const uint8_t *buf);
    void setYbuf(const uint8_t *buf);
    void setUbuf(const uint8_t *buf);
    void setVbuf(const uint8_t *buf);

    uint8_t * buffer(){return mYuv420Buffer;}
    int width(){return mWidth;}
    int height(){return mHegiht;}

protected:
    uint8_t *mYuv420Buffer;

    int mWidth;
    int mHegiht;
};

#endif // VIDEOFRAME_H