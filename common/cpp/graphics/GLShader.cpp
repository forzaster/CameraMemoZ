//
// Created by n-naka on 2017/01/04.
//

#include "GLShader.h"

GLShader::GLShader(std::string&& vs, std::string&& fs) :
mVS(std::move(vs)),
mFS(std::move(fs)) {
}

GLShader::~GLShader() {
}

void GLShader::load() {
}

void GLShader::bind() {

}