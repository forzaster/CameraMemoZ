//
// Created by n-naka on 2016/12/30.
//

#ifndef CAMERAMEMOZ_GLMAIN_H
#define CAMERAMEMOZ_GLMAIN_H

#include <memory>

class GLMain {
public:
    GLMain();
    ~GLMain();

    bool init(int width, int height);
    void draw();
    void resize();
    static GLMain& instance();

private:
    class Impl;
    std::unique_ptr<Impl> mImpl;
};


#endif //CAMERAMEMOZ_GLMAIN_H
