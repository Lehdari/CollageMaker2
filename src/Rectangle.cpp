//
// Project: CollageMaker2
// File: Rectangle.cpp
//
// Copyright (c) 2019 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENCE which is distributed
// with this source code package.
//

#include "Rectangle.hpp"

#include <algorithm>
#include <Rectangle.hpp>


Rectangle::Rectangle(int x, int y, int w, int h) :
    x   (x),
    y   (y),
    w   (w),
    h   (h)
{
}

Rectangle::Rectangle(const Rectangle& rect1, const Rectangle& rect2) :
    x   (std::min(rect1.x, rect2.x)),
    y   (std::min(rect1.y, rect2.y)),
    w   (std::max(rect1.x+rect1.w, rect2.x+rect2.w)-x),
    h   (std::max(rect1.y+rect1.h, rect2.y+rect2.h)-y)
{
}

void Rectangle::clamp(int xMin, int xMax, int yMin, int yMax)
{
    x = std::clamp(x, xMin, xMax);
    y = std::clamp(y, yMin, yMax);
    w = std::clamp(w, 0, xMax-x);
    h = std::clamp(h, 0, yMax-y);
}
