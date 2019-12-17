//
// Project: CollageMaker2
// File: CS_ReduceGradient.glsl
//
// Copyright (c) 2019 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENSE which is distributed
// with this source code package.
//

#version 430

layout(local_size_x = 1, local_size_y = 1) in;
layout(r32f, binding = 0) uniform image2DArray img_output;

uniform int level;

// Textures
uniform sampler2DArray texGradient;

void main() {
    ivec2 p2 = ivec2(gl_GlobalInvocationID.xy)*2;

    for (int i=0; i<7; ++i) {
        vec4 gp;
        gp[0] = texelFetch(texGradient, ivec3(p2, i), level).r;
        gp[1] = texelFetch(texGradient, ivec3(p2+ivec2(1, 0), i), level).r;
        gp[2] = texelFetch(texGradient, ivec3(p2+ivec2(0, 1), i), level).r;
        gp[3] = texelFetch(texGradient, ivec3(p2+ivec2(1, 1), i), level).r;

        float g = (gp[0] + gp[1] + gp[2] + gp[3])*0.25;

        imageStore(img_output, ivec3(gl_GlobalInvocationID.xy, i), vec4(g, 0.0, 0.0, 0.0));
    }

}
