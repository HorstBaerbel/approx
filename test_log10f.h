#pragma once

// The functions in this file are collected from all over the internet.
// All functions here assume positive, non-zero input values.
// All functions operate on float values, but some can be used on double values too.

#include "test.h"
#include <cstdint>
#include <math.h>
#include <vector>

// == 1 / log2(10)
#define ONE_OVER_LOG2_10 0.3010299956639812f

// Calculate the reference value for comparison.
// Here we count on the std implementation to be very precise.
long double log10f_reference(const long double x)
{
    return log10l(x);
}

// Standard log10f function for comparison.
float log10f_0(const float x)
{
    return log10f(x);
}

// log2(x) / log2(10).
float log10f_1(const float x)
{
    return log2f(x) * ONE_OVER_LOG2_10;
}

// Get approximation for log2(x).
// Dr. Paul Beckmann
// See: http://openaudio.blogspot.com/2017/02/faster-log10-and-pow.html
// And: https://community.arm.com/developer/tools-software/tools/f/armds-forum/4292/cmsis-dsp-new-functionality-proposal/22621#22621
float log10f_2(const float x)
{
    float Y, F;
    int E;
    F = frexpf(fabsf(x), &E);
    Y = 1.23149591368684f;
    Y *= F;
    Y += -4.11852516267426f;
    Y *= F;
    Y += 6.02197014179219f;
    Y *= F;
    Y += -3.13396450166353f;
    Y += E;
    return (Y * ONE_OVER_LOG2_10);
}

// compute log2(x) by reducing x to [0.75, 1.5), then divide by log2(10)
// David Goldberg
// See: https://tech.ebayinc.com/engineering/fast-approximate-logarithms-part-iii-the-formulas/
// This version uses divides
float log10f_3(float x)
{
    const float a = 0.338953;
    const float b = 2.198599;
    const float c = 1.523692;
#define FND fexp + signif*(a * signif + b) / (signif + c)

    float signif, fexp;
    int exp;
    float lg2;
    union
    {
        float f;
        unsigned int i;
    } ux1, ux2;
    int greater; // really a boolean
    /* 
     * Assume IEEE representation, which is sgn(1):exp(8):frac(23)
     * representing (1+frac)*2^(exp-127).  Call 1+frac the significand
     */

    // get exponent
    ux1.f = x;
    exp = (ux1.i & 0x7F800000) >> 23;
    // actual exponent is exp-127, will subtract 127 later

    greater = ux1.i & 0x00400000; // true if signif > 1.5
    if (greater)
    {
        // signif >= 1.5 so need to divide by 2.  Accomplish this by
        // stuffing exp = 126 which corresponds to an exponent of -1
        ux2.i = (ux1.i & 0x007FFFFF) | 0x3f000000;
        signif = ux2.f;
        fexp = exp - 126; // 126 instead of 127 compensates for division by 2
        signif = signif - 1.0;
        lg2 = FND;
    }
    else
    {
        // get signif by stuffing exp = 127 which corresponds to an exponent of 0
        ux2.i = (ux1.i & 0x007FFFFF) | 0x3f800000;
        signif = ux2.f;
        fexp = exp - 127;
        signif = signif - 1.0;
        lg2 = FND;
    }
    // last two lines of each branch are common code, but optimize better
    //  when duplicated, at least when using gcc
    return lg2 * ONE_OVER_LOG2_10;
}

// compute log2(x) by reducing x to [0.75, 1.5), then divide by log2(10)
// David Goldberg
// See: https://tech.ebayinc.com/engineering/fast-approximate-logarithms-part-iii-the-formulas/
// This version uses only multiplies
float log10f_4(float x)
{
    const float a = 0.338531;
    const float b = -0.741619;
    const float c = 1.445866;
#define FNM fexp + (((a * signif) + b) * signif + c) * signif

    float signif, fexp;
    int exp;
    float lg2;
    union
    {
        float f;
        unsigned int i;
    } ux1, ux2;
    int greater; // really a boolean
    /* 
     * Assume IEEE representation, which is sgn(1):exp(8):frac(23)
     * representing (1+frac)*2^(exp-127).  Call 1+frac the significand
     */

    // get exponent
    ux1.f = x;
    exp = (ux1.i & 0x7F800000) >> 23;
    // actual exponent is exp-127, will subtract 127 later

    greater = ux1.i & 0x00400000; // true if signif > 1.5
    if (greater)
    {
        // signif >= 1.5 so need to divide by 2.  Accomplish this by
        // stuffing exp = 126 which corresponds to an exponent of -1
        ux2.i = (ux1.i & 0x007FFFFF) | 0x3f000000;
        signif = ux2.f;
        fexp = exp - 126; // 126 instead of 127 compensates for division by 2
        signif = signif - 1.0;
        lg2 = FNM;
    }
    else
    {
        // get signif by stuffing exp = 127 which corresponds to an exponent of 0
        ux2.i = (ux1.i & 0x007FFFFF) | 0x3f800000;
        signif = ux2.f;
        fexp = exp - 127;
        signif = signif - 1.0;
        lg2 = FNM;
    }
    // last two lines of each branch are common code, but optimize better
    //  when duplicated, at least when using gcc
    return lg2 * ONE_OVER_LOG2_10;
}

class Log10Test : public Test<float, double>
{
  public:
    Log10Test(const std::pair<float, float>& inputRange, uint64_t samplesInRange)
        : Test("log10f", fixupInputRange(inputRange), samplesInRange, &log10f_reference)
    {
    }

    std::vector<Result<double>> runTests() const
    {
        std::vector<Result<double>> results;
        results.push_back(run("#0", "Reference", &log10f_0));
        results.push_back(run("#1", "log2(x) / log2(10)", &log10f_1));
        results.push_back(run("#2", "ARM forum / Dr. Paul Beckmann", &log10f_2));
        results.push_back(run("#3", "David Goldberg div", &log10f_3));
        results.push_back(run("#4", "David Goldberg mul", &log10f_4));
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
