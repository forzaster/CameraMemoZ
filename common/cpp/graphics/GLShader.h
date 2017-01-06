//
// Created by n-naka on 2017/01/04.
//

#ifndef _Z_GLSHADER_H
#define _Z_GLSHADER_H

#include <string>

class GLShader {
private:
    std::string mVS;
    std::string mFS;
public:
    GLShader(std::string&& vs, std::string&& fs);
    ~GLShader();
    void load();
    void bind();
};


#endif //_Z_GLSHADER_H
