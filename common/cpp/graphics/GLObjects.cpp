//
// Created by n-naka on 2017/01/22.
//

#include <string>
#include "../Log.h"

#include "Vertex.h"
#include "GLObjects.h"
#include "GLObjectsData.h"

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

static void createVBA(GLuint *buffer, GLuint* vba, const Vertex *vertex, int size) {
    glGenBuffers(1, buffer);
    glBindBuffer(GL_ARRAY_BUFFER, *buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * size, vertex, GL_STATIC_DRAW);

    glGenVertexArrays(1, vba);
    glBindVertexArray(*vba);

    glBindBuffer(GL_ARRAY_BUFFER, *vba);
    glVertexAttribPointer(POS_ATTRIB, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, pos));
    glVertexAttribPointer(COLOR_ATTRIB, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, rgba));
    glVertexAttribPointer(UV_ATTRIB, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, uv));
    glEnableVertexAttribArray(POS_ATTRIB);
    glEnableVertexAttribArray(COLOR_ATTRIB);
    glEnableVertexAttribArray(UV_ATTRIB);
}

static void deleteVBA(GLuint buffer, GLuint vba) {
    if (vba) {
        glDeleteVertexArrays(1, &vba);
    }
    if (buffer) {
        glDeleteBuffers(1, &buffer);
    }
}

static GLuint genVideoTexture() {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, texture);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    return texture;
}

GLObjects::GLObjects() :
mVideoTexture(0){
}

void GLObjects::load() {
    for (int i = 0; i < (int)Shader::SHADER_NUM; i++) {
        mShaders.push_back(createProgram(sShaderName[i].vs.c_str(), sShaderName[i].fs.c_str()));
    }

    mVideoTexture = genVideoTexture();

    int meshNum = sizeof(sMeshes) / sizeof(Mesh);
    for (int i = 0; i < meshNum; i++) {
        GLuint buffer, vba;
        int size = sMeshes[i].vertexSize;
        createVBA(&buffer, &vba, sMeshes[i].vertex, size);
        mMeshes.push_back(std::unique_ptr<GLMesh>(new GLMesh{size, buffer, vba,
                                                  mShaders.at((int)Shader::VIDEO_TEXTURE_SHADER),
                                                             0}));
        if (sMeshes[i].shader == Shader::VIDEO_TEXTURE_SHADER) {
            mMeshes.at(mMeshes.size()-1)->updateTexture(mVideoTexture, true);
        }
    }
}

void GLObjects::unload() {
    for (auto it = mShaders.begin(); it != mShaders.end(); ++it) {
        glDeleteProgram(*it);
    }
    mShaders.clear();

    for (auto it = mMeshes.begin(); it != mMeshes.end(); ++it) {
        deleteVBA((*it)->getBuffer(), (*it)->getVBA());
    }
    mMeshes.clear();

    if (mVideoTexture) {
        glDeleteTextures(1, &mVideoTexture);
        mVideoTexture = 0;
    }
}

void GLObjects::draw() {
    for (auto it = mMeshes.begin(); it != mMeshes.end(); ++it) {
        (*it)->draw();
    }
}

void GLObjects::setVideoAspect(float videoAspect, float surfaceAspect) {
    mVideoAspect = videoAspect;
    if (surfaceAspect < mVideoAspect) {
        float y = (1.0f / mVideoAspect) / (1.0f / surfaceAspect);
        sVertex1[0].pos[1] = -y;
        sVertex1[1].pos[1] = -y;
        sVertex1[2].pos[1] = y;
        sVertex1[3].pos[1] = y;
    } else {
        float x = mVideoAspect / surfaceAspect;
        sVertex1[0].pos[0] = -x;
        sVertex1[1].pos[0] = x;
        sVertex1[2].pos[0] = -x;
        sVertex1[3].pos[0] = x;
    }
}