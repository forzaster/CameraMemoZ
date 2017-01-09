//
// Created by n-naka on 2016/12/30.
//

#ifndef _Z_GLMAIN_H
#define _Z_GLMAIN_H

#include <memory>

class GLMain {
public:
    GLMain();
    ~GLMain();

    bool init(int width, int height);
    void draw();
    bool resize(int width, int height);
    unsigned int genTexture();
    static GLMain& instance();

private:
    class Impl;
    std::unique_ptr<Impl> mImpl;
};


#endif //_Z_GLMAIN_H
