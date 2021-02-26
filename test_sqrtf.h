#pragma once

// The functions in this file are collected from all over the internet.
// Originally I found the CodeProject page here:
// https://www.codeproject.com/Articles/69941/Best-Square-Root-Method-Algorithm-Function-Precisi
// Some more approximations come from here:
// https://dsp.stackexchange.com/questions/17269/what-approximation-techniques-exist-for-computing-the-square-root
// https://stackoverflow.com/questions/3051602/how-do-i-compute-the-square-root-of-a-number-without-using-builtins
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
long double sqrtf_reference(const long double x)
{
    return sqrtl(x);
}

// Standard sqrtf function for comparison.
float sqrtf_0(const float x)
{
    return sqrtf(x);
}

// Get approximation for log2(x) / 2 and add bias to improve error.
// See: https://en.wikipedia.org/wiki/Methods_of_computing_square_roots#Approximations_that_depend_on_the_floating_point_representation
float sqrtf_1(const float x)
{
    union
    {
        int i;
        float x;
    } u;
    u.x = x;
    u.i = (1 << 29) + (u.i >> 1) - (1 << 22) - 0x4B0D2;
    return u.x;
}

// Get approximation for log2(x) / 2 as initial guess,
// apply Babylonian method twice.
// See: https://en.wikipedia.org/wiki/Methods_of_computing_square_roots#Approximations_that_depend_on_the_floating_point_representation
float sqrtf_2(const float x)
{
    union
    {
        int i;
        float x;
    } u;
    u.x = x;
    u.i = (1 << 29) + (u.i >> 1) - (1 << 22);
    // Two Babylonian Steps (simplified from:)
    // u.x = 0.5f * (u.x + x/u.x);
    // u.x = 0.5f * (u.x + x/u.x);
    u.x = u.x + x / u.x;
    u.x = 0.25 * u.x + x / u.x;
    return u.x;
}

// Get approximation for log2(x) / 2 as initial guess,
// add bias to improve error,
// apply Babylonian method twice.
// See: https://en.wikipedia.org/wiki/Methods_of_computing_square_roots#Approximations_that_depend_on_the_floating_point_representation
float sqrtf_3(const float x)
{
    union
    {
        int i;
        float x;
    } u;
    u.x = x;
    u.i = (1 << 29) + (u.i >> 1) - (1 << 22) - 0x4B0D2;
    // Two Babylonian Steps (simplified to use only one division)
    // u.x = 0.5f * (u.x + x/u.x);
    // u.x = 0.5f * (u.x + x/u.x);
    auto u2 = u.x * u.x;
    u.x = (x * x + (6.0 * x + u2) * u2) / (4.0 * u.x * (x + u2));
    return u.x;
}

// Get approximation for log2(x) / 2 as initial guess,
// add bias to improve error,
// apply an iteration of the Bakhshali method for better precision.
// See: https://en.wikipedia.org/wiki/Methods_of_computing_square_roots#Approximations_that_depend_on_the_floating_point_representation
float sqrtf_4(const float x)
{
    union
    {
        int i;
        float x;
    } u;
    u.x = x;
    u.i = (1 << 29) + (u.i >> 1) - (1 << 22) - 0x4B0D2;
    u.x = (u.x * u.x + x) / (2 * u.x);
    return u.x;
}

// Fast inverse square root aka "Quake 3 fast inverse square root", multiplied by x, which is sqrt(x).
// This uses the magic number 0x5F375A86 instead of the orginal 0x5F3759DF.
// See: https://en.wikipedia.org/wiki/Fast_inverse_square_root
// See: http://www.lomont.org/Math/Papers/2003/InvSqrt.pdf
// Similar in: // See: http://www.azillionmonkeys.com/qed/sqroot.html#calcmeth
float sqrtf_5(const float x)
{
    const float xhalf = 0.5 * x;
    union // get bits for floating value
    {
        float x;
        int i;
    } u;
    u.x = x;
    u.i = 0x5F375A86 - (u.i >> 1); // gives initial guess y0. use 0x5fe6ec85e7de30da for double
    u.x = u.x * (1.5 - xhalf * u.x * u.x); // Newton method, repeating increases accuracy
    //u.x = u.x * (1.5 - xhalf * u.x * u.x); // Newton method, repeating increases accuracy
    return x * u.x;
}

// Fast inverse square root aka "Quake 3 fast inverse square root", multiplied by x, which is sqrt(x).
// Uses one iteration of Halley's method for precision.
// See: https://en.wikipedia.org/wiki/Methods_of_computing_square_roots#Iterative_methods_for_reciprocal_square_roots
float sqrtf_6(const float x)
{
    union // get bits for floating value
    {
        float x;
        int i;
    } u;
    u.x = x;
    u.i = 0x5F375A86 - (u.i >> 1); // gives initial guess y0. use 0x5fe6ec85e7de30da for double
    float xu = x * u.x;
    float xu2 = xu * u.x;
    u.x = (0.125 * 3.0) * xu * (5.0 - xu2 * ((10.0 / 3.0) - xu2)); // Halley's method, repeating increases accuracy
    return u.x;
}

// Bit twiddling from Intel Software Optimization Cookbook, 2nd edition, page 187
// See: http://bits.stephan-brumme.com/squareRoot.html
float sqrtf_7(const float x)
{
    unsigned int i = *(unsigned int*)&x;
    i += 127 << 23; // adjust bias
    i >>= 1; // approximation of square root
    return *(float*)&i;
}

// Bit twiddling from Intel Software Optimization Cookbook, 2nd edition, page 187
// with an iteration of the Bakhshali method for better precision.
// See: http://bits.stephan-brumme.com/squareRoot.html
// See: https://en.wikipedia.org/wiki/Methods_of_computing_square_roots#Bakhshali_method
float sqrtf_8(const float x)
{
    unsigned int i = *(unsigned int*)&x;
    i += 127 << 23; // adjust bias
    i >>= 1; // approximation of square root
    float f = *(float*)&i;
    return (f * f + x) / (2 * f);
}

// Taylor series with some bit fiddling?
// See: https://dsp.stackexchange.com/questions/17269/what-approximation-techniques-exist-for-computing-the-square-root
float sqrtf_9(const float x)
{
    union
    {
        float f;
        long i;
    } u;
    u.f = x;
    uint32_t intPart = ((u.i) >> 23); // get biased exponent
    intPart -= 127; // unbias it
    float n = (float)(u.i & 0x007FFFFF); // mask off exponent leaving 0x800000*(mantissa - 1)
    n *= 1.192092895507812e-07; // divide by 0x800000
    float accumulator = 1.0 + 0.49959804148061 * n;
    float xPower = n * n;
    accumulator += -0.12047308243453 * xPower;
    xPower *= n;
    accumulator += 0.04585425015501 * xPower;
    xPower *= n;
    accumulator += -0.01076564682800 * xPower;
    if (intPart & 0x00000001)
    {
        accumulator *= 1.41421356237309504880; // an odd input exponent means an extra sqrt(2) in the output
    }
    u.i = intPart >> 1; // divide exponent by 2, lose LSB
    u.i += 127; // rebias exponent
    u.i <<= 23; // move biased exponent into exponent bits
    return accumulator * u.f;
}

// Apply Newtons method until answer does not change.
// See: http://forums.techarena.in/software-development/1290144.htm (Author: "Reegan")
float sqrtf_10(const float x)
{
    float n = x / 2.0;
    float lstX = 0.0;
    while (n != lstX)
    {
        lstX = n;
        n = (n + x / n) / 2.0;
    }
    return n;
}

// Apply Newtons method until ACCURACY is reached.
// See: http://www.cs.uni.edu/~jacobson/C++/newton.html
float sqrtf_11(const float x)
{
    const float ACCURACY = 0.01;
    float lower, upper, guess;
    if (x < 1)
    {
        lower = x;
        upper = 1;
    }
    else
    {
        lower = 1;
        upper = x;
    }
    while ((upper - lower) > ACCURACY)
    {
        guess = (lower + upper) / 2;
        if (guess * guess > x)
            upper = guess;
        else
            lower = guess;
    }
    return (lower + upper) / 2;
}

class SqrtfTest : public Test<float, double>
{
  public:
    SqrtfTest(const std::pair<float, float>& inputRange, uint64_t samplesInRange)
        : Test("sqrtf", fixupInputRange(inputRange), samplesInRange, &sqrtf_reference)
    {
    }

    std::vector<Result<double>> runTests() const
    {
        std::vector<Result<double>> results;
        results.push_back(run("#0", "Reference (std::sqrtf)", &sqrtf_0));
        results.push_back(run("#1", "log2(x) + bias", &sqrtf_1));
        results.push_back(run("#2", "log2(x) + Babylonian", &sqrtf_2));
        results.push_back(run("#3", "log2(x) + bias + Babylonian", &sqrtf_3));
        results.push_back(run("#4", "log2(x) + bias + Bakhshali", &sqrtf_4));
        results.push_back(run("#5", "Quake3 + Newton", &sqrtf_5));
        results.push_back(run("#6", "Quake3 + Halley", &sqrtf_6));
        results.push_back(run("#7", "Intel SOC", &sqrtf_7));
        results.push_back(run("#8", "Intel SOC + Bakhshali", &sqrtf_8));
        results.push_back(run("#9", "Taylor3", &sqrtf_9));
        results.push_back(run("#10", "Newton while change", &sqrtf_10));
        results.push_back(run("#11", "Newton accuracy 0.01", &sqrtf_11));
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
