//
// Project: CollageMaker2
// File: VS_Quad.glsl
//
// Copyright (c) 2019 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENSE which is distributed
// with this source code package.
//

#version 420

layout(location = 0) in vec3 position;

out vec2 pos;
out vec3 col;

uniform mat4 objectToWorld;
uniform mat4 worldToClip;

void main() {
    pos = 0.5*(objectToWorld * vec4(position, 1.0)).xy + vec2(0.5, 0.5);
    gl_Position = worldToClip * objectToWorld * vec4(position, 1.0);
}
