#pragma once

// The functions in this file are collected from all over the internet.
// See also Wikipedia: https://en.wikipedia.org/wiki/Methods_of_computing_square_roots
// And "Jack W. Crenshaw - Math Toolkit for Real-Time Development": http%3A%2F%2Ffmipa.umri.ac.id%2Fwp-content%2Fuploads%2F2016%2F03%2FJack-W.-Crenshaw-Math-toolkit-for-real-time-programming.9781929629091.35924.pdf

// All functions here assume positive, non-zero input values.
// All functions operate on 32bit values, but some can be used on smaller or bigger data types too.

#include "test.h"
#include <cstdint>
#include <math.h>
#include <vector>

// Calculate the reference value for comparison.
// Note that we could square the approximate result to get the input number
// and use that for precision calculation, but then RMS etc. would have a
// different meaning. Here we count on the std implementation to be very precise.
uint32_t sqrti_reference(const uint32_t x)
{
    return sqrt(x);
}

// https://www.cs.uni-potsdam.de/ti/kreitz/PDF/03cucs-intsqrt.pdf

// Optimized binomial theorem
// See: https://www.drdobbs.com/parallel/algorithm-alley/184409869
uint32_t sqrti_1(const uint32_t x)
{
    uint32_t l2, u, v, u2, n;
    if (2 > x)
    {
        return x;
    }
    u = x;
    l2 = 0;
    while (u >>= 2)
    {
        l2++;
    }
    u = 1L << l2;
    v = u;
    u2 = u << l2;
    while (l2--)
    {
        v >>= 1;
        n = (u + u + v) << l2;
        n += u2;
        if (n <= x)
        {
            u += v;
            u2 = n;
        }
    }
    return u;
}

// Square root by abacus algorithm, Martin Guy @ UKC, June 1985.
// From a book on programming abaci by Mr C. Woo.
// See: http://freaknet.org/martin/tape/gos/misc/personal/msc/sqrt
uint32_t sqrti_2(uint32_t x)
{
    // Logically, these are unsigned. We need the sign bit to test whether (op - res - one) underflowed.
    uint32_t op = x;
    uint32_t res = 0;
    // "one" starts at the highest power of four <= than the argument
    uint32_t one = 1 << 30; // second-to-top bit set
    while (one > op) one >>= 2;
    while (one != 0)
    {
        if (op >= res + one)
        {
            op -= res + one;
            res += one << 1;
        }
        res >>= 1; // res/2
        one >>= 2; // one/4
    }
    return res;
}

// From Jack W. Crenshaw's 1998 article in Embedded:
// http://www.embedded.com/electronics-blogs/programmer-s-toolbox/4219659/Integer-Square-Roots
// See also: https://gist.github.com/foobaz/3287f153d125277eefea
uint32_t sqrti_3(uint32_t x)
{
    uint32_t rem = 0;
    uint32_t root = 0;
    for (int i = 32 / 2; i > 0; i--)
    {
        root <<= 1;
        rem = (rem << 2) | (x >> (32 - 2));
        x <<= 2;
        if (root < rem)
        {
            rem -= root | 1;
            root += 2;
        }
    }
    return root >> 1;
}

// Ross M. Fosler, Microchip Technology Inc.
// See: http://ww1.microchip.com/downloads/en/AppNotes/91040a.pdf
// See also: https://gist.github.com/foobaz/3287f153d125277eefea
uint32_t sqrti_4(uint32_t x)
{
    uint32_t res = 0;
    uint32_t add = 0x8000;
    uint32_t i;
    for (i = 0; i < 16; i++)
    {
        uint32_t temp = res | add;
        uint32_t g2 = temp;
        g2 *= temp;
        if (x >= g2)
        {
            res = temp;
        }
        add >>= 1;
    }
    return res;
}

// Tristan Muntsinger (Tristan.Muntsinger@gmail.com)
// See: http://www.codecodex.com/wiki/Calculate_an_integer_square_root
uint32_t sqrti_5(uint32_t n)
{
    uint32_t c = 0x8000;
    uint32_t g = 0x8000;
    for (;;)
    {
        if (g * g > n)
        {
            g ^= c;
        }
        c >>= 1;
        if (c == 0)
        {
            return g;
        }
        g |= c;
    }
}

class SqrtiTest : public Test<uint32_t, double>
{
  public:
    SqrtiTest(const std::pair<uint32_t, uint32_t>& inputRange, uint64_t samplesInRange)
        : Test("sqrti", fixupInputRange(inputRange), samplesInRange, &sqrti_reference)
    {
    }

    std::vector<Result<double>> runTests() const
    {
        std::vector<Result<double>> results;
        results.push_back(run("#0", "Reference", &sqrti_reference));
        results.push_back(run("#1", "Optimized binomial theorem", &sqrti_1));
        results.push_back(run("#2", "Abacus algorithm", &sqrti_2));
        results.push_back(run("#3", "Crenshaw Embedded 1998", &sqrti_3));
        results.push_back(run("#4", "Fosler Microchip", &sqrti_4));
        results.push_back(run("#5", "Tristan Muntsinger", &sqrti_5));
        return results;
    }

  protected:
    static std::pair<uint32_t, uint32_t> fixupInputRange(const std::pair<uint32_t, uint32_t>& range)
    {
        std::pair<uint32_t, uint32_t> result;
        result.first = range.first <= 0 ? std::numeric_limits<uint32_t>::min() : range.first;
        result.second = range.second <= 0 ? std::numeric_limits<uint32_t>::min() : range.second;
        result.first = result.first > std::numeric_limits<uint32_t>::max() ? std::numeric_limits<uint32_t>::max() : result.first;
        result.second = result.second > std::numeric_limits<uint32_t>::max() ? std::numeric_limits<uint32_t>::max() : result.second;
        return result;
    }
};