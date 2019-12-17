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
    double gScale = renderContext.imprintScale*renderContext.imprintScale*renderContext.imprintRatio;
    float diff = renderContext.pError - renderContext.error;

    // Detect if error difference is small enough
    double diffLimit = 0.0001*gScale;
    if (diff > -1.0e-8 && diff < diffLimit) {
        swap = true;
    } else {
        // Gradient descent
        renderGradient(renderContext);

        float gdRate = std::clamp(0.75+0.1/gScale, 0.0, 8.0);
        renderContext.imprintX -= gdRate*1000.0*renderContext.gradient[0];
        renderContext.imprintY -= gdRate*1000.0*renderContext.gradient[1];
        renderContext.imprintScale -= gdRate*renderContext.gradient[2];
        renderContext.imprintAngle -= gdRate*renderContext.gradient[3];
        renderContext.imprintColor[0] -= gdRate*renderContext.gradient[4];
        renderContext.imprintColor[1] -= gdRate*renderContext.gradient[5];
        renderContext.imprintColor[2] -= gdRate*renderContext.gradient[6];

        renderContext.imprintScale = std::clamp(renderContext.imprintScale, 0.1f, 1.0f);
        renderContext.imprintColor = renderContext.imprintColor.cwiseMax(0.0).cwiseMin(1.0);
    }

    // Imprint
    renderContext.imprintShader.use();
    renderContext.imprintShader.setUniform("width", renderContext.width);
    renderContext.imprintShader.setUniform("height", renderContext.height);
    renderContext.imprintShader.setUniform("imprintWidth", renderContext.imprintWidth);
    renderContext.imprintShader.setUniform("imprintHeight", renderContext.imprintHeight);

    renderContext.imprintShader.setUniform("imprintX", renderContext.imprintX);
    renderContext.imprintShader.setUniform("imprintY", renderContext.imprintY);
    renderContext.imprintShader.setUniform("imprintScale", renderContext.imprintScale);
    renderContext.imprintShader.setUniform("imprintAngle", renderContext.imprintAngle);
    renderContext.imprintShader.setUniform("imprintColor", renderContext.imprintColor);

    renderContext.readTexture->bind(GL_TEXTURE0);
    renderContext.imprintTexture.bind(GL_TEXTURE1);

    renderContext.writeTexture->bindImage(0);
    renderContext.imprintShader.dispatch(renderContext.width, renderContext.height, 1);

    // Draw
    renderContext.drawShader.use();
    renderContext.writeTexture->bind(GL_TEXTURE0);
    //renderContext.errorTexture.bind(GL_TEXTURE0);
    renderContext.quad.render(renderContext.drawShader, renderContext.camera);

    // Imgui
    {
        ImGui::Begin("Imprint");
        ImGui::Text("error: %0.10f %0.10f %0.10f %0.10f", renderContext.error, diff,
            diffLimit, diff/diffLimit);
        ImGui::ColorEdit4("Color", renderContext.imprintColor.data());
        ImGui::SliderFloat("X", &renderContext.imprintX, 0.0f, 1024.0f, "%.3f");
        ImGui::SliderFloat("Y", &renderContext.imprintY, 0.0f, 1024.0f, "%.3f");
        ImGui::SliderFloat("Scale", &renderContext.imprintScale, 0.0f, 8.0f, "%.3f");
        ImGui::SliderFloat("Angle", &renderContext.imprintAngle, 0.0f, 6.2831f, "%.3f");
        if (ImGui::Button("Imprint"))
            swap = true;
        ImGui::End();
    }

    // swap texture read/write
    if (swap) {
        std::swap(renderContext.readTexture, renderContext.writeTexture);

        renderContext.imprintX = RND*renderContext.width;
        renderContext.imprintY = RND*renderContext.height;
        renderContext.imprintScale = 0.1 + RND*0.4;
        //renderContext.imprintAngle = PI*2.0*RND;
        renderContext.imprintAngle = atan2(512.0-renderContext.imprintY, renderContext.imprintX-512.0)+0.5*PI;
        renderContext.imprintColor << RND, RND, RND, 1.0;

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
    renderContext.imprintShader.addUniform("imprintWidth");
    renderContext.imprintShader.addUniform("imprintHeight");
    renderContext.imprintShader.addUniform("imprintX");
    renderContext.imprintShader.addUniform("imprintY");
    renderContext.imprintShader.addUniform("imprintScale");
    renderContext.imprintShader.addUniform("imprintAngle");
    renderContext.imprintShader.addUniform("imprintColor");
    renderContext.imprintShader.addUniform("texCurrent");
    renderContext.imprintShader.setUniform("texCurrent", 0);
    renderContext.imprintShader.addUniform("texImprint");
    renderContext.imprintShader.setUniform("texImprint", 1);

    renderContext.errorShader.use();
    renderContext.errorShader.addUniform("imprintWidth");
    renderContext.errorShader.addUniform("imprintHeight");
    renderContext.errorShader.addUniform("imprintX");
    renderContext.errorShader.addUniform("imprintY");
    renderContext.errorShader.addUniform("imprintScale");
    renderContext.errorShader.addUniform("imprintAngle");
    renderContext.errorShader.addUniform("imprintColor");
    renderContext.errorShader.addUniform("texCurrent");
    renderContext.errorShader.setUniform("texCurrent", 0);
    renderContext.errorShader.addUniform("texTarget");
    renderContext.errorShader.setUniform("texTarget", 1);
    renderContext.errorShader.addUniform("texImprint");
    renderContext.errorShader.setUniform("texImprint", 2);

    renderContext.gradientShader.use();
    renderContext.gradientShader.addUniform("imprintWidth");
    renderContext.gradientShader.addUniform("imprintHeight");
    renderContext.gradientShader.addUniform("imprintX");
    renderContext.gradientShader.addUniform("imprintY");
    renderContext.gradientShader.addUniform("imprintScale");
    renderContext.gradientShader.addUniform("imprintAngle");
    renderContext.gradientShader.addUniform("imprintColor");
    renderContext.gradientShader.addUniform("texCurrent");
    renderContext.gradientShader.setUniform("texCurrent", 0);
    renderContext.gradientShader.addUniform("texTarget");
    renderContext.gradientShader.setUniform("texTarget", 1);
    renderContext.gradientShader.addUniform("texImprint");
    renderContext.gradientShader.setUniform("texImprint", 2);

    renderContext.errorReductionShader.use();
    renderContext.errorReductionShader.addUniform("level");
    renderContext.errorReductionShader.addUniform("texError");
    renderContext.errorReductionShader.setUniform("texError", 0);

    renderContext.gradientReductionShader.use();
    renderContext.gradientReductionShader.addUniform("level");
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
    renderContext.pError = -1.0f;
    renderContext.nIters = 0;

    renderContext.imprintTexture.loadFromFile(std::string(RES_DIR)+"textures/brush1.png");
    renderContext.imprintTexture.setFiltering(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    renderContext.imprintTexture.setWrapping(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
    renderContext.imprintWidth = renderContext.imprintTexture.width();
    renderContext.imprintHeight = renderContext.imprintTexture.height();

    renderContext.imprintRatio =
        (double)(renderContext.imprintWidth*renderContext.imprintHeight)/
        (double)(renderContext.width*renderContext.height);

    renderContext.imprintX = RND*renderContext.width;
    renderContext.imprintY = RND*renderContext.height;
    renderContext.imprintScale = 0.1 + RND*0.4;
    renderContext.imprintAngle = PI*2.0*RND;
    renderContext.imprintColor << RND, RND, RND, 1.0;

    renderContext.errorTexture.create(renderContext.width, renderContext.height);
    renderContext.errorTexture.setFiltering(GL_NEAREST, GL_NEAREST);
    renderContext.gradientTexture.create(renderContext.width, renderContext.height, 7);
    renderContext.gradientTexture.setFiltering(GL_NEAREST, GL_NEAREST);

    renderContext.texture1.create(renderContext.width, renderContext.height);
    renderContext.texture1.setFiltering(GL_NEAREST, GL_NEAREST);
    renderContext.texture2.create(renderContext.width, renderContext.height);
    renderContext.texture2.setFiltering(GL_NEAREST, GL_NEAREST);

    // Enter application loop
    app.loop();

    return 0;
}
