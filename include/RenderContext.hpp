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


#include "CsvLog.hpp"
#include "Clock.hpp"
#include "UpdateArea.hpp"

#include <array>
#include <OpenGLUtils/Mesh.hpp>
#include <OpenGLUtils/Shader.hpp>
#include <OpenGLUtils/Camera.hpp>
#include <OpenGLUtils/Texture.hpp>
#include <OpenGLUtils/Image.hpp>


struct RenderContext {
    gut::Mesh               quad;
    gut::Shader             drawShader; // shader for drawing the quad
    gut::Shader             imprintShader; // compute shader for the imprinting
    gut::Shader             errorShader; // compute shader for determining the error
    gut::Shader             gradientShader; // compute shader for determining the gradient
    gut::Shader             errorReductionShader; // compute shader for doing error reduction
    gut::Shader             gradientReductionShader; // compute shader for doing gradient reduction
    gut::Camera             camera;

    // Main texture dimensions
    int                     width;
    int                     height;

    int                     nLevels;
    float                   error; // error from parallel reduction
    std::array<float, 8>    gradient; // gradient from parallel reduction
    std::array<float, 8>    fGradient; // momentum filtering
    float                   fRatio;
    std::array<float, 8>    gdRateMod; // gradient descent rate modifiers
    float                   pError; // previous error
    double                  imprintRatio; // imprint brush area / canvas area
    int                     nImprints; // number of imprints

    // Imprint parameters
    int                     imprintTextureWidth;
    int                     imprintTextureHeight;
    // 0: x
    // 1: y
    // 2: scale
    // 3: angle
    // 4-6: color
    std::array<float, 8>    imprintParams;
    UpdateArea              updateArea;

    // Textures for the imprint process
    gut::Texture            targetTexture;
    gut::Texture            imprintTexture; // Texture to be imprinted (brush)
    gut::Texture            errorTexture;
    gut::Texture            gradientTexture;

    // 2 textures for alterating between read and write
    gut::Texture            texture1;
    gut::Texture            texture2;
    gut::Texture*           readTexture;
    gut::Texture*           writeTexture;

    // Image for error data
    gut::Image<float>       errorImage;

    // Log
    CsvLog                  log;
    Clock                   clock;

    RenderContext() :
        targetTexture   (GL_TEXTURE_2D, GL_RGBA32F),
        imprintTexture  (GL_TEXTURE_2D, GL_RGBA32F),
        errorTexture    (GL_TEXTURE_2D, GL_R32F),
        gradientTexture (GL_TEXTURE_2D_ARRAY, GL_R32F),
        texture1        (GL_TEXTURE_2D, GL_RGBA32F),
        texture2        (GL_TEXTURE_2D, GL_RGBA32F),
        readTexture     (&texture1),
        writeTexture    (&texture2),
        log             ("log.csv", ';', 3)
    {}
};


#endif //COLLAGEMAKER2_RENDERCONTEXT_HPP
