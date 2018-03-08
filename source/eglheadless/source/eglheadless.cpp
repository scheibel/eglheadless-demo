
#include <eglheadless/eglheadless.h>


namespace
{


EGLDisplay eglDisplay;
EGLint eglMajor, eglMinor;


} // namespace


namespace eglheadless
{


void initialize()
{
    eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(eglDisplay, &eglMajor, &eglMinor);
}

void deinitialize()
{
    eglTerminate(eglDisplay);
}

EGLDisplay defaultDisplay()
{
    return eglDisplay;
}

PixelBuffer createPixelBuffer(const PixelBufferInfo & info)
{
    return PixelBuffer(info, info.createPixelBuffer());
}


PixelBufferInfo::PixelBufferInfo()
: m_width(0)
, m_height(0)
, m_redBits(0)
, m_greenBits(0)
, m_bluebits(0)
, m_depthBits(0)
, m_apis(0)
{
}

PixelBufferInfo::~PixelBufferInfo()
{
}

void PixelBufferInfo::setSize(EGLint width, EGLint height)
{
    m_width = width;
    m_height = height;
}

void PixelBufferInfo::setChannelDepths(EGLint redBits, EGLint greenBits, EGLint bluebits, EGLint depthBits)
{
    m_redBits = redBits;
    m_greenBits = greenBits;
    m_bluebits = bluebits;
    m_depthBits = depthBits;
}

void PixelBufferInfo::enableRenderingAPIs(EGLint renderingAPIs)
{
    m_apis = renderingAPIs;
}

EGLSurface PixelBufferInfo::createPixelBuffer() const
{
    const EGLint configAttribs[] = {
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
        EGL_BLUE_SIZE, m_bluebits,
        EGL_GREEN_SIZE, m_greenBits,
        EGL_RED_SIZE, m_redBits,
        EGL_DEPTH_SIZE, m_depthBits,
        EGL_RENDERABLE_TYPE, m_apis,
        EGL_NONE
    };

    const EGLint pbufferAttribs[] = {
        EGL_WIDTH, m_width,
        EGL_HEIGHT, m_height,
        EGL_NONE,
    };

    EGLint numConfigs;
    EGLConfig eglCfg;

    eglChooseConfig(eglDisplay, configAttribs, &eglCfg, 1, &numConfigs);

    // 3. Create a surface
    EGLSurface eglSurf = eglCreatePbufferSurface(eglDisplay, eglCfg,
                                               pbufferAttribs);

    return eglSurf;
}


PixelBuffer::PixelBuffer(const PixelBufferInfo & info, EGLSurface surfaceId)
: m_info(info)
, m_surfaceId(surfaceId)
{
}

PixelBuffer::~PixelBuffer()
{
}

EGLSurface PixelBuffer::surfaceId() const
{
    return m_surfaceId;
}


} // namespace eglheadless
