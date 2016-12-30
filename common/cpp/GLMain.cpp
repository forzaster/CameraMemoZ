//
// Created by n-naka on 2016/12/30.
//

#include <string>
#include "gl_lib.h"
#include "Log.h"
#include "GLMain.h"

struct Vertex {
    GLfloat pos[2];
    GLubyte rgba[4];
};

#ifdef GLESv3
#define STR(s) #s
#define STRV(s) STR(s)

#define POS_ATTRIB 0
#define COLOR_ATTRIB 1

static auto sVertexShader =
        "#version 300 es\n"
                "layout(location = " STRV(POS_ATTRIB) ") in vec2 pos;\n"
                "layout(location=" STRV(COLOR_ATTRIB) ") in vec4 color;\n"
                "out vec4 vColor;\n"
                "void main() {\n"
                "    gl_Position = vec4(pos, 0.0, 1.0);\n"
                "    vColor = color;\n"
                "}\n";

static auto sFragmentShader =
        "#version 300 es\n"
                "precision mediump float;\n"
                "in vec4 vColor;\n"
                "out vec4 outColor;\n"
                "void main() {\n"
                "    outColor = vColor;\n"
                "}\n";
#else
static auto sVertexShader =
        "attribute vec4 vPosition;\n"
                "void main() {\n"
                "  gl_Position = vPosition;\n"
                "}\n";

static auto sFragmentShader =
        "precision mediump float;\n"
                "void main() {\n"
                "  gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);\n"
                "}\n";
#endif

static const Vertex sQuad[4] = {
        {{-0.7f, -0.7f}, {0x00, 0xFF, 0x00}},
        {{ 0.7f, -0.7f}, {0x00, 0x00, 0xFF}},
        {{-0.7f,  0.7f}, {0xFF, 0x00, 0x00}},
        {{ 0.7f,  0.7f}, {0xFF, 0xFF, 0xFF}},
};
static const GLfloat sTriangleVertices[] = { 0.0f, 0.5f, -0.5f, -0.5f,
                                      0.5f, -0.5f };

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
#ifdef GLESv3
    glGenBuffers(VB_COUNT, mImpl->mVB);
    glBindBuffer(GL_ARRAY_BUFFER, mImpl->mVB[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sQuad), &sQuad[0], GL_STATIC_DRAW);

    glGenVertexArrays(1, &mImpl->mVBState);
    glBindVertexArray(mImpl->mVBState);

    glBindBuffer(GL_ARRAY_BUFFER, mImpl->mVB[0]);
    glVertexAttribPointer(POS_ATTRIB, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, pos));
    glVertexAttribPointer(COLOR_ATTRIB, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, rgba));
    glEnableVertexAttribArray(POS_ATTRIB);
    glEnableVertexAttribArray(COLOR_ATTRIB);

#else
    mImpl->mVBState = glGetAttribLocation(mImpl->mProgram, "vPosition");
    checkGlError("glGetAttribLocation");
    LOGI("glGetAttribLocation(\"vPosition\") = %d\n",
         mImpl->mVBState);
#endif
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
#ifdef GLESv3
    glBindVertexArray(mImpl->mVBState);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 1);
#else
    glVertexAttribPointer(mImpl->mVBState, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, pos));
    checkGlError("glVertexAttribPointer");
    glEnableVertexAttribArray(mImpl->mVBState);
    checkGlError("glEnableVertexAttribArray");
    glDrawArrays(GL_TRIANGLES, 0, 3);
    checkGlError("glDrawArrays");
#endif
}

void GLMain::resize() {

}

