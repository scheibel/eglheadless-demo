
#include <gmock/gmock.h>

#include <eglheadless/eglheadless.h>


class eglheadless_test: public testing::Test
{
public:
};


TEST_F(eglheadless_test, NativeCheck)
{
    static const EGLint configAttribs[] = {
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_DEPTH_SIZE, 8,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
        EGL_NONE
    };

    static const EGLint pbufferAttribs[] = {
        EGL_WIDTH, 1920,
        EGL_HEIGHT, 1080,
        EGL_NONE,
    };

    EGLDisplay eglDpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    EGLint vmajor, vminor;

    eglInitialize(eglDpy, &vmajor, &vminor);

    // 2. Select an appropriate configuration
    EGLint numConfigs;
    EGLConfig eglCfg;

    eglChooseConfig(eglDpy, configAttribs, &eglCfg, 1, &numConfigs);

    // 3. Create a surface
    EGLSurface eglSurf = eglCreatePbufferSurface(eglDpy, eglCfg,
                                               pbufferAttribs);

    ASSERT_NE(nullptr, eglSurf);

    eglTerminate(eglDpy);
}

TEST_F(eglheadless_test, AbstractionCheck)
{
    eglheadless::initialize();

    eglheadless::PixelBufferInfo pBufferInfo;
    pBufferInfo.setSize(1920, 1080);
    pBufferInfo.setChannelDepths(8, 8, 8, 8);
    pBufferInfo.enableRenderingAPIs(EGL_OPENGL_BIT);

    eglheadless::PixelBuffer pixelBuffer = eglheadless::createPixelBuffer(pBufferInfo);

    ASSERT_NE(nullptr, pixelBuffer.surfaceId());

    eglheadless::deinitialize();
}
