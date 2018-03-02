
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

    EGLSurface createPixelBuffer() const;


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
    PixelBuffer(const PixelBufferInfo & info, EGLSurface surfaceId);
    ~PixelBuffer();


public:
    EGLSurface surfaceId() const;


protected:
    const PixelBufferInfo & m_info;
    EGLSurface m_surfaceId;
};


/*class EGLHEADLESS_API Context
{
public:
    Context();
    ~Context();


public:
    void makeCurrent(const PixelBuffer & readBuffer, const PixelBuffer & drawBuffer, EGLint renderingAPI);


protected:
};*/


void EGLHEADLESS_API initialize();
void EGLHEADLESS_API deinitialize();
PixelBuffer EGLHEADLESS_API createPixelBuffer(const PixelBufferInfo & info);


} // namespace eglheadless
