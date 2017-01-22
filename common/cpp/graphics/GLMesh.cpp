//
// Created by n-naka on 2017/01/21.
//

#include <memory>

#include "GLMesh.h"

GLMesh::GLMesh(int vertexNum, GLuint buffer, GLuint vba, GLuint program, GLuint texture) :
        mVertexNum(vertexNum), mBuffer(buffer), mVBA(vba), mProgram(program), mTexture(texture) {
    mTextureTarget = GL_TEXTURE_2D;
}

GLMesh::~GLMesh() {
}

void GLMesh::updateTexture(GLuint texture, bool externalOes) {
    mTexture = texture;
    mTextureTarget = externalOes ? GL_TEXTURE_EXTERNAL_OES : GL_TEXTURE_2D;
}

void GLMesh::draw() const {
    if (mProgram) {
        glUseProgram(mProgram);
        checkGlError("glUseProgram");
    }

    if (mTexture) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(mTextureTarget, mTexture);
    }

    glBindVertexArray(mVBA);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, mVertexNum, 1);

    if (mProgram) {
        glDeleteProgram(mProgram);
    }
}