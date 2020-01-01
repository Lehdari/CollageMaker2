//
// Project: CollageMaker2
// File: UpdateArea.hpp
//
// Copyright (c) 2019 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENCE which is distributed
// with this source code package.
//

#ifndef COLLAGEMAKER2_UPDATEAREA_HPP
#define COLLAGEMAKER2_UPDATEAREA_HPP


#include "Rectangle.hpp"


class RenderContext;


class UpdateArea {
public:
    UpdateArea(int w = 0, int h = 0);

    const Rectangle& updateError(const RenderContext& rc);
    const Rectangle& updateGradient(const RenderContext& rc);
    const Rectangle& updateImprint(const RenderContext& rc);

    const Rectangle& errorRect() const;
    const Rectangle& gradientRect() const;
    const Rectangle& imprintRect() const;

private:
    // Rectangles indicating texture update areas
    Rectangle   _prevErrorRect;
    Rectangle   _prevGradientRect;
    Rectangle   _prevImprintRect;
    Rectangle   _updateErrorRect;
    Rectangle   _updateGradientRect;
    Rectangle   _updateImprintRect;

    float       _xMin;
    float       _xMax;
    float       _yMin;
    float       _yMax;

    // Update min/max values
    void newRect(const RenderContext& rc);
};


#endif //COLLAGEMAKER2_UPDATEAREA_HPP
