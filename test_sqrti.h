#pragma once

// WIP

// https://www.cs.uni-potsdam.de/ti/kreitz/PDF/03cucs-intsqrt.pdf

// Optimized Binomial Theorem
// See: https://www.drdobbs.com/parallel/algorithm-alley/184409869
uint32_t sqrti1(uint32_t N)
{
    uint32_t l2, u, v, u2, n;
    if (2 > N)
        return (N);
    u = N;
    l2 = 0;
    while (u >>= 2)
        l2++;
    u = 1L << l2;
    v = u;
    u2 = u << l2;
    while (l2--)
    {
        v >>= 1;
        n = (u + u + v) << l2;
        n += u2;
        if (n <= N)
        {
            u += v;
            u2 = n;
        }
    }
    return (u);
}

// No idea where this comes from.
float sqrti2(const float m)
{
    int i = 0;
    while ((i * i) <= m)
        i++;
    i--;
    if (i > 0)
    {
        float d = m - i * i;
        float p = d / (2 * i);
        float a = i + p;
        return a - (p * p) / (2 * a);
    }
    return 0;
}