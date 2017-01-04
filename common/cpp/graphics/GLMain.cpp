//
// Created by n-naka on 2016/12/30.
//

#include <string>
#include "../Log.h"
#include "gl_lib.h"
#include "GLMain.h"

struct Vertex {
    GLfloat pos[2];
    GLubyte rgba[4];
    GLfloat uv[2];
};

#define STR(s) #s
#define STRV(s) STR(s)

#define POS_ATTRIB 0
#define COLOR_ATTRIB 1
#define UV_ATTRIB 2

static auto sVertexShader =
        "#version 300 es\n"
                "layout(location = " STRV(POS_ATTRIB) ") in vec2 pos;\n"
                "layout(location=" STRV(COLOR_ATTRIB) ") in vec4 color;\n"
                "layout(location=" STRV(UV_ATTRIB) ") in vec2 uv;\n"
                "out vec4 vColor;\n"
                "out vec2 vUv;\n"
                "void main() {\n"
                "    gl_Position = vec4(pos, 0.0, 1.0);\n"
                "    vColor = color;\n"
                "    vUv = uv;\n"
                "}\n";

static auto sFragmentShader =
        "#version 300 es\n"
                "#extension GL_OES_EGL_image_external : require\n"
                "precision mediump float;\n"
                "uniform samplerExternalOES tex;\n"
                "in vec4 vColor;\n"
                "in vec2 vUv;\n"
                "out vec4 outColor;\n"
                "void main() {\n"
                "    outColor = texture(tex, vUv);\n"
                "}\n";

static const Vertex sQuad[4] = {
        {{-0.7f, -0.7f}, {0x00, 0xFF, 0x00}, {0.0f, 0.0f}},
        {{ 0.7f, -0.7f}, {0x00, 0x00, 0xFF}, {1.0f, 0.0f}},
        {{-0.7f,  0.7f}, {0xFF, 0x00, 0x00}, {0.0f, 1.0f}},
        {{ 0.7f,  0.7f}, {0xFF, 0xFF, 0xFF}, {1.0f, 1.0f}},
};

static GLuint loadShader(GLenum shaderType, const char* pSource) {
    GLuint shader = glCreateShader(shaderType);
    if (shader) {
        glShaderSource(shader, 1, &pSource, NULL);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen) {
                char* buf = (char*) malloc(infoLen);
                if (buf) {
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    LOGE("Could not compile shader %d:\n%s\n",
                         shaderType, buf);
                    free(buf);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
    }
    return shader;
}

static GLuint createProgram(const char* pVertexSource, const char* pFragmentSource) {
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource);
    if (!vertexShader) {
        return 0;
    }

    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource);
    if (!pixelShader) {
        return 0;
    }

    GLuint program = glCreateProgram();
    if (program) {
        glAttachShader(program, vertexShader);
        checkGlError("glAttachShader");
        glAttachShader(program, pixelShader);
        checkGlError("glAttachShader");
        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
                char* buf = (char*) malloc(bufLength);
                if (buf) {
                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                    LOGE("Could not link program:\n%s\n", buf);
                    free(buf);
                }
            }
            glDeleteProgram(program);
            program = 0;
        }
    }
    return program;
}

#define VB_COUNT 1

class GLMain::Impl {
public:
    GLuint mProgram;
    GLuint mVB[VB_COUNT];
    GLuint mVBState;
    GLuint mTexture;
    GLuint mTexHandle;

    Impl() : mTexture(0) { }
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
    mImpl->mProgram = createProgram(sVertexShader, sFragmentShader);
    if (!mImpl->mProgram) {
        LOGE("Could not create program.");
        return false;
    }

    mImpl->mTexHandle = glGetUniformLocation(mImpl->mProgram, "tex");

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

    glUseProgram(mImpl->mProgram);
    checkGlError("glUseProgram");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, mImpl->mTexture);

    glBindVertexArray(mImpl->mVBState);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 1);

    glUseProgram(0);
}

void GLMain::resize() {

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

