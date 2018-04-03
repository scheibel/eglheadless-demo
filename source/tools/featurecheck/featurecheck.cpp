
#include <iostream>
#include <cstring>
#include <utility>

#include <eglbinding/eglbinding.h>
#include <eglbinding/Version.h>
#include <eglbinding/egl/egl.h>

#include <eglbinding-aux/Meta.h>

#include <glbinding/glbinding.h>
#include <glbinding/Version.h>
#include <glbinding/gl32/gl.h>

#include <glbinding-aux/ContextInfo.h>
#include <glbinding-aux/types_to_string.h>

#include "getProcAddress.h"


using namespace egl;


std::tuple<bool, std::string> testRendering()
{
    glbinding::setAfterCallback([](const glbinding::FunctionCall &)
    {
        gl::GLenum error = gl::glGetError();
        if (error != gl::GL_NO_ERROR)
            std::cout << "error: " << error << std::endl;
    });

    glbinding::initialize([](const char * name) {
        return eglGetProcAddress(name);
    });

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

    const auto & version = glbinding::aux::ContextInfo::version();

    const auto pixelsMatch = pixels[0] == 0xFF && (pixels[1] == 0x7F || pixels[1] == 0x80) && pixels[2] == 0x40 && pixels[3] == 0xFF;

    std::string info;

    const auto versionString = reinterpret_cast<const char *>(gl::glGetString(gl::GL_VERSION));
    if (versionString != nullptr)
    {
        info = std::string(versionString);
    }
    else
    {
        info = "No context information";
    }

    if (!pixelsMatch)
    {
        info = "Pixel mismatch, " + info;
    }

    glbinding::releaseCurrentContext();

    return { pixelsMatch, info };
}

std::tuple<bool, std::string> testPBufferOpenGL4(EGLDisplay eglDpy)
{
    static const EGLint configAttribs[] = {
        static_cast<EGLint>(EGL_SURFACE_TYPE), static_cast<EGLint>(EGL_PBUFFER_BIT),
        static_cast<EGLint>(EGL_BLUE_SIZE), 8,
        static_cast<EGLint>(EGL_GREEN_SIZE), 8,
        static_cast<EGLint>(EGL_RED_SIZE), 8,
        static_cast<EGLint>(EGL_CONFORMANT), static_cast<EGLint>(EGL_OPENGL_BIT),
        static_cast<EGLint>(EGL_NONE)
    };

    static const EGLint pbufferAttribs[] = {
        static_cast<EGLint>(EGL_WIDTH), 1,
        static_cast<EGLint>(EGL_HEIGHT), 1,
        static_cast<EGLint>(EGL_NONE)
    };

    static const EGLint ctxattr[] = {
        static_cast<EGLint>(EGL_CONTEXT_MAJOR_VERSION), 4,
        static_cast<EGLint>(EGL_CONTEXT_MINOR_VERSION), 1,
        static_cast<EGLint>(EGL_NONE)
    };

    eglBindAPI(EGL_OPENGL_API);

    EGLint numConfigs;
    std::array<EGLConfig, 128> eglCfgs;

    eglChooseConfig(eglDpy, configAttribs, eglCfgs.data(), eglCfgs.size(), &numConfigs);

    if (numConfigs == 0)
    {
        return { false, "No configuration found: " + eglbinding::aux::Meta::getString(eglGetError()) };
    }

    EGLSurface eglSurf = eglCreatePbufferSurface(eglDpy, eglCfgs[0], pbufferAttribs);

    if (eglSurf == nullptr)
    {
        return { false, "No surface created: " + eglbinding::aux::Meta::getString(eglGetError()) };
    }

    auto openGLContext = eglCreateContext(eglDpy, eglCfgs[0], nullptr, ctxattr);

    if (openGLContext == nullptr)
    {
        return { false, "No context created: " + eglbinding::aux::Meta::getString(eglGetError()) };
    }

    if (!eglMakeCurrent(eglDpy, eglSurf, eglSurf, openGLContext))
    {
        return { false, "Context not current: " + eglbinding::aux::Meta::getString(eglGetError()) };
    }

    const auto result = testRendering();

    eglMakeCurrent(eglDpy, nullptr, nullptr, nullptr);

    return result;
}

std::tuple<bool, std::string> testPBufferOpenGLES3(EGLDisplay eglDpy)
{
    static const EGLint ctxattr[] = {
        static_cast<EGLint>(EGL_CONTEXT_CLIENT_VERSION), 3,
        static_cast<EGLint>(EGL_NONE)
    };

    static const EGLint configAttribs[] = {
        static_cast<EGLint>(EGL_SURFACE_TYPE), static_cast<EGLint>(EGL_PBUFFER_BIT),
        static_cast<EGLint>(EGL_BLUE_SIZE), 8,
        static_cast<EGLint>(EGL_GREEN_SIZE), 8,
        static_cast<EGLint>(EGL_RED_SIZE), 8,
        static_cast<EGLint>(EGL_CONFORMANT), static_cast<EGLint>(EGL_OPENGL_ES3_BIT),
        static_cast<EGLint>(EGL_NONE)
    };

    static const EGLint pbufferAttribs[] = {
        static_cast<EGLint>(EGL_WIDTH), 1,
        static_cast<EGLint>(EGL_HEIGHT), 1,
        static_cast<EGLint>(EGL_NONE)
    };

    eglBindAPI(EGL_OPENGL_ES_API);

    EGLint numConfigs;
    std::array<EGLConfig, 128> eglCfgs;

    eglChooseConfig(eglDpy, configAttribs, eglCfgs.data(), eglCfgs.size(), &numConfigs);

    if (numConfigs == 0)
    {
        return { false, "No configuration found: " + eglbinding::aux::Meta::getString(eglGetError()) };
    }

    EGLSurface eglSurf = eglCreatePbufferSurface(eglDpy, eglCfgs[0], pbufferAttribs);

    if (eglSurf == nullptr)
    {
        return { false, "No surface created: " + eglbinding::aux::Meta::getString(eglGetError()) };
    }

    auto openGLContext = eglCreateContext(eglDpy, eglCfgs[0], nullptr, ctxattr);

    if (openGLContext == nullptr)
    {
        return { false, "No context created: " + eglbinding::aux::Meta::getString(eglGetError()) };
    }

    if (!eglMakeCurrent(eglDpy, eglSurf, eglSurf, openGLContext))
    {
        return { false, "Context not current: " + eglbinding::aux::Meta::getString(eglGetError()) };
    }

    const auto result = testRendering();

    eglMakeCurrent(eglDpy, nullptr, nullptr, nullptr);

    return result;
}

std::tuple<bool, std::string> testPBufferOpenGLES2(EGLDisplay eglDpy)
{
    static const EGLint ctxattr[] = {
        static_cast<EGLint>(EGL_CONTEXT_CLIENT_VERSION), 2,
        static_cast<EGLint>(EGL_NONE)
    };

    static const EGLint configAttribs[] = {
        static_cast<EGLint>(EGL_SURFACE_TYPE), static_cast<EGLint>(EGL_PBUFFER_BIT),
        static_cast<EGLint>(EGL_BLUE_SIZE), 8,
        static_cast<EGLint>(EGL_GREEN_SIZE), 8,
        static_cast<EGLint>(EGL_RED_SIZE), 8,
        static_cast<EGLint>(EGL_CONFORMANT), static_cast<EGLint>(EGL_OPENGL_ES3_BIT),
        static_cast<EGLint>(EGL_NONE)
    };

    static const EGLint pbufferAttribs[] = {
        static_cast<EGLint>(EGL_WIDTH), 1,
        static_cast<EGLint>(EGL_HEIGHT), 1,
        static_cast<EGLint>(EGL_NONE)
    };

    eglBindAPI(EGL_OPENGL_ES_API);

    EGLint numConfigs;
    std::array<EGLConfig, 128> eglCfgs;

    eglChooseConfig(eglDpy, configAttribs, eglCfgs.data(), eglCfgs.size(), &numConfigs);

    if (numConfigs == 0)
    {
        return { false, "No configuration found: " + eglbinding::aux::Meta::getString(eglGetError()) };
    }

    EGLSurface eglSurf = eglCreatePbufferSurface(eglDpy, eglCfgs[0], pbufferAttribs);

    if (eglSurf == nullptr)
    {
        return { false, "No surface created: " + eglbinding::aux::Meta::getString(eglGetError()) };
    }

    auto openGLContext = eglCreateContext(eglDpy, eglCfgs[0], nullptr, ctxattr);

    if (openGLContext == nullptr)
    {
        return { false, "No context created: " + eglbinding::aux::Meta::getString(eglGetError()) };
    }

    if (!eglMakeCurrent(eglDpy, eglSurf, eglSurf, openGLContext))
    {
        return { false, "Context not current: " + eglbinding::aux::Meta::getString(eglGetError()) };
    }

    const auto result = testRendering();

    eglMakeCurrent(eglDpy, nullptr, nullptr, nullptr);

    return result;
}

std::tuple<bool, std::string> testSurfacelessOpenGL4(EGLDisplay eglDpy)
{
    static const EGLint ctxattr[] = {
        static_cast<EGLint>(EGL_CONTEXT_MAJOR_VERSION), 4,
        static_cast<EGLint>(EGL_CONTEXT_MINOR_VERSION), 1,
        static_cast<EGLint>(EGL_NONE)
    };

    eglBindAPI(EGL_OPENGL_API);

    EGLint numConfigs;
    std::array<EGLConfig, 128> eglCfgs;

    eglGetConfigs(eglDpy, eglCfgs.data(), eglCfgs.size(), &numConfigs);

    if (numConfigs == 0)
    {
        return { false, "No configuration found: " + eglbinding::aux::Meta::getString(eglGetError()) };
    }

    eglBindAPI(EGL_OPENGL_API);

    auto openGLContext = eglCreateContext(eglDpy, eglCfgs[0], nullptr, ctxattr);

    if (openGLContext == nullptr)
    {
        return { false, "No context created: " + eglbinding::aux::Meta::getString(eglGetError()) };
    }

    if (!eglMakeCurrent(eglDpy, reinterpret_cast<egl::EGLSurface>(egl::EGL_NO_SURFACE), reinterpret_cast<egl::EGLSurface>(egl::EGL_NO_SURFACE), openGLContext))
    {
        return { false, "Context not current: " + eglbinding::aux::Meta::getString(eglGetError()) };
    }

    const auto result = testRendering();

    eglMakeCurrent(eglDpy, nullptr, nullptr, nullptr);

    return result;
}

std::tuple<bool, std::string> testSurfacelessOpenGLES3(EGLDisplay eglDpy)
{
    static const EGLint ctxattr[] = {
        static_cast<EGLint>(EGL_CONTEXT_CLIENT_VERSION), 3,
        static_cast<EGLint>(EGL_NONE)
    };

    eglBindAPI(EGL_OPENGL_ES_API);

    EGLint numConfigs;
    std::array<EGLConfig, 128> eglCfgs;

    eglGetConfigs(eglDpy, eglCfgs.data(), eglCfgs.size(), &numConfigs);

    if (numConfigs == 0)
    {
        return { false, "No configuration found: " + eglbinding::aux::Meta::getString(eglGetError()) };
    }

    auto openGLContext = eglCreateContext(eglDpy, eglCfgs[0], nullptr, ctxattr);

    if (openGLContext == nullptr)
    {
        return { false, "No context created: " + eglbinding::aux::Meta::getString(eglGetError()) };
    }

    if (!eglMakeCurrent(eglDpy, reinterpret_cast<egl::EGLSurface>(egl::EGL_NO_SURFACE), reinterpret_cast<egl::EGLSurface>(egl::EGL_NO_SURFACE), openGLContext))
    {
        return { false, "Context not current: " + eglbinding::aux::Meta::getString(eglGetError()) };
    }

    const auto result = testRendering();

    eglMakeCurrent(eglDpy, nullptr, nullptr, nullptr);

    return result;
}

std::tuple<bool, std::string> testSurfacelessOpenGLES2(EGLDisplay eglDpy)
{
    static const EGLint ctxattr[] = {
        static_cast<EGLint>(EGL_CONTEXT_CLIENT_VERSION), 2,
        static_cast<EGLint>(EGL_NONE)
    };

    eglBindAPI(EGL_OPENGL_ES_API);

    EGLint numConfigs;
    std::array<EGLConfig, 128> eglCfgs;

    eglGetConfigs(eglDpy, eglCfgs.data(), eglCfgs.size(), &numConfigs);

    if (numConfigs == 0)
    {
        return { false, "No configuration found: " + eglbinding::aux::Meta::getString(eglGetError()) };
    }

    auto openGLContext = eglCreateContext(eglDpy, eglCfgs[0], nullptr, ctxattr);

    if (openGLContext == nullptr)
    {
        return { false, "No context created: " + eglbinding::aux::Meta::getString(eglGetError()) };
    }

    if (!eglMakeCurrent(eglDpy, reinterpret_cast<egl::EGLSurface>(egl::EGL_NO_SURFACE), reinterpret_cast<egl::EGLSurface>(egl::EGL_NO_SURFACE), openGLContext))
    {
        return { false, "Context not current: " + eglbinding::aux::Meta::getString(eglGetError()) };
    }

    const auto result = testRendering();

    eglMakeCurrent(eglDpy, nullptr, nullptr, nullptr);

    return result;
}

std::tuple<bool, std::string> testConfiglessOpenGL4(EGLDisplay eglDpy)
{
    static const EGLint ctxattr[] = {
        static_cast<EGLint>(EGL_CONTEXT_MAJOR_VERSION), 4,
        static_cast<EGLint>(EGL_CONTEXT_MINOR_VERSION), 1,
        static_cast<EGLint>(EGL_NONE)
    };

    eglBindAPI(EGL_OPENGL_API);

    auto openGLContext = eglCreateContext(eglDpy, nullptr, nullptr, ctxattr);

    if (openGLContext == nullptr)
    {
        return { false, "No context created: " + eglbinding::aux::Meta::getString(eglGetError()) };
    }

    if (!eglMakeCurrent(eglDpy, reinterpret_cast<egl::EGLSurface>(egl::EGL_NO_SURFACE), reinterpret_cast<egl::EGLSurface>(egl::EGL_NO_SURFACE), openGLContext))
    {
        return { false, "Context not current: " + eglbinding::aux::Meta::getString(eglGetError()) };
    }

    const auto result = testRendering();

    eglMakeCurrent(eglDpy, nullptr, nullptr, nullptr);

    return result;
}

std::tuple<bool, std::string> testConfiglessOpenGLES3(EGLDisplay eglDpy)
{
    static const EGLint ctxattr[] = {
        static_cast<EGLint>(EGL_CONTEXT_CLIENT_VERSION), 3,
        static_cast<EGLint>(EGL_NONE)
    };

    eglBindAPI(EGL_OPENGL_ES_API);

    auto openGLContext = eglCreateContext(eglDpy, nullptr, nullptr, ctxattr);

    if (openGLContext == nullptr)
    {
        return { false, "No context created: " + eglbinding::aux::Meta::getString(eglGetError()) };
    }

    if (!eglMakeCurrent(eglDpy, reinterpret_cast<egl::EGLSurface>(egl::EGL_NO_SURFACE), reinterpret_cast<egl::EGLSurface>(egl::EGL_NO_SURFACE), openGLContext))
    {
        return { false, "Context not current: " + eglbinding::aux::Meta::getString(eglGetError()) };
    }

    const auto result = testRendering();

    eglMakeCurrent(eglDpy, nullptr, nullptr, nullptr);

    return result;
}

std::tuple<bool, std::string> testConfiglessOpenGLES2(EGLDisplay eglDpy)
{
    static const EGLint ctxattr[] = {
        static_cast<EGLint>(EGL_CONTEXT_CLIENT_VERSION), 2,
        static_cast<EGLint>(EGL_NONE)
    };

    eglBindAPI(EGL_OPENGL_ES_API);

    auto openGLContext = eglCreateContext(eglDpy, nullptr, nullptr, ctxattr);

    if (openGLContext == nullptr)
    {
        return { false, "No context created: " + eglbinding::aux::Meta::getString(eglGetError()) };
    }

    if (!eglMakeCurrent(eglDpy, reinterpret_cast<egl::EGLSurface>(egl::EGL_NO_SURFACE), reinterpret_cast<egl::EGLSurface>(egl::EGL_NO_SURFACE), openGLContext))
    {
        return { false, "Context not current: " + eglbinding::aux::Meta::getString(eglGetError()) };
    }

    const auto result = testRendering();

    eglMakeCurrent(eglDpy, nullptr, nullptr, nullptr);

    return result;
}

void printResult(const std::string & testName, const std::tuple<bool, std::string> & result)
{
    bool success = false;
    std::string info;
    std::tie(success, info) = result;
    std::clog << testName << " - " << (success ? "SUCCEED" : "FAILED");
    if (info.size() > 0)
    {
        std::clog << " (" << info << ")";
    }
    std::clog << std::endl;
}

int main(int argc, char* argv[])
{
    eglbinding::initialize(getProcAddress);

    const auto eglDpy = eglGetDisplay(0); // EGL_DEFAULT_DISPLAY

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
        std::cerr << "Could not initialize EGL: " << eglbinding::aux::Meta::getString(eglGetError()) << std::endl;
        return -1;
    }
    else
    {
        std::clog << "Initialize EGL " << vmajor << "." << vminor << std::endl;
    }

    const auto apiString = eglQueryString(eglDpy, static_cast<EGLint>(EGL_CLIENT_APIS));
    std::clog << "APIs: " << (strlen(apiString) > 0 ? apiString : "unspecified") << std::endl;
    std::clog << "Extensions: " << eglQueryString(eglDpy, static_cast<EGLint>(EGL_EXTENSIONS)) << std::endl;
    std::clog << "Vendor: " << eglQueryString(eglDpy, static_cast<EGLint>(EGL_VENDOR)) << std::endl;
    std::clog << "Version: " << eglQueryString(eglDpy, static_cast<EGLint>(EGL_VERSION)) << std::endl;

    std::clog << std::endl << "Test Native Display" << std::endl;

    std::clog << std::endl << "PBuffers as surface" << std::endl;
    printResult("OpenGL 4   ", testPBufferOpenGL4(eglDpy));
    printResult("OpenGL ES 2", testPBufferOpenGLES2(eglDpy));
    printResult("OpenGL ES 3", testPBufferOpenGLES3(eglDpy));

    std::clog << std::endl << "No surface" << std::endl;
    printResult("OpenGL 4   ", testSurfacelessOpenGL4(eglDpy));
    printResult("OpenGL ES 2", testSurfacelessOpenGLES2(eglDpy));
    printResult("OpenGL ES 3", testSurfacelessOpenGLES3(eglDpy));

    std::clog << std::endl << "No config" << std::endl;
    printResult("OpenGL 4   ", testConfiglessOpenGL4(eglDpy));
    printResult("OpenGL ES 2", testConfiglessOpenGLES2(eglDpy));
    printResult("OpenGL ES 3", testConfiglessOpenGLES3(eglDpy));

    eglTerminate(eglDpy);

    std::array<EGLDeviceEXT, 255> devices;

    EGLint numDevices = 0;

    if (eglQueryDevicesEXT(255, devices.data(), &numDevices))
    {
        for (auto i = 0; i < numDevices; ++i)
        {
            const auto & device = devices[i];

            const auto deviceDisplay = eglGetPlatformDisplayEXT(EGL_PLATFORM_DEVICE_EXT, device, nullptr);

            std::clog << std::endl << "Test GPU Display " << (i+1) << std::endl;

            eglInitialize(deviceDisplay, nullptr, nullptr);

            if (deviceDisplay == nullptr) // EGL_NO_DISPLAY
            {
                std::clog << std::endl << "Create display from device FAILED" << std::endl;
                continue;
            }

            std::clog << std::endl << "PBuffers as surface" << std::endl;
            printResult("OpenGL 4   ", testPBufferOpenGL4(deviceDisplay));
            printResult("OpenGL ES 2", testPBufferOpenGLES2(deviceDisplay));
            printResult("OpenGL ES 3", testPBufferOpenGLES3(deviceDisplay));

            std::clog << std::endl << "No surface" << std::endl;
            printResult("OpenGL 4   ", testSurfacelessOpenGL4(deviceDisplay));
            printResult("OpenGL ES 2", testSurfacelessOpenGLES2(deviceDisplay));
            printResult("OpenGL ES 3", testSurfacelessOpenGLES3(deviceDisplay));

            std::clog << std::endl << "No config" << std::endl;
            printResult("OpenGL 4   ", testConfiglessOpenGL4(deviceDisplay));
            printResult("OpenGL ES 2", testConfiglessOpenGLES2(deviceDisplay));
            printResult("OpenGL ES 3", testConfiglessOpenGLES3(deviceDisplay));

            eglTerminate(deviceDisplay);
        }
    }

    return 0;
}
