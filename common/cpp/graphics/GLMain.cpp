//
// Created by n-naka on 2016/12/30.
//

#include <string>
#include "../Log.h"
#include "GLMain.h"
#include "GLObjects.h"

class GLMain::Impl {
public:
    std::unique_ptr<GLObjects> mGLObjects;
    bool mInitialized;
    float mVideoAspect;
    int mWidth;
    int mHeight;

    Impl() : mGLObjects(nullptr),
             mInitialized(false),
             mVideoAspect(1.0f),
             mWidth(0),
             mHeight(0)
    {
    }

    ~Impl() {
        finalize();
    }

    void finalize() {
        if (mGLObjects) {
            mGLObjects->unload();
        }
    }
};

GLMain::GLMain() :
mImpl(std::unique_ptr<Impl>(new Impl()))
{
}

GLMain::~GLMain() {
}

GLMain& GLMain::instance() {
    static GLMain* instance = nullptr;
    if (!instance) {
        instance = new GLMain();
    }
    return *instance;
}

bool GLMain::init(int width, int height) {
    printGLString("Version", GL_VERSION);
    printGLString("Vendor", GL_VENDOR);
    printGLString("Renderer", GL_RENDERER);
    printGLString("Extensions", GL_EXTENSIONS);

    LOGI("setupGraphics(%d, %d)", width, height);
    mImpl->mWidth = width;
    mImpl->mHeight = height;

    mImpl->mGLObjects = std::unique_ptr<GLObjects>(new GLObjects());
    mImpl->mGLObjects->setVideoAspect(mImpl->mVideoAspect, width, height);
    mImpl->mGLObjects->load();

    glViewport(0, 0, width, height);
    checkGlError("glViewport");

    mImpl->mInitialized = true;
    return true;
}

void GLMain::draw() {
    static float grey;
    grey += 0.01f;
    if (grey > 1.0f) {
        grey = 0.0f;
    }
    glClearColor(grey, grey, grey, 1.0f);
    checkGlError("glClearColor");
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    checkGlError("glClear");

    mImpl->mGLObjects->draw();
}

bool GLMain::resize(int width, int height) {
    if (mImpl->mInitialized) {
        glViewport(0, 0, width, height);
        return true;
    }
    return false;
}

unsigned int GLMain::genTexture() {
    return mImpl->mGLObjects->getVideoTexture();
}

void GLMain::setVideoAspect(float aspect) {
    mImpl->mVideoAspect = aspect;
    if (mImpl->mGLObjects) {
        mImpl->mGLObjects->setVideoAspect(aspect,
        mImpl->mWidth, mImpl->mHeight);
    }
}
