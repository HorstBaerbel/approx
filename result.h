#pragma once

#include <cstdint>
#include <limits>
#include <string>
#include <vector>

template <typename InputT, typename StorageT>
struct Result
{
    using input_t = InputT;
    using input_range_t = std::pair<input_t, input_t>;
    using storage_t = StorageT;
    using storage_range_t = std::pair<storage_t, storage_t>;

    struct Errors
    {
        std::vector<storage_t> values; // error values
        storage_t minimum = std::numeric_limits<storage_t>::max(); // minimum of error values
        storage_t maximum = std::numeric_limits<storage_t>::lowest(); // maximum of error values
        storage_t mean; // average of error values
        storage_t median; // median of error values
        storage_t variance; // variance of error values
    };

    std::string suiteName; // name of the test suite, e.g. "sqrtf"
    std::string name; // identifier / short name of the method "#1"
    std::string description; // description of the method "foobar method"
    input_range_t inputRange; // range of input values
    std::size_t samplesInRange = 0; // number of points in range (2,n)
    std::vector<storage_t> values; // approx_f(x) of all calls
    Errors absoluteErrors; // absolute error abs(approx_f(x) - f(x)) of all calls
    Errors relativeErrors; // relative error abs(1 - approx_f(x) / f(x)) of all calls
    storage_t stddev = std::numeric_limits<storage_t>::max(); // standard deviation sqrt(sum(sqr(approx_f(x) - f(x))) / (N - 1))
    uint64_t callNs = 0; // execution time for all calls of the function (accumulated)
    uint64_t overheadNs = 0; // estimated loop and data fetch overhead (accumulated)
};
