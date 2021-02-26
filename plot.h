#pragma once

#include "result.h"
#include <algorithm>
#include <functional>
#include <sciplot/sciplot.hpp>
#include <string>
#include <vector>

template <typename T>
std::pair<T, T> calculateRange(const std::vector<Result<T>>& rs, std::function<T(const Result<T>&)> valueFunc)
{
    // get values from result
    std::vector<T> values;
    for (const auto& r : rs)
    {
        values.push_back(valueFunc(r));
    }
    // calculate min / max percentage range of values
    auto mme = std::minmax_element(values.cbegin(), values.cend());
    return std::make_pair(*mme.first, *mme.second);
}

template <typename T>
std::pair<T, T> calculateMedianRange(const std::vector<Result<T>>& rs, std::function<T(const Result<T>&)> valueFunc, T sizePercent)
{
    // calculate median of values
    std::vector<T> v;
    std::transform(rs.cbegin(), rs.cend(), std::back_inserter(v), [&valueFunc](const auto& r) { return valueFunc(r); });
    std::nth_element(v.begin(), v.begin() + v.size() / 2, v.end());
    auto medianIt = v.cbegin() + v.size() / 2;
    // now get sizePercent values centered around median
    const decltype(v.size()) halfRange = v.size() / 2 * sizePercent / 100;
    auto leftIt = std::prev(medianIt, halfRange);
    auto rightIt = std::next(medianIt, halfRange);
    return std::make_pair(*leftIt, *rightIt);
}

template <typename T>
std::pair<T, T> calculateMedianRange(const std::vector<Result<T>>& rs, std::function<const std::vector<T>&(const Result<T>&)> valuesFunc, T sizePercent)
{
    // calculate min / max percentage range of all values around median
    std::pair<T, T> rangeMinMax(std::numeric_limits<T>::max(), std::numeric_limits<T>::lowest());
    for (const auto& r : rs)
    {
        // calculate median of values
        std::vector<T> v = valuesFunc(r);
        std::nth_element(v.begin(), v.begin() + v.size() / 2, v.end());
        auto medianIt = v.cbegin() + v.size() / 2;
        // now get sizePercent values centered around median
        const decltype(v.size()) halfRange = v.size() / 2 * sizePercent / 100;
        auto leftIt = std::prev(medianIt, halfRange);
        auto rightIt = std::next(medianIt, halfRange);
        rangeMinMax.first = std::min(rangeMinMax.first, *leftIt);
        rangeMinMax.second = std::max(rangeMinMax.second, *rightIt);
    }
    return rangeMinMax;
}

template <typename T>
sciplot::Plot plotLines(const std::vector<Result<T>>& rs, std::function<const std::vector<T>&(const Result<T>&)> valuesFunc, T sizePercent, const std::string& title, const std::string& yLabel)
{
    auto rangePercent = calculateMedianRange(rs, valuesFunc, sizePercent);
    rangePercent.second = rangePercent.second - rangePercent.first == 0 ? rangePercent.first + 1 : rangePercent.second;
    const auto& fr = rs.front();
    const auto x = sciplot::linspace(fr.inputRange.first, fr.inputRange.second, fr.samplesInRange);
    sciplot::Plot p;
    p.tics();
    p.xlabel("");
    p.legend().title(title);
    p.border().lineWidth(1);
    p.ylabel(yLabel);
    p.yrange(rangePercent.first, rangePercent.second);
    for (const auto& r : rs)
    {
        p.drawCurve(x, valuesFunc(r)).label(r.description).lineWidth(1);
    }
    return p;
}

template <typename T>
sciplot::Plot plotBars(const std::vector<Result<T>>& rs, std::function<T(const Result<T>&)> valueFunc, T sizePercent, const std::string& title, const std::string& yLabel)
{
    auto rangePercent = calculateMedianRange(rs, valueFunc, sizePercent);
    rangePercent.second = rangePercent.second - rangePercent.first == 0 ? rangePercent.first + 1 : rangePercent.second;
    const auto& fr = rs.front();
    sciplot::Plot p;
    p.tics();
    p.xlabel("");
    p.legend().hide();
    p.xtics().rotate();
    //p.legend().title(title);
    p.border().lineWidth(1);
    p.ylabel(yLabel);
    p.yrange(0, rangePercent.second);
    p.boxWidthRelative(0.75F);
    std::vector<std::string> x;
    std::vector<T> y;
    for (decltype(rs.size()) i = 0; i < rs.size(); i++)
    {
        x.push_back(rs[i].description);
        y.push_back(valueFunc(rs[i]));
    }
    p.drawBoxes(x, y).fillSolid();
    return p;
}

template <typename T>
void plot(const std::vector<Result<T>>& rs, const std::string& fileName)
{
    const auto& fr = rs.front();
    std::function<const std::vector<T>&(const Result<T>&)> valueFunc = [](const Result<T>& r) -> const std::vector<T>& { return r.values; };
    auto p0 = plotLines(rs, valueFunc, (T)98, "Value", "f(x)");
    std::function<const std::vector<T>&(const Result<T>&)> absFunc = [](const Result<T>& r) -> const std::vector<T>& { return r.absoluteErrors.values; };
    auto p1 = plotLines(rs, absFunc, (T)80, "Absolute error", "|f(x) - F(x)|");
    std::function<const std::vector<T>&(const Result<T>&)> relFunc = [](const Result<T>& r) -> const std::vector<T>& { return r.relativeErrors.values; };
    auto p2 = plotLines(rs, relFunc, (T)80, "Relative error", "|1 - f(x) / F(x)|");
    std::function<T(const Result<T>&)> callNsFunc = [](const Result<T>& r) { return float(r.callNs - r.overheadNs) / (float)r.samplesInRange; };
    auto p3 = plotBars(rs, callNsFunc, (T)70, "", "Execution time [ns / call]");
    sciplot::Figure mp = {{p0, p3}, {p1, p2}};
    mp.size(1200, 800);
    mp.title("Results for " + fr.suiteName);
    mp.save(fileName);
}