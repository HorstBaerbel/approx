#pragma once

#include "result.h"

#include <chrono>
#include <cstdint>
#include <functional>
#include <iostream>
#include <math.h>
#include <numeric>
#include <string>
#include <vector>

/// @brief Test suite base class. Use to derive test suites from.
/// @tparam TestT Test variable / input / output type (e.g. float).
/// @tparam ResultT Type the results are stored in. Use a wider type for storing results with increased precision (e.g double).
template <typename TestT, typename ResultT = TestT>
class Test
{
  public:
    Test(const std::string& suiteName, const std::pair<TestT, TestT>& inputRange, uint64_t samplesInRange)
        : m_suiteName(suiteName), m_inputRange(inputRange)
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
        volatile TestT dummy = 0;
        // "calibrate" the speed loop
        const TestT* inputData = m_inputValues.data();
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
    static std::pair<ResultT, ResultT> minmax(const std::vector<ResultT>& values)
    {
        // calculate min / max percentage range of values
        auto mme = std::minmax_element(values.cbegin(), values.cend());
        return std::make_pair(*mme.first, *mme.second);
    }

    static ResultT sumOfSquares(const std::vector<ResultT>& values)
    {
        ResultT sqrValue = ResultT();
        for (const auto v : values)
        {
            sqrValue += v * v;
        }
        return sqrValue;
    }

    static ResultT stddev(const std::vector<ResultT>& values)
    {
        return sqrt(sumOfSquares(values) / (values.size() - 1));
    }

    static ResultT mean(const std::vector<ResultT>& values)
    {
        return std::accumulate(values.cbegin(), values.cend(), ResultT()) / values.size();
    }

    static ResultT median(const std::vector<ResultT>& values)
    {
        std::vector<ResultT> sortvalues = values;
        std::nth_element(sortvalues.begin(), sortvalues.begin() + sortvalues.size() / 2, sortvalues.end());
        auto medianIt = sortvalues.cbegin() + sortvalues.size() / 2;
        return *medianIt;
    }

    static ResultT variance(const std::vector<ResultT>& values)
    {
        return sumOfSquares(values) - std::pow(mean(values), 2);
    }

    static void calculateErrorStatistics(typename Result<ResultT>::Errors& errors)
    {
        auto minmaxValue = minmax(errors.values);
        errors.minimum = minmaxValue.first;
        errors.maximum = minmaxValue.second;
        errors.mean = mean(errors.values);
        errors.median = median(errors.values);
        errors.variance = variance(errors.values);
    }

    template <typename Approximation, typename ReferenceFunction>
    Result<ResultT> run(const std::string& name, const std::string& description, Approximation approx, ReferenceFunction ref) const
    {
        Result<ResultT> result;
        result.suiteName = m_suiteName;
        result.name = name;
        result.description = description;
        result.inputRange = m_inputRange;
        result.samplesInRange = m_samplesInRange;
        result.overheadNs = m_overheadNs;
        // make sure we use a volatile destination, so values are not thrown away.
        volatile TestT dummy = 0;
        const TestT* inputData = m_inputValues.data();
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
            ResultT v = ref(inputData[i]);
            ResultT a = approx(inputData[i]);
            result.values.push_back(a);
            // calculate absolute error
            result.absoluteErrors.values.push_back(abs(a - v));
            // calculate relative error
            if (v > 0)
            {
                result.relativeErrors.values.push_back(abs(1.0 - a / v));
            }
        }
        // calculate error statistics
        calculateErrorStatistics(result.absoluteErrors);
        calculateErrorStatistics(result.relativeErrors);
        // calculate stddev
        result.stddev = stddev(result.absoluteErrors.values);
        return result;
    }

  private:
    std::string m_suiteName;
    std::pair<TestT, TestT> m_inputRange = {0, 0};
    uint64_t m_samplesInRange = 0;
    std::vector<TestT> m_inputValues;
    uint64_t m_overheadNs = 0;
};

template <typename ResultT>
std::ostream& operator<<(std::ostream& os, const Result<ResultT>& r)
{
    os << r.name << " - " << r.description << std::endl;
    auto& ae = r.absoluteErrors;
    os << "Absolute error: (" << ae.minimum << ", " << ae.maximum << "), mean: " << ae.mean << ", median: " << ae.median << ", variance: " << ae.variance << std::endl;
    auto& re = r.relativeErrors;
    os << "Relative error: (" << re.minimum << ", " << re.maximum << "), mean: " << re.mean << ", median: " << re.median << ", variance: " << re.variance << std::endl;
    os << "Standard deviation: " << r.stddev << std::endl;
    os << "Execution time: " << float(r.callNs - r.overheadNs) / float(r.samplesInRange) << " ns / call" << std::endl;
    return os;
}

template <typename ResultT>
std::ostream& operator<<(std::ostream& os, const std::vector<Result<ResultT>>& rs)
{
    const auto& fr = rs.front();
    os << "Testing: " << fr.suiteName << std::endl;
    os << "Input range: (" << fr.inputRange.first << ", " << fr.inputRange.second << "), " << fr.samplesInRange << " samples in range" << std::endl;
    os << "Tested functions:" << std::endl
       << std::endl;
    for (const auto& r : rs)
    {
        os << r << std::endl;
    }
    return os;
}
