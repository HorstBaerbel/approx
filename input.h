#pragma once

#include <cstdint>
#include <random>
#include <tuple>
#include <vector>

template <typename T>
std::vector<T> generateLinearX(const std::pair<T, T>& range, uint64_t samplesInRange)
{
    std::vector<T> values;
    for (uint_fast64_t i = 0; i < samplesInRange; ++i)
    {
        values.push_back(range.first + ((range.second - range.first) * i) / (samplesInRange - 1));
    }
    return values;
}

template <typename T>
std::vector<T> generateRandomXY(const std::pair<T, T>& range, uint64_t samplesInRange)
{
    std::vector<T> values;
    values.push_back({0, 0});
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<typename T::first_type> distX(range.first.first, range.second.first);
    std::uniform_real_distribution<typename T::second_type> distY(range.first.second, range.second.second);
    for (uint_fast64_t i = 0; i < samplesInRange - 1; ++i)
    {
        values.push_back({distX(mt), distY(mt)});
    }
    return values;
}

template <typename T>
std::vector<T> generateCirclesXY(const std::pair<T, T>& range, uint64_t samplesInRange)
{
    auto xA = std::get<0>(range.first);
    auto xB = std::get<0>(range.second);
    auto yA = std::get<1>(range.first);
    auto yB = std::get<1>(range.second);
    const auto xMin = std::min(xA, xB);
    const auto xMax = std::max(xA, xB);
    const auto xRange = xMax - xMin;
    const auto xCenter = xMin + 0.5 * xRange;
    const auto yMin = std::min(yA, yB);
    const auto yMax = std::max(yA, yB);
    const auto yRange = yMax - yMin;
    const auto yCenter = yMin + 0.5 * yRange;
    std::vector<T> values;
    values.push_back({0, 0});
    const auto steps = static_cast<uint64_t>(std::trunc(std::sqrt(samplesInRange - 1)));
    const double stepR = (0.5 * std::min(xRange, yRange)) / steps;
    const double stepT = (2.0 * M_PI) / steps;
    for (uint_fast64_t ri = 1; ri <= steps; ++ri)
    {
        const double r = ri * stepR;
        for (uint_fast64_t ti = 0; ti < steps; ++ti)
        {
            const double t = ti * stepT;
            values.push_back({r * std::cos(t), r * std::sin(t)});
        }
    }
    return values;
}