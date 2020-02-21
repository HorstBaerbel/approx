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

// Standard sqrt function for comaprison.
float sqrtf0(const float x)
{
    return sqrtf(x);
}

// Get approximation for log2(x) / 2 and add bias to improve error.
// See: https://en.wikipedia.org/wiki/Methods_of_computing_square_roots#Approximations_that_depend_on_the_floating_point_representation
float sqrtf1(const float x)
{
    union {
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
float sqrtf2(const float x)
{
    union {
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
float sqrtf3(const float x)
{
    union {
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
float sqrtf4(const float x)
{
    union {
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
float sqrtf5(const float x)
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
// Uses two Newton iterations for better precision.
float sqrtf6(const float x)
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
    u.x = u.x * (1.5 - xhalf * u.x * u.x); // Newton method, repeating increases accuracy
    return x * u.x;
}

// Bit twiddling from Intel Software Optimization Cookbook, 2nd edition, page 187
// See: http://bits.stephan-brumme.com/squareRoot.html
float sqrtf7(const float x)
{
    unsigned int i = *(unsigned int *)&x;
    i += 127 << 23; // adjust bias
    i >>= 1; // approximation of square root
    return *(float *)&i;
}

// Bit twiddling from Intel Software Optimization Cookbook, 2nd edition, page 187
// with an iteration of the Bakhshali method for better precision.
// See: http://bits.stephan-brumme.com/squareRoot.html
// See: https://en.wikipedia.org/wiki/Methods_of_computing_square_roots#Bakhshali_method
float sqrtf8(const float x)
{
    union {
        int i;
        float x;
    } u;
    u.x = x;
    u.i += 127 << 23; // adjust bias
    u.i >>= 1; // approximation of square root
    u.x = (u.x * u.x + x) / (2 * u.x);
    return u.x;
}

// Taylor series with some bit fiddling?
// See: https://dsp.stackexchange.com/questions/17269/what-approximation-techniques-exist-for-computing-the-square-root
float sqrtf9(const float x)
{
    union {
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
float sqrtf10(const float x)
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
float sqrtf11(const float x)
{
    const float ACCURACY = 0.001;
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

class SqrtfTest : public Test<float>
{
public:
    SqrtfTest(const std::pair<float, float> &inputRange, uint64_t samplesInRange)
        : Test(fixupInputRange(inputRange), samplesInRange)
    {
    }

    std::vector<Result<float>> runTests() const
    {
        std::vector<Result<float>> results;
        results.push_back(run( "sqrt0", "Standard library sqrtf", &sqrtf0, &sqrtf_reference));
        results.push_back(run( "sqrt1", "log2(x) + bias", &sqrtf1, &sqrtf_reference));
        results.push_back(run( "sqrt2", "log2(x) + Babylonian", &sqrtf2, &sqrtf_reference));
        results.push_back(run( "sqrt3", "log2(x) + bias + Babylonian", &sqrtf3, &sqrtf_reference));
        results.push_back(run( "sqrt4", "log2(x) + bias + Bakhshali", &sqrtf4, &sqrtf_reference));
        results.push_back(run( "sqrt5", "Quake3 + Newton", &sqrtf5, &sqrtf_reference));
        results.push_back(run( "sqrt6", "Quake3 + 2 * Newton", &sqrtf6, &sqrtf_reference));
        results.push_back(run( "sqrt7", "Intel SOC", &sqrtf7, &sqrtf_reference));
        results.push_back(run( "sqrt8", "Intel SOC + Bakhshali", &sqrtf8, &sqrtf_reference));
        results.push_back(run( "sqrt9", "Taylor3", &sqrtf9, &sqrtf_reference));
        results.push_back(run("sqrt10", "Newton while change", &sqrtf10, &sqrtf_reference));
        results.push_back(run("sqrt11", "Newton accuracy 0.001", &sqrtf11, &sqrtf_reference));
        return results;
    }

protected:
    static std::pair<float, float> fixupInputRange(const std::pair<float, float> &range)
    {
        std::pair<float, float> result;
        result.first = range.first <= 0 ? std::numeric_limits<float>::min() : range.first;
        result.second = range.second <= 0 ? std::numeric_limits<float>::min() : range.second;
        result.first = result.first > std::numeric_limits<float>::max() ? std::numeric_limits<float>::max() : result.first;
        result.second = result.second > std::numeric_limits<float>::max() ? std::numeric_limits<float>::max() : result.second;
        return result;
    }
};
