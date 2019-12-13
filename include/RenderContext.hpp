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
    gut::Shader     drawShader;
    gut::Camera     camera;
    gut::Texture    texture1;
    gut::Texture    texture2;

    RenderContext() = default;
};


#endif //COLLAGEMAKER2_RENDERCONTEXT_HPP
