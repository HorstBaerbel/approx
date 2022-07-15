#pragma once

#include "result.h"

#include <chrono>
#include <cstdint>
#include <functional>
#include <iostream>
#include <math.h>
#include <numeric>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

/// @brief Test suite base class. Use to derive test suites from.
/// @tparam InputT Test input variable type (e.g. float, int or std::pair<float, float>).
/// @tparam OutputT Test output type (e.g. float).
/// @tparam StorageT Type the results are stored in. Use a wider type for storing results with increased precision (e.g double).
template <typename InputT, typename OutputT, typename StorageT = OutputT>
class Test
{
  public:
    using input_t = InputT;
    using input_range_t = std::pair<input_t, input_t>;
    using input_generator_t = std::function<std::vector<input_t>(const input_range_t&, uint64_t)>;
    using output_t = OutputT;
    using storage_t = StorageT;
    using storage_range_t = std::pair<storage_t, storage_t>;

    template <typename ReferenceFunction, typename DummyFunction>
    Test(const std::string& suiteName, input_generator_t inputGenerator, input_range_t inputRange, uint64_t samplesInRange, ReferenceFunction refFunc, DummyFunction dummyFunc)
        : m_suiteName(suiteName), m_inputRange(inputRange), m_inputValues(inputGenerator(inputRange, samplesInRange))
    {
        // generate reference result values
        const input_t* inputData = m_inputValues.data();
        for (uint_fast64_t i = 0; i < m_inputValues.size(); ++i)
        {
            m_referenceValues.push_back(refFunc(inputData[i]));
        }
        // make sure we use a volatile destination, so values are not thrown away.
        volatile storage_t dummy{};
        // "calibrate" the speed loop
        auto startCalib = std::chrono::high_resolution_clock::now();
        for (uint_fast64_t j = 0; j < LOOPCOUNT; ++j)
        {
            for (uint_fast64_t i = 0; i < m_inputValues.size(); ++i)
            {
                dummy = dummyFunc(inputData[i]);
            }
        }
        auto overheadDuration = std::chrono::high_resolution_clock::now() - startCalib;
        m_overheadNs = std::chrono::duration_cast<std::chrono::nanoseconds>(overheadDuration).count() / LOOPCOUNT;
    }

  protected:
    static storage_range_t minmax(const std::vector<storage_t>& values)
    {
        // calculate min / max percentage range of values
        auto mme = std::minmax_element(values.cbegin(), values.cend());
        return std::make_pair(*mme.first, *mme.second);
    }

    static storage_t sumOfSquares(const std::vector<storage_t>& values)
    {
        storage_t sqrValue = storage_t();
        for (const auto v : values)
        {
            sqrValue += v * v;
        }
        return sqrValue;
    }

    static storage_t stddev(const std::vector<storage_t>& values)
    {
        return sqrt(sumOfSquares(values) / (values.size() - 1));
    }

    static storage_t mean(const std::vector<storage_t>& values)
    {
        return std::accumulate(values.cbegin(), values.cend(), storage_t()) / values.size();
    }

    static storage_t median(const std::vector<storage_t>& values)
    {
        std::vector<storage_t> sortvalues = values;
        std::nth_element(sortvalues.begin(), sortvalues.begin() + sortvalues.size() / 2, sortvalues.end());
        auto medianIt = sortvalues.cbegin() + sortvalues.size() / 2;
        return *medianIt;
    }

    static storage_t variance(const std::vector<storage_t>& values)
    {
        return sumOfSquares(values) - std::pow(mean(values), 2);
    }

    static void calculateErrorStatistics(typename Result<input_t, storage_t>::Errors& errors)
    {
        auto minmaxValue = minmax(errors.values);
        errors.minimum = minmaxValue.first;
        errors.maximum = minmaxValue.second;
        errors.mean = mean(errors.values);
        errors.median = median(errors.values);
        errors.variance = variance(errors.values);
    }

    template <typename Approximation>
    Result<input_t, storage_t> run(const std::string& name, const std::string& description, Approximation approx) const
    {
        Result<input_t, storage_t> result;
        result.suiteName = m_suiteName;
        result.name = name;
        result.description = description;
        result.inputRange = m_inputRange;
        result.samplesInRange = m_inputValues.size();
        result.overheadNs = m_overheadNs;
        // make sure we use a volatile destination, so values are not thrown away.
        volatile storage_t dummy{};
        const input_t* inputData = m_inputValues.data();
        // start speed measurement
        auto startSpeed = std::chrono::high_resolution_clock::now();
        for (uint_fast64_t j = 0; j < LOOPCOUNT; ++j)
        {
            for (uint_fast64_t i = 0; i < result.samplesInRange; ++i)
            {
                dummy = approx(inputData[i]);
            }
        }
        auto speedDuration = std::chrono::high_resolution_clock::now() - startSpeed;
        result.callNs = std::chrono::duration_cast<std::chrono::nanoseconds>(speedDuration).count() / LOOPCOUNT;
        // now check precision
        for (uint_fast64_t i = 0; i < result.samplesInRange; ++i)
        {
            storage_t a = approx(inputData[i]);
            result.values.push_back(a);
            // calculate absolute and relative errors
            auto v = m_referenceValues[i];
            result.absoluteErrors.values.push_back(abs(a - v));
            result.relativeErrors.values.push_back(v != 0.0 ? abs(1.0 - a / v) : 0.0);
        }
        // calculate error statistics
        calculateErrorStatistics(result.absoluteErrors);
        calculateErrorStatistics(result.relativeErrors);
        // calculate stddev
        result.stddev = stddev(result.absoluteErrors.values);
        return result;
    }

  private:
    static constexpr uint_fast64_t LOOPCOUNT = 10000;
    const std::string m_suiteName;
    const input_range_t m_inputRange{};
    const std::vector<input_t> m_inputValues;
    std::vector<storage_t> m_referenceValues;
    uint64_t m_overheadNs = 0;
    volatile input_t m_dummy{};
};

template <typename InputT, typename StorageT>
std::ostream& operator<<(std::ostream& os, const Result<InputT, StorageT>& r)
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

template <typename InputT>
std::ostream& operator<<(std::ostream& os, const std::pair<InputT, InputT>& v)
{
    os << "(" << v.first << ", " << v.second << ")";
    return os;
}

template <typename... Ts>
std::ostream& operator<<(std::ostream& os, const std::tuple<Ts...> t)
{
    os << '(';
    apply([&](auto&&... args)
          { ((os << args << ", "), ...); },
          t);
    os << "\b\b";
    os << ')';
    return os;
}

template <typename InputT, typename StorageT>
std::ostream& operator<<(std::ostream& os, const std::vector<Result<InputT, StorageT>>& rs)
{
    const auto& fr = rs.front();
    os << "Testing: " << fr.suiteName << std::endl;
    os << "Input range: ";
    os << "(" << fr.inputRange.first << ", " << fr.inputRange.second << "), ";
    os << fr.samplesInRange << " samples in range" << std::endl;
    os << "Approximate loop and call overhead (already subtracted): " << float(fr.overheadNs) / float(fr.samplesInRange) << " ns / call" << std::endl;
    os << "Tested functions:" << std::endl
       << std::endl;
    for (const auto& r : rs)
    {
        os << r << std::endl;
    }
    return os;
}
