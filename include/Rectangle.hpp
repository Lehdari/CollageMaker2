//
// Project: CollageMaker2
// File: Rectangle.hpp
//
// Copyright (c) 2019 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENCE which is distributed
// with this source code package.
//

#ifndef COLLAGEMAKER2_RECTANGLE_HPP
#define COLLAGEMAKER2_RECTANGLE_HPP


struct Rectangle {
    int x;
    int y;
    int w;
    int h;

    Rectangle(const Rectangle&) = default;
    Rectangle(int x = 0, int y = 0, int w = 0, int h = 0);

    // Create bounding rectangle for two Rectangles
    Rectangle(const Rectangle& rect1, const Rectangle& rect2);

    // Clamp to limits
    void clamp(int xMin, int xMax, int yMin, int yMax);
};


#endif //COLLAGEMAKER2_RECTANGLE_HPP
