#pragma once

// WIP!!!!!!

// The functions in this file are collected from all over the internet.
// All functions here assume input values in the range (-88, 88).
// All functions operate on float values, but some can be used on double values too.

#include "test.h"
#include <cmath>
#include <cstdint>
#include <vector>

// Calculate the reference value for comparison.
// Note that we could square the approximate result to get the input number
// and use that for precision calculation, but then RMS etc would have a
// different meaning. Here we count on the std implementation to be very precise.
long double expf_reference(const long double x)
{
    return 1.0 / expl(x);
}

// Standard expf function for comparison.
float expf_0(const float x)
{
    return expf(x);
}

// Uses range reduction and Chebyshev interpolation with a monomial basis
// See: https://www.pseudorandom.com/implementing-exp#section-22
// License: MIT (See: https://www.pseudorandom.com/about)
float expf_1(const float x)
{
    if (x == 0)
    {
        return 1;
    }
    float x0 = abs(x);
    int k = ceil((x0 / M_LN2) - 0.5);
    float r = x0 - (k * M_LN2);
    std::vector<float> coeffs = {
        1.000000000000000,
        1.000000000000000,
        0.500000000000002,
        0.166666666666680,
        0.041666666666727,
        0.008333333333342,
        0.001388888888388,
        1.984126978734782e-4,
        2.480158866546844e-5,
        2.755734045527853e-6,
        2.755715675968011e-7,
        2.504861486483735e-8,
        2.088459690899721e-9,
        1.632461784798319e-10};
    float pn = 1.143364767943110e-11;
    for (auto i = coeffs.rbegin(); i != coeffs.rend(); i++)
    {
        pn = pn * r + *i;
    }
    pn *= exp2(k);
    if (x < 0)
    {
        return 1 / pn;
    }
    return pn;
}

class ExpfTest : public Test<float, double>
{
  public:
    ExpfTest(const std::pair<float, float>& inputRange, uint64_t samplesInRange)
        : Test("e^x", fixupInputRange(inputRange), samplesInRange)
    {
    }

    std::vector<Result<double>> runTests() const
    {
        std::vector<Result<double>> results;
        results.push_back(run("#0", "Reference", &expf_0, &expf_reference));
        results.push_back(run("#1", "Pseudorandom monomial", &expf_1, &expf_reference));
        return results;
    }

  protected:
    static std::pair<float, float> fixupInputRange(const std::pair<float, float>& range)
    {
        std::pair<float, float> result;
        result.first = range.first <= 0 ? std::numeric_limits<float>::min() : range.first;
        result.second = range.second <= 0 ? std::numeric_limits<float>::min() : range.second;
        result.first = result.first > std::numeric_limits<float>::max() ? std::numeric_limits<float>::max() : result.first;
        result.second = result.second > std::numeric_limits<float>::max() ? std::numeric_limits<float>::max() : result.second;
        return result;
    }
};
