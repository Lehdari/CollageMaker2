//
// Project: CollageMaker2
// File: FS_Quad.glsl
//
// Copyright (c) 2019 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENSE which is distributed
// with this source code package.
//

#version 430

layout(local_size_x = 1, local_size_y = 1) in;
layout(rgba32f, binding = 0) uniform image2D img_output;

// Textures
uniform sampler2D texCurrent;
uniform sampler2D texTarget;

void main() {
    ivec2 p = ivec2(gl_GlobalInvocationID.xy);
    vec4 cPixel = texelFetch(texCurrent, p, 0);
    vec4 tPixel = texelFetch(texTarget, p, 0);

    vec4 diff = cPixel - tPixel;
    float e = dot(diff, diff);

    // output to a specific pixel in the image
    imageStore(img_output, ivec2(gl_GlobalInvocationID.xy), vec4(e, e, e, 1.0));
}
