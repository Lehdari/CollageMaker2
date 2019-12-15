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


#define RND (((int)(rnd()%20001)-10000)*0.0001)


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
    static std::default_random_engine rnd(2347865);
    bool swap = false;

    // Render geometry
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float newImprintX = renderContext.imprintX + RND*4.0f;
    float newImprintY = renderContext.imprintY + RND*4.0f;
    float newImprintScale = renderContext.imprintScale * (0.99+(RND*0.02f));
    float newImprintAngle = renderContext.imprintAngle + RND*0.04f-0.02f;

    // Imgui
    {
        ImGui::Begin("Imprint");
        ImGui::Text("error: %0.5f", renderContext.error);
        ImGui::Text("newImprintScale: %0.5f", newImprintScale);
        ImGui::ColorEdit4("Color", renderContext.imprintColor.data());
        ImGui::SliderFloat("X", &renderContext.imprintX, 0.0f, 4096.0f, "%.3f");
        ImGui::SliderFloat("Y", &renderContext.imprintY, 0.0f, 4096.0f, "%.3f");
        ImGui::SliderFloat("Scale", &renderContext.imprintScale, 0.0f, 8.0f, "%.3f");
        ImGui::SliderFloat("Angle", &renderContext.imprintAngle, 0.0f, 6.2831f, "%.3f");
        if (ImGui::Button("Imprint"))
            swap = true;
        ImGui::End();
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

    renderContext.imprintShader.setUniform("imprintX", newImprintX);
    renderContext.imprintShader.setUniform("imprintY", newImprintY);
    renderContext.imprintShader.setUniform("imprintScale", newImprintScale);
    renderContext.imprintShader.setUniform("imprintAngle", newImprintAngle);

    renderContext.readTexture->bind(GL_TEXTURE0);
    renderContext.imprintTexture.bind(GL_TEXTURE1);

    renderContext.writeTexture->bindImage(0);
    renderContext.imprintShader.dispatch(renderContext.width, renderContext.height, 1);

    // Error texture
    renderContext.errorShader.use();
    renderContext.writeTexture->bind(GL_TEXTURE0);
    renderContext.targetTexture.bind(GL_TEXTURE1);
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
    // Fetch error
    float newError = renderContext.error;
    glGetTexImage(GL_TEXTURE_2D, renderContext.nLevels-1, GL_RED, GL_FLOAT, &newError);
    if (newError < renderContext.error) {
        renderContext.error = newError;
        renderContext.imprintX = newImprintX;
        renderContext.imprintY = newImprintY;
        renderContext.imprintScale = newImprintScale;
        renderContext.imprintAngle = newImprintAngle;
    }

    // Draw
    renderContext.drawShader.use();
    renderContext.errorTexture.bind(GL_TEXTURE0);
    renderContext.quad.render(renderContext.drawShader, renderContext.camera);

    // swap texture read/write
    if (swap)
        std::swap(renderContext.readTexture, renderContext.writeTexture);
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
    renderContext.errorShader.addUniform("texCurrent");
    renderContext.errorShader.setUniform("texCurrent", 0);
    renderContext.errorShader.addUniform("texTarget");
    renderContext.errorShader.setUniform("texTarget", 1);

    renderContext.reductionShader.use();
    renderContext.reductionShader.addUniform("level");
    renderContext.reductionShader.addUniform("texError");
    renderContext.reductionShader.setUniform("texError", 0);

    // Camera
    gut::Camera::Settings cameraSettings;
    cameraSettings.aspectRatio = (float)appSettings.window.width/(float)appSettings.window.height;

    // Textures
    renderContext.targetTexture.loadFromFile(std::string(RES_DIR)+"textures/kekkonen1024.jpg", GL_RGBA32F);
    renderContext.targetTexture.setFiltering(GL_NEAREST, GL_NEAREST);
    renderContext.width = renderContext.targetTexture.width();
    renderContext.height = renderContext.targetTexture.height();
    renderContext.nLevels = 0;
    for (int i=renderContext.width; i>0; i = i >> 1, ++renderContext.nLevels);
    renderContext.error = FLT_MAX;

    renderContext.imprintTexture.loadFromFile(std::string(RES_DIR)+"textures/brush1.png", GL_RGBA32F);
    renderContext.imprintTexture.setFiltering(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    renderContext.imprintTexture.setWrapping(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
    renderContext.imprintWidth = renderContext.imprintTexture.width();
    renderContext.imprintHeight = renderContext.imprintTexture.height();
    renderContext.imprintX = 512.0f;
    renderContext.imprintY = 512.0f;
    renderContext.imprintScale = 1.0f;
    renderContext.imprintAngle = 0.0f;
    renderContext.imprintX = 912.0f;
    renderContext.imprintY = 350.0f;
    renderContext.imprintScale = 2.27f;
    renderContext.imprintAngle = 1.45f;
    renderContext.imprintColor << 1.0f, 1.0f, 1.0f, 1.0f;

    renderContext.errorTexture.create(renderContext.width, renderContext.height, GL_RGBA32F);
    renderContext.errorTexture.setFiltering(GL_NEAREST, GL_NEAREST);

    renderContext.texture1.create(renderContext.width, renderContext.height, GL_RGBA32F);
    renderContext.texture1.setFiltering(GL_NEAREST, GL_NEAREST);
    renderContext.texture2.create(renderContext.width, renderContext.height, GL_RGBA32F);
    renderContext.texture2.setFiltering(GL_NEAREST, GL_NEAREST);

    // Enter application loop
    app.loop();

    return 0;
}
