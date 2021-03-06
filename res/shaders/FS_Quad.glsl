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

uniform sampler2D tex;
//uniform sampler2DArray tex;

void main() {
    vec2 ppos = vec2(pos.x, 1.0-pos.y);
    color = vec4(texture(tex, ppos).rgb, 1.0);
    //color = vec4(texelFetch(tex, ivec3(ppos*256, 3), 2).rrr+vec3(0.5, 0.5, 0.5), 1.0);
}
