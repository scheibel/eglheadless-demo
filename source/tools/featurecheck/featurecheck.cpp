
#include <iostream>
#include <cstring>

#include <glbinding/glbinding.h>
#include <glbinding/Version.h>
#include <glbinding/gl32/gl.h>

#include <glbinding-aux/ContextInfo.h>
#include <glbinding-aux/types_to_string.h>

#include <eglheadless/eglheadless.h>


namespace
{


EGLDisplay eglDpy = nullptr;


}

bool testRendering()
{
    glbinding::setAfterCallback([](const glbinding::FunctionCall &)
    {
        gl::GLenum error = gl::glGetError();
        if (error != gl::GL_NO_ERROR)
            std::cout << "error: " << error << std::endl;
    });

    glbinding::initialize(eglGetProcAddress);

    glbinding::setCallbackMaskExcept(glbinding::CallbackMask::After, { "glGetError" });

    gl::GLuint fbo;
    gl::GLuint colorBuffer;

    gl::glGenFramebuffers(1, &fbo);
    gl::glGenRenderbuffers(1, &colorBuffer);

    gl::glBindFramebuffer(gl::GL_FRAMEBUFFER, fbo);
    gl::glBindRenderbuffer(gl::GL_RENDERBUFFER, colorBuffer);

    gl::glRenderbufferStorage(gl::GL_RENDERBUFFER, gl::GL_RGB8, 1, 1);
    gl::glFramebufferRenderbuffer(gl::GL_FRAMEBUFFER, gl::GL_COLOR_ATTACHMENT0, gl::GL_RENDERBUFFER, colorBuffer);

    gl::glDrawBuffers(1, &gl::GL_COLOR_ATTACHMENT0);

    gl::glViewport(0, 0, 1, 1);
    gl::glClearColor(1.0f, 0.5f, 0.25f, 1.0f);
    gl::glClear(gl::GL_COLOR_BUFFER_BIT);

    gl::glFinish();

    std::vector<gl::GLubyte> pixels(1 * 1 * 4 * sizeof(gl::GLubyte));

    gl::glReadBuffer(gl::GL_COLOR_ATTACHMENT0);

    gl::glReadPixels(0, 0, 1, 1, gl::GL_RGBA, gl::GL_UNSIGNED_BYTE, pixels.data());

    gl::glFinish();

    gl::glDeleteRenderbuffers(1, &colorBuffer);
    gl::glDeleteFramebuffers(1, &fbo);

    glbinding::releaseCurrentContext();

    return pixels[0] == 0xFF && pixels[1] == 0x7F && pixels[2] == 0x40 && pixels[3] == 0xFF;
}

bool testPBufferOpenGL4()
{
    static const EGLint configAttribs[] = {
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
        EGL_NONE
    };

    static const EGLint pbufferAttribs[] = {
        EGL_WIDTH, 1,
        EGL_HEIGHT, 1,
        EGL_NONE
    };

    static const EGLint ctxattr[] = {
        EGL_CONTEXT_MAJOR_VERSION, 4,
        EGL_CONTEXT_MINOR_VERSION, 1,
        EGL_NONE
    };

    eglBindAPI(EGL_OPENGL_API);

    EGLint numConfigs;
    std::array<EGLConfig, 128> eglCfgs;

    eglChooseConfig(eglDpy, configAttribs, eglCfgs.data(), eglCfgs.size(), &numConfigs);

    EGLSurface eglSurf = eglCreatePbufferSurface(eglDpy, eglCfgs[0], pbufferAttribs);

    if (eglSurf == nullptr)
    {
        return false;
    }

    auto openGLContext = eglCreateContext(eglDpy, eglCfgs[0], 0, ctxattr);

    eglMakeCurrent(eglDpy, eglSurf, eglSurf, openGLContext);

    const auto result = testRendering();

    eglMakeCurrent(eglDpy, nullptr, nullptr, nullptr);

    return result;
}

bool testPBufferOpenGLES3()
{
    static const EGLint ctxattr[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_NONE
    };

    static const EGLint configAttribs[] = {
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_NONE
    };

    static const EGLint pbufferAttribs[] = {
        EGL_WIDTH, 1,
        EGL_HEIGHT, 1,
        EGL_NONE
    };

    eglBindAPI(EGL_OPENGL_ES_API);

    EGLint numConfigs;
    std::array<EGLConfig, 128> eglCfgs;

    eglChooseConfig(eglDpy, configAttribs, eglCfgs.data(), eglCfgs.size(), &numConfigs);

    EGLSurface eglSurf = eglCreatePbufferSurface(eglDpy, eglCfgs[0], pbufferAttribs);

    if (eglSurf == nullptr)
    {
        return false;
    }

    auto openGLContext = eglCreateContext(eglDpy, eglCfgs[0], 0, ctxattr);

    eglMakeCurrent(eglDpy, eglSurf, eglSurf, openGLContext);

    const auto result = testRendering();

    eglMakeCurrent(eglDpy, nullptr, nullptr, nullptr);

    return result;
}

bool testPBufferOpenGLES2()
{
    static const EGLint ctxattr[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

    static const EGLint configAttribs[] = {
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_NONE
    };

    static const EGLint pbufferAttribs[] = {
        EGL_WIDTH, 1,
        EGL_HEIGHT, 1,
        EGL_NONE
    };

    eglBindAPI(EGL_OPENGL_ES_API);

    EGLint numConfigs;
    std::array<EGLConfig, 128> eglCfgs;

    eglChooseConfig(eglDpy, configAttribs, eglCfgs.data(), eglCfgs.size(), &numConfigs);

    EGLSurface eglSurf = eglCreatePbufferSurface(eglDpy, eglCfgs[0], pbufferAttribs);

    if (eglSurf == nullptr)
    {
        return false;
    }

    auto openGLContext = eglCreateContext(eglDpy, eglCfgs[0], 0, ctxattr);

    eglMakeCurrent(eglDpy, eglSurf, eglSurf, openGLContext);

    const auto result = testRendering();

    eglMakeCurrent(eglDpy, nullptr, nullptr, nullptr);

    return result;
}

bool testNativeSurfaceOpenGL4()
{
    static const EGLint ctxattr[] = {
        EGL_CONTEXT_MAJOR_VERSION, 4,
        EGL_CONTEXT_MINOR_VERSION, 1,
        EGL_NONE
    };

    eglBindAPI(EGL_OPENGL_API);

    const auto result = testRendering();

    return result;
}

bool testNativeSurfaceOpenGLES3()
{
    eglBindAPI(EGL_OPENGL_ES_API);

    const auto result = testRendering();

    return result;
}

bool testNativeSurfaceOpenGLES2()
{
    eglBindAPI(EGL_OPENGL_ES_API);

    const auto result = testRendering();

    return result;
}

bool testSurfacelessOpenGL4()
{
    static const EGLint ctxattr[] = {
        EGL_CONTEXT_MAJOR_VERSION, 4,
        EGL_CONTEXT_MINOR_VERSION, 1,
        EGL_NONE
    };

    eglBindAPI(EGL_OPENGL_API);

    auto openGLContext = eglCreateContext(eglDpy, nullptr, 0, ctxattr);

    eglMakeCurrent(eglDpy, nullptr, nullptr, openGLContext);

    const auto result = testRendering();

    eglMakeCurrent(eglDpy, nullptr, nullptr, nullptr);

    return result;
}

bool testSurfacelessOpenGLES3()
{
    static const EGLint ctxattr[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_NONE
    };

    eglBindAPI(EGL_OPENGL_ES_API);

    auto openGLContext = eglCreateContext(eglDpy, nullptr, 0, ctxattr);

    eglMakeCurrent(eglDpy, nullptr, nullptr, openGLContext);

    const auto result = testRendering();

    eglMakeCurrent(eglDpy, nullptr, nullptr, nullptr);

    return result;
}

bool testSurfacelessOpenGLES2()
{
    static const EGLint ctxattr[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

    eglBindAPI(EGL_OPENGL_ES_API);

    auto openGLContext = eglCreateContext(eglDpy, nullptr, 0, ctxattr);

    eglMakeCurrent(eglDpy, nullptr, nullptr, openGLContext);

    const auto result = testRendering();

    eglMakeCurrent(eglDpy, nullptr, nullptr, nullptr);

    return result;
}

void printResult(const std::string & testName, const bool result)
{
    std::clog << testName << " - " << (result ? "Success" : "Failure") << std::endl;
}

int main(int argc, char* argv[])
{
    eglDpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);

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
        return -1;
    }
    else
    {
        std::clog << "Initialize EGL " << vmajor << "." << vminor << std::endl;
    }

    const auto apiString = eglQueryString(eglDpy, EGL_CLIENT_APIS);
    std::clog << "APIs: " << (strlen(apiString) > 0 ? apiString : "unspecified") << std::endl;
    std::clog << "Extensions: " << eglQueryString(eglDpy, EGL_EXTENSIONS) << std::endl;
    std::clog << "Vendor: " << eglQueryString(eglDpy, EGL_VENDOR) << std::endl;
    std::clog << "Version: " << eglQueryString(eglDpy, EGL_VERSION) << std::endl;

    std::clog << std::endl << "Test EGL context creation configurations" << std::endl;

    std::clog << std::endl << "PBuffers as surface" << std::endl;
    printResult("OpenGL 4   ", testPBufferOpenGL4());
    printResult("OpenGL ES 2", testPBufferOpenGLES2());
    printResult("OpenGL ES 3", testPBufferOpenGLES3());

    /*
    std::clog << std::endl << "GPU-created surface" << std::endl;
    printResult("OpenGL 4   ", testNativeSurfaceOpenGL4());
    printResult("OpenGL ES 2", testNativeSurfaceOpenGLES2());
    printResult("OpenGL ES 3", testNativeSurfaceOpenGLES3());
    */

    std::clog << std::endl << "No surface" << std::endl;
    printResult("OpenGL 4   ", testSurfacelessOpenGL4());
    printResult("OpenGL ES 2", testSurfacelessOpenGLES2());
    printResult("OpenGL ES 3", testSurfacelessOpenGLES3());

    eglTerminate(eglDpy);

    return 0;
}
