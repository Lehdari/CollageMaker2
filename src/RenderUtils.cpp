//
// Project: CollageMaker2
// File: RenderUtils.cpp
//
// Copyright (c) 2019 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENSE which is distributed
// with this source code package.
//

#include "RenderUtils.hpp"
#include "Utils/MathUtils.hpp"


void renderError(RenderContext& renderContext)
{
    // Find update boundaries
    auto& updateRect = renderContext.updateArea.updateError(renderContext);

    // Bind uniforms
    renderContext.errorShader.use();
    renderContext.errorShader.setUniform("imprintTextureWidth", renderContext.imprintTextureWidth);
    renderContext.errorShader.setUniform("imprintTextureHeight", renderContext.imprintTextureHeight);
    renderContext.errorShader.setUniform("imprintXOffset", updateRect.x);
    renderContext.errorShader.setUniform("imprintYOffset", updateRect.y);
    renderContext.errorShader.setUniform("imprintParams", renderContext.imprintParams);
    renderContext.readTexture->bind(GL_TEXTURE0);
    renderContext.targetTexture.bind(GL_TEXTURE1);
    renderContext.imprintTexture.bind(GL_TEXTURE2);
    renderContext.errorTexture.bindImage(0);
    renderContext.errorShader.dispatch(updateRect.w, updateRect.h, 1);

    // Reduction
    renderContext.errorReductionShader.use();
    renderContext.errorTexture.bind(GL_TEXTURE0);
    for (int level=0; level<renderContext.nLevels-1; ++level) {
        // Downscaled update area
        Rectangle levelRect(updateRect);
        int d = 2 << level;
        levelRect.x = levelRect.x / d - 0;
        levelRect.y = levelRect.y / d - 0;
        levelRect.w = std::ceil(levelRect.w / (float)d) + 1;
        levelRect.h = std::ceil(levelRect.h / (float)d) + 1;
        levelRect.clamp(0, renderContext.width / d, 0, renderContext.height / d);

        renderContext.errorTexture.bindImage(0, level+1);
        renderContext.errorReductionShader.setUniform("level", level);
        renderContext.errorReductionShader.setUniform("xOffset", levelRect.x);
        renderContext.errorReductionShader.setUniform("yOffset", levelRect.y);
        renderContext.errorReductionShader.dispatch(levelRect.w, levelRect.h, 1);
    }

    // Fetch error
    renderContext.pError = renderContext.error;
    glGetTexImage(GL_TEXTURE_2D, renderContext.nLevels-1, GL_RED, GL_FLOAT, &renderContext.error);
}

void renderGradient(RenderContext& renderContext)
{
    // Find update boundaries
    auto& updateRect = renderContext.updateArea.updateGradient(renderContext);

    // Bind uniforms
    renderContext.gradientShader.use();
    renderContext.gradientShader.setUniform("imprintTextureWidth", renderContext.imprintTextureWidth);
    renderContext.gradientShader.setUniform("imprintTextureHeight", renderContext.imprintTextureHeight);
    renderContext.gradientShader.setUniform("imprintXOffset", updateRect.x);
    renderContext.gradientShader.setUniform("imprintYOffset", updateRect.y);
    renderContext.gradientShader.setUniform("imprintParams", renderContext.imprintParams);
    renderContext.readTexture->bind(GL_TEXTURE0);
    renderContext.targetTexture.bind(GL_TEXTURE1);
    renderContext.imprintTexture.bind(GL_TEXTURE2);
    renderContext.gradientTexture.bindImage(0);
    renderContext.gradientShader.dispatch(updateRect.w, updateRect.h, 1);

    // Reduction
    renderContext.gradientReductionShader.use();
    renderContext.gradientTexture.bind(GL_TEXTURE0);
    for (int level=0; level<renderContext.nLevels-1; ++level) {
        // Downscaled update area
        Rectangle levelRect(updateRect);
        int d = 2 << level;
        levelRect.x = levelRect.x / d - 0;
        levelRect.y = levelRect.y / d - 0;
        levelRect.w = std::ceil(levelRect.w / (float)d) + 1;
        levelRect.h = std::ceil(levelRect.h / (float)d) + 1;
        levelRect.clamp(0, renderContext.width / d, 0, renderContext.height / d);

        renderContext.gradientTexture.bindImage(0, level+1);
        renderContext.gradientReductionShader.setUniform("level", level);
        renderContext.gradientReductionShader.setUniform("xOffset", levelRect.x);
        renderContext.gradientReductionShader.setUniform("yOffset", levelRect.y);
        renderContext.gradientReductionShader.dispatch(levelRect.w, levelRect.h, 1);
    }

    // Fetch gradient
    glGetTexImage(GL_TEXTURE_2D_ARRAY, renderContext.nLevels-1, GL_RED, GL_FLOAT, renderContext.gradient.data());
}

void renderImprint(RenderContext& renderContext)
{
    // Find update boundaries
    auto& updateRect = renderContext.updateArea.updateImprint(renderContext);

    // Imprint
    renderContext.imprintShader.use();
    renderContext.imprintShader.setUniform("width", renderContext.width);
    renderContext.imprintShader.setUniform("height", renderContext.height);
    renderContext.imprintShader.setUniform("imprintTextureWidth", renderContext.imprintTextureWidth);
    renderContext.imprintShader.setUniform("imprintTextureHeight", renderContext.imprintTextureHeight);
    renderContext.imprintShader.setUniform("imprintXOffset", updateRect.x);
    renderContext.imprintShader.setUniform("imprintYOffset", updateRect.y);
    renderContext.imprintShader.setUniform("imprintParams", renderContext.imprintParams);

    renderContext.readTexture->bind(GL_TEXTURE0);
    renderContext.imprintTexture.bind(GL_TEXTURE1);
    renderContext.writeTexture->bindImage(0);
    renderContext.imprintShader.dispatch(updateRect.w, updateRect.h, 1);
}

void randomizeImprintParams(RenderContext& renderContext)
{
    renderContext.imprintParams[0] = rnd()*renderContext.width;
    renderContext.imprintParams[1] = rnd()*renderContext.height;
    renderContext.imprintParams[2] = 0.1+rnd()*0.4;
    renderContext.imprintParams[3] = PI*2.0*rnd();
    renderContext.imprintParams[4] = rnd();
    renderContext.imprintParams[5] = rnd();
    renderContext.imprintParams[6] = rnd();
    renderContext.imprintParams[7] = 0.9;
}
