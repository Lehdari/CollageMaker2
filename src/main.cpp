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

#include <iostream> // TEMP

#define NO_STDIO_REDIRECT


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
    // Render geometry
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderContext.imprintShader.use();
    renderContext.readTexture->bind(GL_TEXTURE0);
    renderContext.writeTexture->bindImage(0);
    renderContext.imprintShader.dispatch(512, 512, 1);

    renderContext.drawShader.use();
    renderContext.writeTexture->bind(GL_TEXTURE0);
    renderContext.quad.render(renderContext.drawShader, renderContext.camera);

    // swap texture read/write
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

    renderContext.imprintShader.addUniform("tex");
    renderContext.imprintShader.setUniform("tex", 0);

    // Camera
    gut::Camera::Settings cameraSettings;
    cameraSettings.aspectRatio = (float)appSettings.window.width/(float)appSettings.window.height;

    // Textures
    renderContext.texture1.loadFromFile(std::string(RES_DIR)+"textures/voronoise.png", GL_RGBA32F);
    renderContext.texture2.loadFromFile(std::string(RES_DIR)+"textures/binarynoise.png", GL_RGBA32F);

    app.loop();

    return 0;
}
