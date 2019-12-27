//
// Project: CollageMaker2
// File: Clock.cpp
//
// Copyright (c) 2019 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENSE which is distributed
// with this source code package.
//

#include "Clock.hpp"


Clock::Clock() :
    _t(std::chrono::steady_clock::now())
{
}

double Clock::operator()()
{
    auto t = std::chrono::steady_clock::now();
    double out = std::chrono::duration_cast<std::chrono::duration<double>>(t-_t).count();
    _t = t;
    return out;
}
