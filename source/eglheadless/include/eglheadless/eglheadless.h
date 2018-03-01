
#pragma once


#include <EGL/egl.h>

#include <eglheadless/eglheadless_api.h>


namespace eglheadless
{


class EGLHEADLESS_API PixelBufferInfo
{
public:
    PixelBufferInfo();
    ~PixelBufferInfo();


public:
    void setSize(EGLint width, EGLint height);
    void setChannelDepths(EGLint redBits, EGLint greenBits, EGLint bluebits, EGLint depthBits);
    void enableRenderingAPIs(EGLint renderingAPIs);


protected:
    EGLint m_width;
    EGLint m_height;
    EGLint m_redBits;
    EGLint m_greenBits;
    EGLint m_bluebits;
    EGLint m_depthBits;
    EGLint m_apis;
};


class EGLHEADLESS_API PixelBuffer
{
public:
    PixelBuffer();
    ~PixelBuffer();


protected:
    PixelBufferInfo * m_info;
    EGLint m_surfaceId;
};


class EGLHEADLESS_API Context
{
public:
    Context();
    ~Context();


protected:
    PixelBuffer * m_buffer;
};


void initialize();
void deinitialize();
PixelBuffer createPixelBuffer(const PixelBufferInfo & info);


} // namespace eglheadless
