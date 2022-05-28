
#include <iostream>
#include <fstream>

#include <glbinding/glbinding.h>
#include <glbinding/Version.h>
#include <glbinding/gl32/gl.h>

#include <glbinding-aux/ContextInfo.h>
#include <glbinding-aux/types_to_string.h>

#include <eglbinding/eglbinding.h>
#include <eglbinding/egl/egl.h>

#include "getProcAddress.h"


using namespace egl;


namespace
{


const auto useOpenGL = false;


}


int main(int argc, char* argv[]) {
    static const EGLint configAttribs[] = {
        static_cast<EGLint>(EGL_SURFACE_TYPE), static_cast<EGLint>(EGL_PBUFFER_BIT),
        static_cast<EGLint>(EGL_BLUE_SIZE), 8,
        static_cast<EGLint>(EGL_GREEN_SIZE), 8,
        static_cast<EGLint>(EGL_RED_SIZE), 8,
        static_cast<EGLint>(EGL_RENDERABLE_TYPE), static_cast<EGLint>(EGL_OPENGL_BIT | EGL_OPENGL_ES3_BIT | EGL_OPENGL_ES2_BIT),
        static_cast<EGLint>(EGL_NONE)
    };

    static const EGLint pbufferAttribs[] = {
        static_cast<EGLint>(EGL_WIDTH), 1920,
        static_cast<EGLint>(EGL_HEIGHT), 1080,
        static_cast<EGLint>(EGL_NONE)
    };

    eglbinding::initialize(getProcAddress);

    EGLDisplay eglDpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    if (eglDpy == nullptr)
    {
        std::cerr << "Could not load display" << std::endl;
        return -1;
    }
    else
    {
        std::clog << "Load display " << eglDpy << std::endl;
    }

    EGLint vmajor, vminor;

    if (!eglInitialize(eglDpy, &vmajor, &vminor))
    {
        std::cerr << "Could not initialize EGL: " << std::hex << eglGetError() << std::endl;
    }
    else
    {
        std::clog << "Initialize EGL " << vmajor << "." << vminor << std::endl;
    }

    std::clog << "APIs: " << eglQueryString(eglDpy, static_cast<EGLint>(EGL_CLIENT_APIS)) << std::endl;
    std::clog << "Extensions: " << eglQueryString(eglDpy, static_cast<EGLint>(EGL_EXTENSIONS)) << std::endl;
    std::clog << "Vendor: " << eglQueryString(eglDpy, static_cast<EGLint>(EGL_VENDOR)) << std::endl;
    std::clog << "Version: " << eglQueryString(eglDpy, static_cast<EGLint>(EGL_VERSION)) << std::endl;
    
    // 2. Select an appropriate configuration
    EGLint numConfigs;
    std::array<EGLConfig, 128> eglCfgs;

    eglChooseConfig(eglDpy, configAttribs, eglCfgs.data(), eglCfgs.size(), &numConfigs);

    std::clog << numConfigs << " framebuffer configurations available" <<  std::endl;

    // 3. Create a surface
    EGLSurface eglSurf = eglCreatePbufferSurface(eglDpy, eglCfgs[0], pbufferAttribs);

    if (eglSurf == nullptr)
    {
        std::cerr << "Could not create pixel buffer" << std::endl;
        return -1;
    }

    if (useOpenGL)
    {
        eglBindAPI(EGL_OPENGL_API);
    }
    else
    {
        eglBindAPI(EGL_OPENGL_ES_API);
    }

    static const EGLint ctxattr[] = {
        static_cast<EGLint>(EGL_CONTEXT_MAJOR_VERSION), 3,
        static_cast<EGLint>(EGL_CONTEXT_MINOR_VERSION), 2,
        static_cast<EGLint>(EGL_NONE),
        static_cast<EGLint>(EGL_CONTEXT_CLIENT_VERSION), 3,
        static_cast<EGLint>(EGL_NONE)
    };

    //auto openGLContext = eglCreateContext(eglDpy, nullptr, 0, ctxattr + (useOpenGL ? 0 : 5));
    auto openGLContext = eglCreateContext(eglDpy, eglCfgs[0], 0, ctxattr + (useOpenGL ? 0 : 5));

    //eglMakeCurrent(eglDpy, nullptr, nullptr, openGLContext);
    eglMakeCurrent(eglDpy, eglSurf, eglSurf, openGLContext);

    glbinding::setAfterCallback([](const glbinding::FunctionCall &)
    {
        gl::GLenum error = gl::glGetError();
        if (error != gl::GL_NO_ERROR)
            std::cout << "error: " << error << std::endl;
    });

    glbinding::initialize(eglGetProcAddress);

    glbinding::setCallbackMaskExcept(glbinding::CallbackMask::After, { "glGetError" });

    std::cout << std::endl
            << "OpenGL Version:  " << gl::glGetString(gl::GL_VERSION) << std::endl
            << "OpenGL Vendor:   " << glbinding::aux::ContextInfo::vendor() << std::endl
            << "OpenGL Renderer: " << glbinding::aux::ContextInfo::renderer() << std::endl;

    gl::GLuint fbo;
    gl::GLuint colorBuffer;

    gl::glGenFramebuffers(1, &fbo);
    gl::glGenRenderbuffers(1, &colorBuffer);

    gl::glBindFramebuffer(gl::GL_FRAMEBUFFER, fbo);
    gl::glBindRenderbuffer(gl::GL_RENDERBUFFER, colorBuffer);

    gl::glRenderbufferStorage(gl::GL_RENDERBUFFER, gl::GL_RGB8, 1920, 1080);
    gl::glFramebufferRenderbuffer(gl::GL_FRAMEBUFFER, gl::GL_COLOR_ATTACHMENT0, gl::GL_RENDERBUFFER, colorBuffer);

    gl::glDrawBuffers(1, &gl::GL_COLOR_ATTACHMENT0);

    gl::glViewport(0, 0, 1920, 1080);
    gl::glClearColor(1.0f, 0.5f, 1.0f, 1.0f);
    gl::glClear(gl::GL_COLOR_BUFFER_BIT);

    gl::glFinish();

    std::vector<gl::GLubyte> pixels(1920 * 1080 * 4 * sizeof(gl::GLubyte));

    gl::glReadBuffer(gl::GL_COLOR_ATTACHMENT0);

    gl::glReadPixels(0, 0, 1920, 1080, gl::GL_RGBA, gl::GL_UNSIGNED_BYTE, pixels.data());

    gl::glFinish();

    std::fstream stream("test.1920.1080.rgba.ub.raw", std::fstream::out | std::fstream::binary);
    stream.write(reinterpret_cast<const char *>(pixels.data()), pixels.size());

    stream.close();

    gl::glDeleteRenderbuffers(1, &colorBuffer);
    gl::glDeleteFramebuffers(1, &fbo);

    eglMakeCurrent(eglDpy, nullptr, nullptr, nullptr);

    eglTerminate(eglDpy);
}
