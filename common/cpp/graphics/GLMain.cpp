//
// Created by n-naka on 2016/12/30.
//

#include <string>
#include "../Log.h"
#include "GLMain.h"
#include "GLShader.h"
#include "VideoTextureShader.h"

struct Vertex {
    GLfloat pos[2];
    GLubyte rgba[4];
    GLfloat uv[2];
};

static const Vertex sQuad[4] = {
        {{-0.7f, -0.7f}, {0x00, 0xFF, 0x00}, {0.0f, 1.0f}},
        {{ 0.7f, -0.7f}, {0x00, 0x00, 0xFF}, {1.0f, 1.0f}},
        {{-0.7f,  0.7f}, {0xFF, 0x00, 0x00}, {0.0f, 0.0f}},
        {{ 0.7f,  0.7f}, {0xFF, 0xFF, 0xFF}, {1.0f, 0.0f}},
};

#define VB_COUNT 1

class GLMain::Impl {
public:
    GLuint mVB[VB_COUNT];
    GLuint mVBState;
    GLuint mTexture;
    std::unique_ptr<GLShader> mShader;
    bool mInitialized;

    Impl() : mVBState(0),
             mTexture(0),
             mShader(nullptr),
             mInitialized(false)
    {
        for (int i = 0; i < VB_COUNT; i++) {
            mVB[i] = 0;
        }
    }

    ~Impl() {
        finalize();
    }

    void finalize() {
        if (mShader)
        {
            mShader->unload();
            mShader = 0;
        }
        if (mVBState) {
            glDeleteVertexArrays(1, &mVBState);
            mVBState = 0;
        }
        glDeleteBuffers(VB_COUNT, mVB);
        for (int i = 0; i < VB_COUNT; i++) {
            mVB[i] = 0;
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
    mImpl->mShader = std::unique_ptr<GLShader>(
            new GLShader(VideoTextureShader::vs(), VideoTextureShader::fs()));
    mImpl->mShader->load();

    glGenBuffers(VB_COUNT, mImpl->mVB);
    glBindBuffer(GL_ARRAY_BUFFER, mImpl->mVB[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sQuad), &sQuad[0], GL_STATIC_DRAW);

    glGenVertexArrays(1, &mImpl->mVBState);
    glBindVertexArray(mImpl->mVBState);

    glBindBuffer(GL_ARRAY_BUFFER, mImpl->mVB[0]);
    glVertexAttribPointer(POS_ATTRIB, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, pos));
    glVertexAttribPointer(COLOR_ATTRIB, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, rgba));
    glVertexAttribPointer(UV_ATTRIB, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, uv));
    glEnableVertexAttribArray(POS_ATTRIB);
    glEnableVertexAttribArray(COLOR_ATTRIB);
    glEnableVertexAttribArray(UV_ATTRIB);

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

    mImpl->mShader->bindStart();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, mImpl->mTexture);

    glBindVertexArray(mImpl->mVBState);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 1);

    mImpl->mShader->bindEnd();
}

bool GLMain::resize(int width, int height) {
    if (mImpl->mInitialized) {
        glViewport(0, 0, width, height);
        return true;
    }
    return false;
}

unsigned int GLMain::genTexture() {
    glGenTextures(1, &mImpl->mTexture);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, mImpl->mTexture);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    return mImpl->mTexture;
}

