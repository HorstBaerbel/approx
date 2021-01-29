// Test spped and precision of transcendental function approximations

#include "html.h"
#include "plot.h"
#include "test_expf.h"
#include "test_invsqrtf.h"
#include "test_log10f.h"
#include "test_sqrtf.h"
#include "test_sqrti.h"
#include <cstdio>
#include <cxxopts.hpp>
#include <fstream>
#include <iostream>
#include <string>

std::string m_approxFunc = "sqrtf";
std::string m_plotFormat = "";

bool readArguments(int argc, char**& argv)
{
    cxxopts::Options options("approx", "Test transcendental function approximations");
    options.allow_unrecognised_options();
    options.add_options()("h,help", "Print help")("p,plot", "Plot results using GNUplot. Supported: \"pdf\" or \"html\"", cxxopts::value<std::string>())("f,function", "Name of function to test. Supported: \"expf\", \"log10f\", \"invsqrtf\", \"sqrti\" or \"sqrtf\"", cxxopts::value<std::string>());
    auto result = options.parse(argc, argv);
    // check if help was requested
    if (result.count("help"))
    {
        return false;
    }
    // check if a function was specified
    if (result.count("function"))
    {
        m_approxFunc = result["function"].as<std::string>();
    }
    else
    {
        std::cout << "No function name passed!" << std::endl;
        return false;
    }
    if (result.count("plot"))
    {
        m_plotFormat = result["plot"].as<std::string>();
    }
    return true;
}

void printUsage()
{
    // 80 chars:  --------------------------------------------------------------------------------
    std::cout << "approx - Test transcendental function approximations" << std::endl;
    std::cout << "Usage: approx (-h, -p FORMAT, -f FUNC)" << std::endl;
    std::cout << "-h: Print usage help." << std::endl;
    std::cout << "-f FUNC: Function to test." << std::endl;
    std::cout << "FUNC can be \"expf\", \"log10f\", \"invsqrtf\", \"sqrtf\" or \"sqrti\"." << std::endl;
    std::cout << "-p FORMAT: Plot test results using GNUplot." << std::endl;
    std::cout << "FORMAT is the result file format. Either \"pdf\" or \"html\"." << std::endl;
    std::cout << "Example: approx -f sqrtf -p pdf" << std::endl;
}

// ----- main -------------------------------------------------------------------------------------

template <typename ResultT>
void output(const std::vector<Result<ResultT>>& results)
{
    // plot results to file using gnuplot
    if (!m_plotFormat.empty() && !results.empty())
    {
        std::string plotFileName = m_plotFormat == "html" ? "result.svg" : "result.pdf";
        if (m_plotFormat == "html")
        {
            html(results, "result.html", plotFileName);
        }
        plot(results, plotFileName);
    }
}

int main(int argc, char** argv)
{
#ifdef _DEBUG
    std::cout << "Please compile and run approx in release mode!" << std::endl;
    return -99;
#endif
    // check arguments
    if (argc < 3 || !readArguments(argc, argv))
    {
        printUsage();
        return -1;
    }
    // check which tests to run
    /*if (m_approxFunc == "expf")
    {
        ExpfTest expfTest(std::make_pair(-88, 88), 10000);
        auto results = expfTest.runTests();
        std::cout << results;
        output(results);
    } else */
    if (m_approxFunc == "log10f")
    {
        Log10Test log10Test(std::make_pair(0, 65535), 10000);
        auto results = log10Test.runTests();
        std::cout << results;
        output(results);
    }
    else if (m_approxFunc == "invsqrtf")
    {
        InvSqrtfTest invSqrtTest(std::make_pair(0, 2), 10000);
        auto results = invSqrtTest.runTests();
        std::cout << results;
        output(results);
    }
    else if (m_approxFunc == "sqrtf")
    {
        SqrtfTest sqrtTest(std::make_pair(0, 65535), 10000);
        auto results = sqrtTest.runTests();
        std::cout << results;
        output(results);
    }
    else if (m_approxFunc == "sqrti")
    {
        SqrtiTest sqrtTest(std::make_pair(0, 0xFFFFFFFF), 10000);
        auto results = sqrtTest.runTests();
        std::cout << results;
        output(results);
    }
    else
    {
        std::cout << "Unsupported function \"" << m_approxFunc << "\"" << std::endl;
        return -2;
    }
    return 0;
}
