//
// Project: CollageMaker2
// File: UpdateArea.cpp
//
// Copyright (c) 2019 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENCE which is distributed
// with this source code package.
//

#include "UpdateArea.hpp"
#include "RenderContext.hpp"


UpdateArea::UpdateArea(int w, int h) :
    _prevErrorRect      (0, 0, w, h),
    _prevGradientRect   (0, 0, w, h),
    _prevImprintRect    (0, 0, w, h),
    _updateErrorRect    (0, 0, w, h),
    _updateGradientRect (0, 0, w, h),
    _updateImprintRect  (0, 0, w, h)
{
}

const Rectangle& UpdateArea::updateError(const RenderContext& rc)
{
    newRect(rc);
    Rectangle newErrorRect(
        (int)_xMin-1, (int)_yMin-1, (int)(_xMax-_xMin)+1, (int)(_yMax-_yMin)+1);
    _updateErrorRect = Rectangle(newErrorRect, _prevErrorRect);
    _updateErrorRect.clamp(0, rc.width, 0, rc.height);
    _prevErrorRect = newErrorRect;

    return _updateErrorRect;
}

const Rectangle& UpdateArea::updateGradient(const RenderContext& rc)
{
    newRect(rc);
    Rectangle newGradientRect(
        (int)_xMin-1, (int)_yMin-1, (int)(_xMax-_xMin)+1, (int)(_yMax-_yMin)+1);
    _updateGradientRect = Rectangle(newGradientRect, _prevGradientRect);
    _updateGradientRect.clamp(0, rc.width, 0, rc.height);
    _prevGradientRect = newGradientRect;

    return _updateGradientRect;
}

const Rectangle& UpdateArea::updateImprint(const RenderContext& rc)
{
    newRect(rc);
    Rectangle newImprintRect(
        (int)_xMin-1, (int)_yMin-1, (int)(_xMax-_xMin)+1, (int)(_yMax-_yMin)+1);
    _updateImprintRect = Rectangle(newImprintRect, _prevImprintRect);
    _updateImprintRect.clamp(0, rc.width, 0, rc.height);
    _prevImprintRect = newImprintRect;

    return _updateImprintRect;
}

const Rectangle& UpdateArea::errorRect() const
{
    return _updateErrorRect;
}

const Rectangle& UpdateArea::gradientRect() const
{
    return _updateGradientRect;
}

const Rectangle& UpdateArea::imprintRect() const
{
    return _updateImprintRect;
}

void UpdateArea::newRect(const RenderContext& rc)
{
    // Rotated corners
    float ww = rc.imprintTextureWidth*0.5f*rc.imprintParams[2];
    float hh = rc.imprintTextureHeight*0.5f*rc.imprintParams[2];
    Mat2f rm;
    rm <<   cosf(rc.imprintParams[3]), -sinf(rc.imprintParams[3]),
        sinf(rc.imprintParams[3]), cosf(rc.imprintParams[3]);
    Vec2f c[4] = {
        Vec2f(rc.imprintParams[0], rc.imprintParams[1]) + rm*Vec2f(-ww, -hh),
        Vec2f(rc.imprintParams[0], rc.imprintParams[1]) + rm*Vec2f(ww, -hh),
        Vec2f(rc.imprintParams[0], rc.imprintParams[1]) + rm*Vec2f(-ww, hh),
        Vec2f(rc.imprintParams[0], rc.imprintParams[1]) + rm*Vec2f(ww, hh)
    };

    // Boundaries
    _xMin = std::min(std::min(c[0](0), c[1](0)), std::min(c[2](0), c[3](0)));
    _xMax = std::max(std::max(c[0](0), c[1](0)), std::max(c[2](0), c[3](0)));
    _yMin = std::min(std::min(c[0](1), c[1](1)), std::min(c[2](1), c[3](1)));
    _yMax = std::max(std::max(c[0](1), c[1](1)), std::max(c[2](1), c[3](1)));
}
