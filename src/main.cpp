//
// Project: CollageMaker2
// File: main.cpp
//
// Copyright (c) 2019 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENSE which is distributed
// with this source code package.
//

#include <OpenGLUtils/App.hpp>
#include "RenderContext.hpp"
#include "Quad.hpp"
#include "RenderUtils.hpp"

#include <random>

#include <iostream> // TEMP


#define RND (rnd())


using A_App = gut::App<RenderContext>;


// Pipeline function for the event handling
void handleEvents(SDL_Event& event, A_App::Context& appContext)
{
    // Handle SDL events
    switch (event.type) {
        case SDL_QUIT:
            *appContext.quit = true;
            break;

        case SDL_KEYDOWN:
            // Skip events if imgui widgets are being modified
            if (ImGui::IsAnyItemActive())
                return;
            switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    *appContext.quit = true;
                    break;

                default:
                    break;
            }
            break;

        default:
            break;
    }
}

// Pipeline function for rendering
void render(RenderContext& renderContext, A_App::Context& appContext)
{
    bool swap = false;

    // Render geometry
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderError(renderContext);
    double gScale =
        renderContext.imprintParams[2]*
        renderContext.imprintParams[2]*
        renderContext.imprintRatio;
    float gdRate = 3.0+0.015/gScale;
    static float pcError = renderContext.pError; // previous cycle error
    float diff = pcError - renderContext.error;
    pcError = renderContext.pError;
    double diffLimit = 0.00001*gScale;

    // Logging
    static double tCum = renderContext.clock();
    tCum += renderContext.clock();
    renderContext.log.addRow(tCum, renderContext.error);

    // Detect if error difference is small enough
    if (diff > -1.0e-12 && diff < diffLimit) {
        swap = true;
    } else {
        for (int i=0; i<1; ++i) {
            // Gradient descent
            renderGradient(renderContext);

            for (int j = 0; j < 8; ++j) {
                // momentum filtering for the gradient
                renderContext.fGradient[j] =
                    (1.0f - renderContext.fRatio) * renderContext.gradient[j] +
                    renderContext.fRatio * renderContext.fGradient[j];
            }

            // line search parameters
            double lineSearch = 1.0;
            constexpr double lineSearchRatio = 1.1;

            auto prevImprintParams = renderContext.imprintParams;
            do {
                prevImprintParams = renderContext.imprintParams;

                // update imprint parameters
                for (int j = 0; j < 8; ++j) {
                    renderContext.imprintParams[j] -=
                        gdRate * lineSearch * renderContext.gdRateMod[j] *
                        renderContext.fGradient[j];
                }

                renderContext.imprintParams[2] = std::clamp(renderContext.imprintParams[2], 0.1f, 1.0f);
                renderContext.imprintParams[4] = std::clamp(renderContext.imprintParams[4], 0.0f, 1.0f);
                renderContext.imprintParams[5] = std::clamp(renderContext.imprintParams[5], 0.0f, 1.0f);
                renderContext.imprintParams[6] = std::clamp(renderContext.imprintParams[6], 0.0f, 1.0f);
                renderContext.imprintParams[7] = std::clamp(renderContext.imprintParams[7], 0.75f, 1.0f);

                // find error for the update
                renderError(renderContext);

                // advance exponentially in line search
                lineSearch *= lineSearchRatio;
            } while (renderContext.pError > renderContext.error);

            // use previous params (last step increased the error)
            // but in case the first step increased the error, the algorithm will
            // get stuck, therefore at least one step is to be performed
            if (lineSearch > lineSearchRatio*1.000001)
                renderContext.imprintParams = prevImprintParams;
        }
    }

    // Imprint
    renderImprint(renderContext);

    // Draw
    renderContext.drawShader.use();
    renderContext.writeTexture->bind(GL_TEXTURE0);
    renderContext.quad.render(renderContext.drawShader, renderContext.camera);

    // Imgui
    {
        ImGui::Begin("Imprint");
        ImGui::Text("error: %0.10f %0.10f %0.10f", renderContext.error,
            diff/diffLimit, gdRate);
        ImGui::ColorEdit4("Color", &renderContext.imprintParams[4]);
        ImGui::SliderFloat("X", &renderContext.imprintParams[0], 0.0f, 1024.0f, "%.3f");
        ImGui::SliderFloat("Y", &renderContext.imprintParams[1], 0.0f, 1024.0f, "%.3f");
        ImGui::SliderFloat("Scale", &renderContext.imprintParams[2], 0.0f, 8.0f, "%.3f");
        ImGui::SliderFloat("Angle", &renderContext.imprintParams[3], 0.0f, 6.2831f, "%.3f");
        if (ImGui::Button("Imprint"))
            swap = true;
        ImGui::End();
    }

    if (swap) {
        // swap texture read/write
        std::swap(renderContext.readTexture, renderContext.writeTexture);

        // generate new imprint position
        randomizeImprintParams(renderContext);
        auto minImprintParams = renderContext.imprintParams;
        renderError(renderContext);
        float minError = renderContext.error;
        float firstError = minError;

        // try different positions, pick the best
        for (int i=0; i<100; ++i) {
            randomizeImprintParams(renderContext);
            renderError(renderContext);
            if (renderContext.error < minError) {
                minImprintParams = renderContext.imprintParams;
                minError = renderContext.error;
            }
        }

        renderContext.imprintParams = minImprintParams;
        renderContext.gradient.fill(0.0f);
        renderContext.fGradient.fill(0.0f);

        //renderContext.imprintParams[3] = atan2(512.0-renderContext.imprintParams[1], renderContext.imprintParams[0]-512.0)+0.5*PI;

        renderContext.error = -1.0;
        renderContext.pError = -1.0;
        renderContext.nIters = 0;
    }
}


int main(int argc, char** argv)
{
    // App settings
    A_App::Settings appSettings;
    appSettings.window.width = 1024;
    appSettings.window.height = 1024;
    appSettings.handleEvents = &handleEvents;
    appSettings.render = &render;
    appSettings.window.framerateLimit = 1000;

    RenderContext renderContext;
    // Create app (required here because it calls gl3wInit)
    A_App app(appSettings, &renderContext);

    // Quad
    renderContext.quad.loadFromVertexData(quadPositions, 4, nullptr, 0, quadIndices, 6);

    // Shaders
    try {
        renderContext.drawShader.load(
            std::string(RES_DIR)+"shaders/VS_Quad.glsl",
            std::string(RES_DIR)+"shaders/FS_Quad.glsl");

        renderContext.imprintShader.load(
            std::string(RES_DIR)+"shaders/CS_Imprint.glsl", GL_COMPUTE_SHADER);

        renderContext.errorShader.load(
            std::string(RES_DIR)+"shaders/CS_Error.glsl", GL_COMPUTE_SHADER);

        renderContext.gradientShader.load(
            std::string(RES_DIR)+"shaders/CS_Gradient.glsl", GL_COMPUTE_SHADER);

        renderContext.errorReductionShader.load(
            std::string(RES_DIR)+"shaders/CS_ReduceError.glsl", GL_COMPUTE_SHADER);

        renderContext.gradientReductionShader.load(
            std::string(RES_DIR)+"shaders/CS_ReduceGradient.glsl", GL_COMPUTE_SHADER);
    }
    catch (char* e) {
        printf("%s\n", e);
        return 0;
    }

    renderContext.drawShader.use();
    renderContext.drawShader.addUniform("objectToWorld");
    renderContext.drawShader.addUniform("worldToClip");
    renderContext.drawShader.addUniform("tex");
    renderContext.drawShader.setUniform("tex", 0);

    renderContext.imprintShader.use();
    renderContext.imprintShader.addUniform("width");
    renderContext.imprintShader.addUniform("height");
    renderContext.imprintShader.addUniform("imprintTextureWidth");
    renderContext.imprintShader.addUniform("imprintTextureHeight");
    renderContext.imprintShader.addUniform("imprintXOffset");
    renderContext.imprintShader.addUniform("imprintYOffset");
    renderContext.imprintShader.addUniform("imprintParams");
    renderContext.imprintShader.addUniform("texCurrent");
    renderContext.imprintShader.setUniform("texCurrent", 0);
    renderContext.imprintShader.addUniform("texImprint");
    renderContext.imprintShader.setUniform("texImprint", 1);

    renderContext.errorShader.use();
    renderContext.errorShader.addUniform("imprintTextureWidth");
    renderContext.errorShader.addUniform("imprintTextureHeight");
    renderContext.errorShader.addUniform("imprintXOffset");
    renderContext.errorShader.addUniform("imprintYOffset");
    renderContext.errorShader.addUniform("imprintParams");
    renderContext.errorShader.addUniform("texCurrent");
    renderContext.errorShader.setUniform("texCurrent", 0);
    renderContext.errorShader.addUniform("texTarget");
    renderContext.errorShader.setUniform("texTarget", 1);
    renderContext.errorShader.addUniform("texImprint");
    renderContext.errorShader.setUniform("texImprint", 2);

    renderContext.gradientShader.use();
    renderContext.gradientShader.addUniform("imprintTextureWidth");
    renderContext.gradientShader.addUniform("imprintTextureHeight");
    renderContext.gradientShader.addUniform("imprintXOffset");
    renderContext.gradientShader.addUniform("imprintYOffset");
    renderContext.gradientShader.addUniform("imprintParams");
    renderContext.gradientShader.addUniform("texCurrent");
    renderContext.gradientShader.setUniform("texCurrent", 0);
    renderContext.gradientShader.addUniform("texTarget");
    renderContext.gradientShader.setUniform("texTarget", 1);
    renderContext.gradientShader.addUniform("texImprint");
    renderContext.gradientShader.setUniform("texImprint", 2);

    renderContext.errorReductionShader.use();
    renderContext.errorReductionShader.addUniform("level");
    renderContext.errorReductionShader.addUniform("xOffset");
    renderContext.errorReductionShader.addUniform("yOffset");
    renderContext.errorReductionShader.addUniform("texError");
    renderContext.errorReductionShader.setUniform("texError", 0);

    renderContext.gradientReductionShader.use();
    renderContext.gradientReductionShader.addUniform("level");
    renderContext.gradientReductionShader.addUniform("xOffset");
    renderContext.gradientReductionShader.addUniform("yOffset");
    renderContext.gradientReductionShader.addUniform("texGradient");
    renderContext.gradientReductionShader.setUniform("texGradient", 0);

    // Camera
    gut::Camera::Settings cameraSettings;
    cameraSettings.aspectRatio = (float)appSettings.window.width/(float)appSettings.window.height;

    // Textures
    renderContext.targetTexture.loadFromFile(std::string(RES_DIR)+"textures/kekkonen1024.jpg");
    renderContext.targetTexture.setFiltering(GL_NEAREST, GL_NEAREST);
    renderContext.width = renderContext.targetTexture.width();
    renderContext.height = renderContext.targetTexture.height();
    renderContext.nLevels = 0;
    for (int i=renderContext.width; i>0; i = i >> 1, ++renderContext.nLevels);
    renderContext.error = -1.0f;
    renderContext.gradient.fill(0.0f);
    renderContext.fGradient.fill(0.0f);
    renderContext.fRatio = 0.8f;
    renderContext.gdRateMod.fill(1.0f);
    renderContext.gdRateMod[0] = 1000.0f;
    renderContext.gdRateMod[1] = 1000.0f;
    renderContext.pError = -1.0f;
    renderContext.nIters = 0;

    renderContext.imprintTexture.loadFromFile(std::string(RES_DIR)+"textures/brush1.png");
    renderContext.imprintTexture.setFiltering(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    renderContext.imprintTexture.setWrapping(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
    renderContext.imprintTextureWidth = renderContext.imprintTexture.width();
    renderContext.imprintTextureHeight = renderContext.imprintTexture.height();

    renderContext.imprintRatio =
        (double)(renderContext.imprintTextureWidth*renderContext.imprintTextureHeight)/
        (double)(renderContext.width*renderContext.height);

    randomizeImprintParams(renderContext);

    renderContext.errorTexture.create(renderContext.width, renderContext.height);
    renderContext.errorTexture.setFiltering(GL_NEAREST, GL_NEAREST);
    renderContext.gradientTexture.create(renderContext.width, renderContext.height, 8);
    renderContext.gradientTexture.setFiltering(GL_NEAREST, GL_NEAREST);

    renderContext.texture1.create(renderContext.width, renderContext.height);
    renderContext.texture1.setFiltering(GL_NEAREST, GL_NEAREST);
    renderContext.texture2.create(renderContext.width, renderContext.height);
    renderContext.texture2.setFiltering(GL_NEAREST, GL_NEAREST);

    renderContext.updateArea = UpdateArea(renderContext.width, renderContext.height);

    // Enter application loop
    app.loop();

    return 0;
}
