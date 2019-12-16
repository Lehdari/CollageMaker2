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

    // Error
    renderContext.errorShader.use();
    renderContext.errorShader.setUniform("imprintWidth", renderContext.imprintWidth);
    renderContext.errorShader.setUniform("imprintHeight", renderContext.imprintHeight);
    renderContext.errorShader.setUniform("imprintX", renderContext.imprintX);
    renderContext.errorShader.setUniform("imprintY", renderContext.imprintY);
    renderContext.errorShader.setUniform("imprintScale", renderContext.imprintScale);
    renderContext.errorShader.setUniform("imprintAngle", renderContext.imprintAngle);
    renderContext.errorShader.setUniform("imprintColor", renderContext.imprintColor);
    renderContext.readTexture->bind(GL_TEXTURE0);
    renderContext.targetTexture.bind(GL_TEXTURE1);
    renderContext.imprintTexture.bind(GL_TEXTURE2);
    renderContext.errorTexture.bindImage(0);
    renderContext.errorShader.dispatch(renderContext.width, renderContext.height, 1);

    // Reduction
    renderContext.reductionShader.use();
    renderContext.errorTexture.bind(GL_TEXTURE0);
    for (int level=0; level<renderContext.nLevels; ++level) {
        renderContext.errorTexture.bindImage(0, level+1);
        renderContext.reductionShader.setUniform("level", level);
        renderContext.reductionShader.dispatch(
            renderContext.width/(2 << level), renderContext.height/(2 << level), 1);
    }

    // Check whether the firstDiff is to be stored
    bool storeFirstDiff = false;
    if (renderContext.pError < 0.0 && renderContext.error[0] > 0.0)
        storeFirstDiff = true;

    // Fetch error and gradient
    renderContext.pError = renderContext.error[0];
    glGetTexImage(GL_TEXTURE_2D_ARRAY, renderContext.nLevels-1, GL_RED, GL_FLOAT, renderContext.error.data());
    float diff = renderContext.pError - renderContext.error[0];

    // Store first diff
    if (storeFirstDiff)
        renderContext.firstDiff = diff;

    // Detect if error difference is small enough
    if (renderContext.firstDiff > 0.0 && diff < renderContext.firstDiff*0.01) {
        swap = true;
    } else {
        // Gradient descent
        const float gdRate = 1.0;
        renderContext.imprintX -= gdRate*1000.0*renderContext.error[1];
        renderContext.imprintY -= gdRate*1000.0*renderContext.error[2];
        renderContext.imprintScale -= gdRate*renderContext.error[3];
        renderContext.imprintAngle -= gdRate*renderContext.error[4];
        renderContext.imprintColor[0] -= gdRate*renderContext.error[5];
        renderContext.imprintColor[1] -= gdRate*renderContext.error[6];
        renderContext.imprintColor[2] -= gdRate*renderContext.error[7];

        renderContext.imprintScale = std::clamp(renderContext.imprintScale, 0.1f, 4.0f);
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
        ImGui::Text("error: %0.5f %0.10f %0.10f %0.3f",
            renderContext.error[0], diff, renderContext.firstDiff,
            (diff/(renderContext.firstDiff*0.01)));
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
        renderContext.imprintAngle = PI*2.0*RND;
        renderContext.imprintColor << RND, RND, RND, 1.0;

        renderContext.error[0] = -1.0;
        renderContext.pError = -1.0;
        renderContext.firstDiff = -1.0;
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

        renderContext.reductionShader.load(
            std::string(RES_DIR)+"shaders/CS_Reduce.glsl", GL_COMPUTE_SHADER);
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

    renderContext.reductionShader.use();
    renderContext.reductionShader.addUniform("level");
    renderContext.reductionShader.addUniform("texError");
    renderContext.reductionShader.setUniform("texError", 0);

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
    renderContext.error.fill(0.0f);
    renderContext.error[0] = -1.0f;
    renderContext.pError = -1.0f;
    renderContext.firstDiff = -1.0f;

    renderContext.imprintTexture.loadFromFile(std::string(RES_DIR)+"textures/brush1.png");
    renderContext.imprintTexture.setFiltering(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    renderContext.imprintTexture.setWrapping(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
    renderContext.imprintWidth = renderContext.imprintTexture.width();
    renderContext.imprintHeight = renderContext.imprintTexture.height();

    renderContext.imprintX = RND*renderContext.width;
    renderContext.imprintY = RND*renderContext.height;
    renderContext.imprintScale = 0.1 + RND*0.4;
    renderContext.imprintAngle = PI*2.0*RND;
    renderContext.imprintColor << RND, RND, RND, 1.0;

    renderContext.errorTexture.create(renderContext.width, renderContext.height, 8);
    renderContext.errorTexture.setFiltering(GL_NEAREST, GL_NEAREST);

    renderContext.texture1.create(renderContext.width, renderContext.height);
    renderContext.texture1.setFiltering(GL_NEAREST, GL_NEAREST);
    renderContext.texture2.create(renderContext.width, renderContext.height);
    renderContext.texture2.setFiltering(GL_NEAREST, GL_NEAREST);

    // Enter application loop
    app.loop();

    return 0;
}
