//
// Project: CollageMaker2
// File: CS_Reduce.glsl
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
uniform sampler2DArray texError;

void main() {
    ivec2 p2 = ivec2(gl_GlobalInvocationID.xy)*2;

    // Error
    float e = (
        texelFetch(texError, ivec3(p2, 0), level).r +
        texelFetch(texError, ivec3(p2+ivec2(1, 0), 0), level).r +
        texelFetch(texError, ivec3(p2+ivec2(0, 1), 0), level).r +
        texelFetch(texError, ivec3(p2+ivec2(1, 1), 0), level).r)*0.25;
    imageStore(img_output, ivec3(gl_GlobalInvocationID.xy, 0), vec4(e, 0.0, 0.0, 0.0));

    // Gradient
    for (int i=1; i<8; ++i) {
        vec4 gp;
        gp[0] = texelFetch(texError, ivec3(p2, i), level).r;
        gp[1] = texelFetch(texError, ivec3(p2+ivec2(1, 0), i), level).r;
        gp[2] = texelFetch(texError, ivec3(p2+ivec2(0, 1), i), level).r;
        gp[3] = texelFetch(texError, ivec3(p2+ivec2(1, 1), i), level).r;

        //float m = abs(gp[0]) + abs(gp[1]) + abs(gp[2]) + abs(gp[3]);
        float g = gp[0] + gp[1] + gp[2] + gp[3];
        //if (m > 0.0)
        //    g /= m;
        g *= 0.25;

        imageStore(img_output, ivec3(gl_GlobalInvocationID.xy, i), vec4(g, 0.0, 0.0, 0.0));
    }

}
