#pragma once

#include "result.h"
#include <sciplot/sciplot.hpp>
#include <functional>
#include <algorithm>

template <typename T>
std::pair<T, T> calculateMedianRange(const std::vector<Result<T>> &rs, std::function<const std::vector<T> &(const Result<T> &)> valuesFunc, T sizePercent)
{
    // calculate min / max percentage range of all values around median
    std::pair<T, T> rangeMinMax(std::numeric_limits<T>::max(), std::numeric_limits<T>::lowest());
    for (const auto &r : rs)
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
void plot(const std::vector<Result<T>> &rs, std::function<const std::vector<T> &(const Result<T> &)> valuesFunc, T sizePercent, const std::string &title, const std::string &yLabel)
{
    const auto rangePercent = calculateMedianRange(rs, valuesFunc, sizePercent);
    const auto &fr = rs.front();
    const auto x = sciplot::linspace(fr.inputRange.first, fr.inputRange.second, fr.samplesInRange);
    sciplot::plot p;
    p.tics().fontsize(8);
    p.xlabel("").fontsize(8);
    p.legend().header(title).fontsize(12);
    p.border().linewidth(1);
    p.ylabel(yLabel).fontsize(8);
    p.yrange(rangePercent.first, rangePercent.second);
    for (const auto &r : rs)
    {
        p.draw(x, valuesFunc(r)).title(r.description).linewidth(1);
    }
    p.show();
    //p.save(title + ".pdf");
}

template <typename T>
void plotBars(const std::vector<Result<T>> &rs, std::function<T(const Result<T> &)> valueFunc, const std::string &title, const std::string &yLabel)
{
    const auto &fr = rs.front();
    sciplot::plot p;
    p.tics().fontsize(8);
    p.xlabel("").fontsize(8);
    p.legend().header(title).fontsize(12);
    p.border().linewidth(1);
    p.ylabel(yLabel).fontsize(8);
    std::vector<std::string> x;
    std::vector<T> y;
    for (decltype(rs.size()) i = 0; i < rs.size(); i++)
    {
        x.push_back("\"" + rs[i].description + "\"");
        y.push_back(valueFunc(rs[i]));
    }
    p.draw(x, y).with(sciplot::plotstyle::boxes).title(title).linewidth(1);
    p.show();
    //p.save(title + ".pdf");
}

template <typename T>
void plot(const std::vector<Result<T>> &rs)
{
    const auto &fr = rs.front();
    std::function<const std::vector<T> &(const Result<T> &)> valueFunc = [](const Result<T> &r) -> const std::vector<T> & { return r.values; };
    plot(rs, valueFunc, (T)98, fr.suiteName + " value", "f(x)");
    std::function<const std::vector<T> &(const Result<T> &)> absFunc = [](const Result<T> &r) -> const std::vector<T> & { return r.absoluteErrors; };
    plot(rs, absFunc, (T)80, fr.suiteName + " abs. error", "abs(f(x) - ref(x))");
    std::function<const std::vector<T> &(const Result<T> &)> relFunc = [](const Result<T> &r) -> const std::vector<T> & { return r.relativeErrors; };
    plot(rs, relFunc, (T)80, fr.suiteName + " rel. error", "abs(1 - f(x) / ref(x))");
    /*std::function<T(const Result<T> &)> callNsFunc = [](const Result<T> &r) { return float(r.callNs - r.overheadNs) / (float)r.samplesInRange; };
    plotBars(rs, callNsFunc, fr.suiteName + "_time", "Execution time / call (ns)");*/
}