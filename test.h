#pragma once

#include <string>
#include <functional>
#include <chrono>
#include <cstdint>
#include <math.h>
#include <vector>
#include <iostream>

template <typename T>
struct Result
{
    std::string name;                                   // identifier / short name of the method
    std::string description;                            // description of the method
    std::pair<T, T> inputRange = {0, 0};                // range of input values
    uint64_t samplesInRange = 0;                        //number of points in range (2,n)
    std::vector<T> absoluteErrors;                      // absolute error abs(approx_f(x) - f(x)) of all calls
    T minAbsoluteError = std::numeric_limits<T>::max(); // minimum absolute error abs(approx_f(x) - f(x))
    T maxAbsoluteError = std::numeric_limits<T>::min(); // maximum absolute error abs(approx_f(x) - f(x))
    std::vector<T> relativeErrors;                      // relative error abs(1 - approx_f(x) / f(x)) of all calls
    T minRelativeError = std::numeric_limits<T>::max(); // minimum relative error abs(1 - approx_f(x) / f(x))
    T maxRelativeError = std::numeric_limits<T>::min(); // maximum relative error abs(1 - approx_f(x) / f(x))
    T stddev = std::numeric_limits<T>::max();           // standard deviation sqrt(sum(sqr(approx_f(x) - f(x))) / (N - 1))
    uint64_t callNs = 0;                                // execution time for all calls of the function (accumulated)
    uint64_t overheadNs;                                // estimated loop and data fetch overhead (accumulated)
};

template <typename T>
class Test
{
public:
    Test(const std::pair<T, T> &inputRange, uint64_t samplesInRange)
        : m_inputRange(inputRange)
    {
        if (m_inputRange.first > m_inputRange.second)
        {
            std::swap(m_inputRange.first, m_inputRange.second);
        }
        m_samplesInRange = samplesInRange < 2 ? 2 : samplesInRange;
        // generate input values
        for (uint_fast64_t i = 0; i < m_samplesInRange; ++i)
        {
            m_inputValues.push_back(m_inputRange.first + ((m_inputRange.second - m_inputRange.first) * i) / (m_samplesInRange - 1));
        }
        // make sure we use a volatile destination, so values are not thrown away.
        volatile T dummy = 0;
        // "calibrate" the speed loop
        const T *inputData = m_inputValues.data();
        auto startCalib = std::chrono::high_resolution_clock::now();
        for (uint_fast64_t j = 0; j < 10; ++j)
        {
            for (uint_fast64_t i = 0; i < m_samplesInRange; ++i)
            {
                dummy = inputData[i];
            }
        }
        auto overheadDuration = std::chrono::high_resolution_clock::now() - startCalib;
        m_overheadNs = std::chrono::duration_cast<std::chrono::nanoseconds>(overheadDuration).count() / 10;
    }

protected:
    template <typename Approximation, typename ReferenceFunction>
    Result<T> run(const std::string &name, const std::string &description, Approximation approx, ReferenceFunction ref) const
    {
        Result<T> result;
        result.name = name;
        result.description = description;
        result.inputRange = m_inputRange;
        result.samplesInRange = m_samplesInRange;
        result.overheadNs = m_overheadNs;
        // make sure we use a volatile destination, so values are not thrown away.
        volatile T dummy = 0;
        const T *inputData = m_inputValues.data();
        // start speed measurement
        auto startSpeed = std::chrono::high_resolution_clock::now();
        for (uint_fast64_t i = 0; i < result.samplesInRange; ++i)
        {
            dummy = approx(inputData[i]);
        }
        auto speedDuration = std::chrono::high_resolution_clock::now() - startSpeed;
        result.callNs = std::chrono::duration_cast<std::chrono::nanoseconds>(speedDuration).count();
        // now check precision
        for (uint_fast64_t i = 0; i < result.samplesInRange; ++i)
        {
            long double v = ref(inputData[i]);
            long double a = approx(inputData[i]);
            // calculate absolute error
            long double absoluteError = abs(a - v);
            result.minAbsoluteError = absoluteError < result.minAbsoluteError ? absoluteError : result.minAbsoluteError;
            result.maxAbsoluteError = absoluteError > result.maxAbsoluteError ? absoluteError : result.maxAbsoluteError;
            result.absoluteErrors.push_back(absoluteError);
            // calculate relative error
            if (v > 0)
            {
                long double relativeError = abs(1.0 - a / v);
                result.minRelativeError = relativeError < result.minRelativeError ? relativeError : result.minRelativeError;
                result.maxRelativeError = relativeError > result.maxRelativeError ? relativeError : result.maxRelativeError;
                result.relativeErrors.push_back(relativeError);
            }
        }
        // calculate stddev
        long double accumError = 0;
        for (const auto e : result.absoluteErrors)
        {
            accumError += e * e;
        }
        result.stddev = sqrtl(accumError / (result.samplesInRange - 1));
        return result;
    }

private:
    std::pair<T, T> m_inputRange = {0, 0};
    uint64_t m_samplesInRange = 0;
    std::vector<T> m_inputValues;
    uint64_t m_overheadNs = 0;
};

template <typename T>
std::ostream &operator<<(std::ostream &os, const Result<T> &r)
{
    os << r.name << " - " << r.description << std::endl;
    os << "Absolute error: (" << r.minAbsoluteError << ", " << r.maxAbsoluteError << ")" << std::endl;
    os << "Relative error: (" << r.minRelativeError << ", " << r.maxRelativeError << ")" << std::endl;
    os << "Standard deviation: " << r.stddev << std::endl;
    os << "Execution time: " << float(r.callNs - r.overheadNs) / float(r.samplesInRange) << " ns / call" << std::endl;
    return os;
}

template <typename T>
std::ostream &operator<<(std::ostream &os, const std::vector<Result<T>> &rs)
{
    const auto & fr = rs.front();
    os << "Input range: (" << fr.inputRange.first << ", " << fr.inputRange.second << "), " << fr.samplesInRange << " samples in range" << std::endl;
    os << "Tested functions:" << std::endl;
    for (const auto & r : rs)
    {
        os << r << std::endl;
    }
    return os;
}
