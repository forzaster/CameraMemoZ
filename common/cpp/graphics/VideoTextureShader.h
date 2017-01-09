//
// Created by n-naka on 2017/01/09.
//

#ifndef Z_VIDEOTEXTURESHADER_H
#define Z_VIDEOTEXTURESHADER_H

#include "ShaderAttribDef.h"

namespace VideoTextureShader {
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

    static std::string vs() {
        return std::string(sVertexShader);
    }

    static std::string fs() {
        return std::string(sFragmentShader);
    }
};
#endif //Z_VIDEOTEXTURESHADER_H
