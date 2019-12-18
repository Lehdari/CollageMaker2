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
    // Bind uniforms
    renderContext.errorShader.use();
    renderContext.errorShader.setUniform("imprintWidth", renderContext.imprintWidth);
    renderContext.errorShader.setUniform("imprintHeight", renderContext.imprintHeight);
    renderContext.errorShader.setUniform("imprintParams", renderContext.imprintParams);
    renderContext.readTexture->bind(GL_TEXTURE0);
    renderContext.targetTexture.bind(GL_TEXTURE1);
    renderContext.imprintTexture.bind(GL_TEXTURE2);
    renderContext.errorTexture.bindImage(0);
    renderContext.errorShader.dispatch(renderContext.width, renderContext.height, 1);

    // Reduction
    renderContext.errorReductionShader.use();
    renderContext.errorTexture.bind(GL_TEXTURE0);
    for (int level=0; level<renderContext.nLevels; ++level) {
        renderContext.errorTexture.bindImage(0, level+1);
        renderContext.errorReductionShader.setUniform("level", level);
        renderContext.errorReductionShader.dispatch(
            renderContext.width/(2 << level), renderContext.height/(2 << level), 1);
    }

    // Fetch error
    renderContext.pError = renderContext.error;
    glGetTexImage(GL_TEXTURE_2D, renderContext.nLevels-1, GL_RED, GL_FLOAT, &renderContext.error);
}

void renderGradient(RenderContext& renderContext)
{
    // Bind uniforms
    renderContext.gradientShader.use();
    renderContext.gradientShader.setUniform("imprintWidth", renderContext.imprintWidth);
    renderContext.gradientShader.setUniform("imprintHeight", renderContext.imprintHeight);
    renderContext.errorShader.setUniform("imprintParams", renderContext.imprintParams);
    renderContext.readTexture->bind(GL_TEXTURE0);
    renderContext.targetTexture.bind(GL_TEXTURE1);
    renderContext.imprintTexture.bind(GL_TEXTURE2);
    renderContext.gradientTexture.bindImage(0);
    renderContext.gradientShader.dispatch(renderContext.width, renderContext.height, 1);

    // Reduction
    renderContext.gradientReductionShader.use();
    renderContext.gradientTexture.bind(GL_TEXTURE0);
    for (int level=0; level<renderContext.nLevels; ++level) {
        renderContext.gradientTexture.bindImage(0, level+1);
        renderContext.gradientReductionShader.setUniform("level", level);
        renderContext.gradientReductionShader.dispatch(
            renderContext.width/(2 << level), renderContext.height/(2 << level), 1);
    }

    // Fetch gradient
    glGetTexImage(GL_TEXTURE_2D_ARRAY, renderContext.nLevels-1, GL_RED, GL_FLOAT, renderContext.gradient.data());
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
}
