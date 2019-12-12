//
// Project: CollageMaker2
// File: Quad.hpp
//
// Copyright (c) 2019 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENSE which is distributed
// with this source code package.
//

#ifndef COLLAGEMAKER2_QUAD_HPP
#define COLLAGEMAKER2_QUAD_HPP


#include <cstdint>


const float quadPositions[] = {
    -1.0f,  -1.0f,  -1.0f,  1.0f,
    1.0f,   -1.0f,  -1.0f,  1.0f,
    -1.0f,  1.0f,   -1.0f,  1.0f,
    1.0f,   1.0f,   -1.0f,  1.0f
};

const uint32_t quadIndices[] = {
    0, 3, 2, 0, 1, 3
};


#endif //COLLAGEMAKER2_QUAD_HPP
