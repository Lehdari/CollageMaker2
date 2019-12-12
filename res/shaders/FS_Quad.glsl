//
// Project: CollageMaker2
// File: FS_Quad.glsl
//
// Copyright (c) 2019 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENSE which is distributed
// with this source code package.
//

#version 420

in vec2 pos;
in vec3 col;

out vec4 color;

void main() {
    color = vec4(pos.x, pos.y, 0.0, 1.0);
}
