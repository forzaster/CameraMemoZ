//
// Created by n-naka on 2017/01/04.
//

#ifndef _Z_GLSHADER_H
#define _Z_GLSHADER_H

#include <string>
#include "gl_lib.h"

class GLShader {
private:
    std::string mVS;
    std::string mFS;
    GLuint mProgram;
public:
    GLShader(std::string&& vs, std::string&& fs);
    GLShader(GLShader&& rhs) = default;
    GLShader(const GLShader& rhs) = default;
    void load();
    bool bindStart();
    void bindEnd();
    void unload();
};


#endif //_Z_GLSHADER_H
