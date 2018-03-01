
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


} // namespace eglheadless
