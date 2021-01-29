#pragma once

// The functions in this file are collected from all over the internet.
// and Wikipedia:
// https://en.wikipedia.org/wiki/Methods_of_computing_square_roots

// All functions here assume positive, non-zero input values.
// All functions operate on float values, but some can be used on double values too.

#include "test.h"
#include <cstdint>
#include <math.h>
#include <vector>

// Calculate the reference value for comparison.
// Note that we could square the approximate result to get the input number
// and use that for precision calculation, but then RMS etc would have a
// different meaning. Here we count on the std implementation to be very precise.
long double invsqrtf_reference(const long double x)
{
    return 1.0 / sqrtl(x);
}

// Standard sqrtf function for comparison.
float invsqrtf_0(const float x)
{
    return 1.0F / sqrtf(x);
}

// Fast inverse square root aka "Quake 3 fast inverse square root".
// This uses the magic number 0x5F375A86 instead of the orginal 0x5F3759DF.
// See: https://en.wikipedia.org/wiki/Fast_inverse_square_root
// See: http://www.lomont.org/Math/Papers/2003/InvSqrt.pdf
// Similar in: // See: http://www.azillionmonkeys.com/qed/sqroot.html#calcmeth
float invsqrtf_1(const float x)
{
    const float xhalf = 0.5F * x;
    union // get bits for floating value
    {
        float x;
        int i;
    } u;
    u.x = x;
    u.i = 0x5F375A86 - (u.i >> 1); // gives initial guess y0. use 0x5fe6ec85e7de30da for double
    u.x = u.x * (1.5F - xhalf * u.x * u.x); // Newton method, repeating increases accuracy
    return u.x;
}

// Fast inverse square root aka "Quake 3 fast inverse square root".
// This uses the magic number 0x5F375A86 instead of the orginal 0x5F3759DF.
// Uses two Newton iterations for better precision.
// See: https://en.wikipedia.org/wiki/Fast_inverse_square_root
// See: http://www.lomont.org/Math/Papers/2003/InvSqrt.pdf
// Similar in: // See: http://www.azillionmonkeys.com/qed/sqroot.html#calcmeth
float invsqrtf_2(const float x)
{
    const float xhalf = 0.5 * x;
    union // get bits for floating value
    {
        float x;
        int i;
    } u;
    u.x = x;
    u.i = 0x5F375A86 - (u.i >> 1); // gives initial guess y0. use 0x5fe6ec85e7de30da for double
    u.x = u.x * (1.5F - xhalf * u.x * u.x); // Newton method, repeating increases accuracy
    u.x = u.x * (1.5F - xhalf * u.x * u.x); // Newton method, repeating increases accuracy
    return u.x;
}

class InvSqrtfTest : public Test<float, double>
{
  public:
    InvSqrtfTest(const std::pair<float, float>& inputRange, uint64_t samplesInRange)
        : Test("1 / sqrtf", fixupInputRange(inputRange), samplesInRange, &invsqrtf_reference)
    {
    }

    std::vector<Result<double>> runTests() const
    {
        std::vector<Result<double>> results;
        results.push_back(run("#0", "Reference", &invsqrtf_0));
        results.push_back(run("#1", "Quake3", &invsqrtf_1));
        results.push_back(run("#2", "Quake3 + Newton", &invsqrtf_2));
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
