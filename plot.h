#pragma once

#include <algorithm>
#include <functional>
#include <sciplot/sciplot.hpp>
#include "result.h"

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
void plotLines(sciplot::multiplot& mp, const std::vector<Result<T>>& rs, std::function<const std::vector<T>&(const Result<T>&)> valuesFunc, T sizePercent, const std::string& title, const std::string& yLabel)
{
    const auto rangePercent = calculateMedianRange(rs, valuesFunc, sizePercent);
    const auto& fr = rs.front();
    const auto x = sciplot::linspace(fr.inputRange.first, fr.inputRange.second, fr.samplesInRange);
    sciplot::plot p;
    p.tics();
    p.xlabel("");
    p.legend().header(title);
    p.border().linewidth(1);
    p.ylabel(yLabel);
    p.yrange(rangePercent.first, rangePercent.second);
    for (const auto& r : rs)
    {
        p.draw(x, valuesFunc(r)).title(r.description).linewidth(1);
    }
    mp.add(p);
    //p.show();
    //p.save(title + ".pdf");
}

template <typename T>
void plotBars(sciplot::multiplot& mp, const std::vector<Result<T>>& rs, std::function<T(const Result<T>&)> valueFunc, const std::string& title, const std::string& yLabel)
{
    const auto range = calculateRange(rs, valueFunc);
    const auto& fr = rs.front();
    sciplot::plot p;
    p.tics();
    p.xlabel("");
    p.legend().header(title);
    p.border().linewidth(1);
    p.ylabel(yLabel);
    p.yrange(range.first, range.second);
    p.boxwidth(sciplot::boxwidthtype::relative, 0.75F);
    std::vector<std::string> x;
    std::vector<T> y;
    for (decltype(rs.size()) i = 0; i < rs.size(); i++)
    {
        x.push_back(rs[i].description);
        y.push_back(valueFunc(rs[i]));
    }
    p.draw(x, y).with(sciplot::plotstyle::boxes).title(title).fillstyle(sciplot::fillstyle::solid).use(sciplot::plotspecs::USE_AUTO, 2, sciplot::plotspecs::USE_AUTO, 1);
    mp.add(p);
    //p.show();
    //p.save(title + ".pdf");
}

template <typename T>
void plot(const std::vector<Result<T>>& rs)
{
    const auto& fr = rs.front();
    sciplot::multiplot mp;
    mp.size(600, 400);
    mp.title("Results for " + fr.suiteName);
    mp.layout(2, 2);
    std::function<const std::vector<T>&(const Result<T>&)> valueFunc = [](const Result<T>& r) -> const std::vector<T>& { return r.values; };
    plotLines(mp, rs, valueFunc, (T)98, "value", "f(x)");
    std::function<const std::vector<T>&(const Result<T>&)> absFunc = [](const Result<T>& r) -> const std::vector<T>& { return r.absoluteErrors.values; };
    plotLines(mp, rs, absFunc, (T)80, "abs. error", "|f(x) - F(x)|");
    std::function<const std::vector<T>&(const Result<T>&)> relFunc = [](const Result<T>& r) -> const std::vector<T>& { return r.relativeErrors.values; };
    plotLines(mp, rs, relFunc, (T)80, "rel. error", "|1 - f(x) / F(x)|");
    std::function<T(const Result<T>&)> callNsFunc = [](const Result<T>& r) { return float(r.callNs - r.overheadNs) / (float)r.samplesInRange; };
    plotBars(mp, rs, callNsFunc, "", "Execution time / call [ns]");
    mp.show();
    mp.save("result.pdf");
}