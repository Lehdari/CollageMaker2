//
// Project: CollageMaker2
// File: RenderContext.hpp
//
// Copyright (c) 2019 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENSE which is distributed
// with this source code package.
//

#ifndef COLLAGEMAKER2_RENDERCONTEXT_HPP
#define COLLAGEMAKER2_RENDERCONTEXT_HPP


#include <OpenGLUtils/Mesh.hpp>
#include <OpenGLUtils/Shader.hpp>
#include <OpenGLUtils/Camera.hpp>
#include <OpenGLUtils/Texture.hpp>


struct RenderContext {
    gut::Mesh       quad;
    gut::Shader     drawShader; // shader for drawing the quad
    gut::Shader     imprintShader; // compute shader for the imprinting
    gut::Camera     camera;

    // Main texture dimensions
    int             width;
    int             height;

    // Imprint parameters
    int             imprintWidth;
    int             imprintHeight;
    float           imprintX;
    float           imprintY;
    float           imprintScale;
    float           imprintAngle;

    // Textures for the imprint process
    gut::Texture    targetTexture;
    gut::Texture    imprintTexture; // Texture to be imprinted (brush)
    gut::Texture    errorTexture;

    // 2 textures for alterating between read and write
    gut::Texture    texture1;
    gut::Texture    texture2;
    gut::Texture*   readTexture;
    gut::Texture*   writeTexture;

    RenderContext() :
        readTexture     (&texture1),
        writeTexture    (&texture2)
    {}
};


#endif //COLLAGEMAKER2_RENDERCONTEXT_HPP
