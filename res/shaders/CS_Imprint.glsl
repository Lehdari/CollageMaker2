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

// Target texture dimensions
uniform int width;
uniform int height;

// Imprint parameters
uniform int   imprintWidth;
uniform int   imprintHeight;
uniform float imprintX;
uniform float imprintY;
uniform float imprintScale;
uniform float imprintAngle;

// Textures
uniform sampler2D texCurrent;
uniform sampler2D texImprint;

// Constants
const float PI = 3.14159265359;

void main() {
    // Find current pixel location and value
    vec3 cp = vec3(gl_GlobalInvocationID.xy+vec2(0.5, 0.5), 1.0);
    vec3 cuv = vec3(cp.xy/vec2(width, height), 1.0);
    vec4 cPixel = texture(texCurrent, cuv.xy);

    // Sample imprint texture
    mat3 itt = mat3(1.0);
    itt[2][0] = -0.5;
    itt[2][1] = -0.5;

    mat3 itr = mat3(1.0);
    itr[0][0] = cos(imprintAngle);
    itr[1][0] = sin(imprintAngle);
    itr[0][1] = -sin(imprintAngle);
    itr[1][1] = cos(imprintAngle);

    mat3 it = mat3(1.0);
    it[0][0] = imprintScale;
    it[1][1] = imprintScale;
    it[2][0] = imprintX/imprintWidth;
    it[2][1] = imprintY/imprintHeight;

    vec3 iuv = vec3(cp.xy/vec2(imprintWidth, imprintHeight), 1.0);
    vec4 iPixel = texture(texImprint, (inverse(it*itr*itt)*iuv).xy);

    vec4 pixel = vec4((1.0-iPixel.a)*cPixel.rgb + iPixel.a*iPixel.rgb, 1.0);

    // output to a specific pixel in the image
    imageStore(img_output, ivec2(gl_GlobalInvocationID.xy), pixel);
}
