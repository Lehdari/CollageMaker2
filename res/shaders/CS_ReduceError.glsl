//
// Project: CollageMaker2
// File: CS_ReduceError.glsl
//
// Copyright (c) 2019 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENSE which is distributed
// with this source code package.
//

#version 430

layout(local_size_x = 1, local_size_y = 1) in;
layout(r32f, binding = 0) uniform image2D img_output;

uniform int level;

// Textures
uniform sampler2D texError;

void main() {
    ivec2 p2 = ivec2(gl_GlobalInvocationID.xy)*2;

    float e = (
        texelFetch(texError, p2, level).r +
        texelFetch(texError, p2+ivec2(1, 0), level).r +
        texelFetch(texError, p2+ivec2(0, 1), level).r +
        texelFetch(texError, p2+ivec2(1, 1), level).r)*0.25;

    imageStore(img_output, ivec2(gl_GlobalInvocationID.xy), vec4(e, 0.0, 0.0, 0.0));
}
