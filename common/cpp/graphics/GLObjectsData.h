//
// Created by n-naka on 2017/01/22.
//

#ifndef Z_GLOBJECTSDATA_H
#define Z_GLOBJECTSDATA_H

#include <string>

#include "Vertex.h"
#include "VideoTextureShader.h"

enum class Shader {
    VIDEO_TEXTURE_SHADER = 0,
    SHADER_NUM
};

struct ShaderName {
    std::string vs;
    std::string fs;
};

struct Mesh {
    const Vertex* vertex;
    int vertexSize;
    Shader shader;
};

static const ShaderName sShaderName[] = {
        {VideoTextureShader::vs(), VideoTextureShader::fs()},
};

static const Vertex sVertex1[] = {
        {{-0.7f, -0.7f}, {0x00, 0xFF, 0x00}, {0.0f, 1.0f}},
        {{ 0.7f, -0.7f}, {0x00, 0x00, 0xFF}, {1.0f, 1.0f}},
        {{-0.7f,  0.7f}, {0xFF, 0x00, 0x00}, {0.0f, 0.0f}},
        {{ 0.7f,  0.7f}, {0xFF, 0xFF, 0xFF}, {1.0f, 0.0f}},
};

static const Mesh sMeshes[] = {
        {sVertex1, sizeof(sVertex1) / sizeof(Vertex), Shader::VIDEO_TEXTURE_SHADER}
};

#endif //Z_GLOBJECTSDATA_H
