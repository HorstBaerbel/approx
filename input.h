#pragma once

#include <cstdint>
#include <random>
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