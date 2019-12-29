//
// Project: CollageMaker2
// File: CS_Error.glsl
//
// Copyright (c) 2019 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENSE which is distributed
// with this source code package.
//

#version 430

layout(local_size_x = 1, local_size_y = 1) in;
layout(r32f, binding = 0) uniform image2D img_output;

// Imprint parameters
uniform int   imprintTextureWidth;
uniform int   imprintTextureHeight;
uniform float imprintParams[8];

// Textures
uniform sampler2D texCurrent;
uniform sampler2D texTarget;
uniform sampler2D texImprint;

// Epsilons for gradient for different impainting parameters
const float gEps[8] = {
    0.5, 0.5, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001
};

// Color of imprinted pixel
vec4 imprint(vec2 cp, vec4 cPixel, float params[8])
{
    // Sample imprint texture
    mat3 itt = mat3(1.0);
    itt[2][0] = -0.5*imprintTextureWidth;
    itt[2][1] = -0.5*imprintTextureHeight;

    mat3 itr = mat3(1.0);
    itr[0][0] = cos(params[3]);
    itr[1][0] = sin(params[3]);
    itr[0][1] = -sin(params[3]);
    itr[1][1] = cos(params[3]);

    mat3 it = mat3(1.0);
    it[0][0] = params[2];
    it[1][1] = params[2];
    it[2][0] = params[0];
    it[2][1] = params[1];

    vec3 ip = inverse(it*itr*itt)*vec3(cp.xy, 1.0);
    vec4 iPixel = texture(texImprint, vec2(ip.x/imprintTextureWidth, ip.y/imprintTextureHeight))
    *vec4(params[4], params[5], params[6], params[7]);

    return vec4((1.0-iPixel.a)*cPixel.rgb + iPixel.a*iPixel.rgb, 1.0);
}

void main() {
    ivec2 p = ivec2(gl_GlobalInvocationID.xy);
    vec2 cp = p + vec2(0.5, 0.5);
    vec4 cPixel = texelFetch(texCurrent, p, 0); // current pixel
    vec4 tPixel = texelFetch(texTarget, p, 0); // target pixel

    vec4 nPixel = imprint(cp, cPixel, imprintParams); // imprinted pixel

    // error
    vec4 diff = nPixel - tPixel;
    float e = dot(diff, diff);
    imageStore(img_output, ivec2(gl_GlobalInvocationID.xy), vec4(e, 0.0, 0.0, 0.0));
}
