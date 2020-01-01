//
// Project: CollageMaker2
// File: RenderUtils.hpp
//
// Copyright (c) 2019 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENSE which is distributed
// with this source code package.
//

#ifndef COLLAGEMAKER2_RENDERUTILS_HPP
#define COLLAGEMAKER2_RENDERUTILS_HPP


#include "RenderContext.hpp"
#include "Rectangle.hpp"


// Calculate rendering error
void renderError(RenderContext& renderContext);

// Calculate rendering gradient
void renderGradient(RenderContext& renderContext);

// Imprint
void renderImprint(RenderContext& renderContext);

// Generate new imprint parameter vector
void randomizeImprintParams(RenderContext& renderContext);


#endif //COLLAGEMAKER2_RENDERUTILS_HPP
