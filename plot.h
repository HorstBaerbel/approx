#pragma once

#include "result.h"
#include <algorithm>
#include <functional>
#include <sciplot/sciplot.hpp>
#include <string>
#include <vector>

template <typename ResultT>
typename ResultT::storage_range_t calculateRange(const std::vector<ResultT>& rs, std::function<typename ResultT::storage_t(const ResultT&)> valueFunc)
{
    // get values from result
    std::vector<typename ResultT::storage_t> values;
    for (const auto& r : rs)
    {
        values.push_back(valueFunc(r));
    }
    // calculate min / max percentage range of values
    auto mme = std::minmax_element(values.cbegin(), values.cend());
    return std::make_pair(*mme.first, *mme.second);
}

template <typename ResultT>
typename ResultT::storage_range_t calculateMedianRange(const std::vector<ResultT>& rs, std::function<typename ResultT::storage_t(const ResultT&)> valueFunc, typename ResultT::storage_t sizePercent)
{
    // calculate median of values
    std::vector<typename ResultT::storage_t> v;
    std::transform(rs.cbegin(), rs.cend(), std::back_inserter(v), [&valueFunc](const auto& r)
                   { return valueFunc(r); });
    std::nth_element(v.begin(), v.begin() + v.size() / 2, v.end());
    auto medianIt = v.cbegin() + v.size() / 2;
    // now get sizePercent values centered around median
    const decltype(v.size()) halfRange = v.size() / 2 * sizePercent / 100;
    auto leftIt = std::prev(medianIt, halfRange);
    auto rightIt = std::next(medianIt, halfRange);
    return std::make_pair(*leftIt, *rightIt);
}

template <typename ResultT>
typename ResultT::storage_range_t calculateMedianRange(const std::vector<ResultT>& rs, std::function<const std::vector<typename ResultT::storage_t>&(const ResultT&)> valuesFunc, typename ResultT::storage_t sizePercent)
{
    // calculate min / max percentage range of all values around median
    typename ResultT::storage_range_t rangeMinMax(std::numeric_limits<typename ResultT::storage_t>::max(), std::numeric_limits<typename ResultT::storage_t>::lowest());
    for (const auto& r : rs)
    {
        // calculate median of values
        std::vector<typename ResultT::storage_t> v = valuesFunc(r);
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

template <typename ResultT>
sciplot::Plot plotLines(const std::vector<ResultT>& rs, std::function<const std::vector<typename ResultT::storage_t>&(const ResultT&)> valuesFunc, typename ResultT::storage_t sizePercent, const std::string& title, const std::string& yLabel)
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

template <typename ResultT>
sciplot::Plot plotBars(const std::vector<ResultT>& rs, std::function<typename ResultT::storage_t(const ResultT&)> valueFunc, typename ResultT::storage_t sizePercent, const std::string& title, const std::string& yLabel)
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
    std::vector<typename ResultT::storage_t> y;
    for (decltype(rs.size()) i = 0; i < rs.size(); i++)
    {
        x.push_back(rs[i].description);
        y.push_back(valueFunc(rs[i]));
    }
    p.drawBoxes(x, y).fillSolid();
    return p;
}

template <typename ResultT>
void plot(const std::vector<ResultT>& rs, const std::string& fileName)
{
    const auto& fr = rs.front();
    std::function<const std::vector<typename ResultT::storage_t>&(const ResultT&)> valueFunc = [](const ResultT& r) -> const std::vector<typename ResultT::storage_t>& { return r.values; };
    auto p0 = plotLines(rs, valueFunc, (typename ResultT::storage_t)98, "Value", "f(x)");
    std::function<const std::vector<typename ResultT::storage_t>&(const ResultT&)> absFunc = [](const ResultT& r) -> const std::vector<typename ResultT::storage_t>& { return r.absoluteErrors.values; };
    auto p1 = plotLines(rs, absFunc, (typename ResultT::storage_t)80, "Absolute error", "|f(x) - F(x)|");
    std::function<const std::vector<typename ResultT::storage_t>&(const ResultT&)> relFunc = [](const ResultT& r) -> const std::vector<typename ResultT::storage_t>& { return r.relativeErrors.values; };
    auto p2 = plotLines(rs, relFunc, (typename ResultT::storage_t)80, "Relative error", "|1 - f(x) / F(x)|");
    std::function<typename ResultT::storage_t(const ResultT&)> callNsFunc = [](const ResultT& r)
    { return float(r.callNs - r.overheadNs) / (float)r.samplesInRange; };
    auto p3 = plotBars(rs, callNsFunc, (typename ResultT::storage_t)70, "", "Execution time [ns / call]");
    sciplot::Figure mp = {{p0, p3}, {p1, p2}};
    mp.size(1200, 800);
    mp.title("Results for " + fr.suiteName);
    mp.save(fileName);
}