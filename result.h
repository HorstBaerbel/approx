#pragma once

#include <string>
#include <cstdint>
#include <limits>
#include <vector>

template <typename T>
struct Result
{
    std::string suiteName;                                 // name of the test suite, e.g. "sqrtf"
    std::string name;                                      // identifier / short name of the method "#1"
    std::string description;                               // description of the method "foobar method"
    std::pair<T, T> inputRange = {0, 0};                   // range of input values
    uint64_t samplesInRange = 0;                           // number of points in range (2,n)
    std::vector<T> values;                                 // approx_f(x) of all calls
    std::vector<T> absoluteErrors;                         // absolute error abs(approx_f(x) - f(x)) of all calls
    T minAbsoluteError = std::numeric_limits<T>::max();    // minimum absolute error abs(approx_f(x) - f(x))
    T maxAbsoluteError = std::numeric_limits<T>::lowest(); // maximum absolute error abs(approx_f(x) - f(x))
    std::vector<T> relativeErrors;                         // relative error abs(1 - approx_f(x) / f(x)) of all calls
    T minRelativeError = std::numeric_limits<T>::max();    // minimum relative error abs(1 - approx_f(x) / f(x))
    T maxRelativeError = std::numeric_limits<T>::lowest(); // maximum relative error abs(1 - approx_f(x) / f(x))
    T stddev = std::numeric_limits<T>::max();              // standard deviation sqrt(sum(sqr(approx_f(x) - f(x))) / (N - 1))
    uint64_t callNs = 0;                                   // execution time for all calls of the function (accumulated)
    uint64_t overheadNs = 0;                               // estimated loop and data fetch overhead (accumulated)
};
