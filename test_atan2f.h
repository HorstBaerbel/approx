#pragma once

// WIP!!!!!!

// The functions in this file are collected from all over the internet.
// All functions here assume input values in the range (-88, 88).
// All functions operate on float values, but some can be used on double values too.

#include "test.h"
#include <cmath>
#include <cstdint>
#include <tuple>
#include <vector>

// Calculate the reference value for comparison.
// Note that we could square the approximate result to get the input number
// and use that for precision calculation, but then RMS etc would have a
// different meaning. Here we count on the std implementation to be very precise.
long double atan2_reference(const std::tuple<long double, long double> yx)
{
    return atan2(std::get<0>(yx), std::get<1>(yx));
}

// Standard function for comparison.
float atan2_0(const std::tuple<float, float> yx)
{
    return atan2f(std::get<0>(yx), std::get<1>(yx));
}

// Diamond angle
// See: https://www.freesteel.co.uk/wpblog/2009/06/05/encoding-2d-angles-without-trigonometry/
// License: ???
float atan2_1(const std::tuple<float, float> yx)
{
    return atan2f(std::get<0>(yx), std::get<1>(yx));
}

class Atan2fTest : public Test<std::tuple<float, float>, float, double>
{
  public:
    Atan2fTest(input_generator_t inputGenerator, const input_range_t& inputRange, uint64_t samplesInRange)
        : Test(
            "atan2(y,x)", inputGenerator,
            fixupInputRange(inputRange), samplesInRange, &atan2_reference, &dummyFunc)
    {
    }

    std::vector<Result<input_t, storage_t>> runTests() const
    {
        std::vector<Result<input_t, storage_t>> results;
        results.push_back(run("#0", "std::atan2f", &atan2_0));
        results.push_back(run("#1", "???", &atan2_1));
        return results;
    }

  protected:
    static float dummyFunc(const input_t x)
    {
        return std::get<0>(x);
    }

    static input_range_t fixupInputRange(const input_range_t& range)
    {
        auto yA = std::get<0>(range.first);
        auto yB = std::get<0>(range.second);
        auto xA = std::get<1>(range.first);
        auto xB = std::get<1>(range.second);
        return {{std::min(yA, yB), std::min(xA, xB)}, {std::max(yA, yB), std::max(xA, xB)}};
    }
};
