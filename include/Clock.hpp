//
// Project: CollageMaker2
// File: Clock.hpp
//
// Copyright (c) 2019 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENSE which is distributed
// with this source code package.
//

#ifndef COLLAGEMAKER2_CLOCK_HPP
#define COLLAGEMAKER2_CLOCK_HPP


#include <chrono>


class Clock {
public:
    Clock();
    double operator()();

private:
    std::chrono::steady_clock::time_point   _t;
};


#endif //COLLAGEMAKER2_CLOCK_HPP
