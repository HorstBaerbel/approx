// Test spped and precision of transcendental function approximations

#include "test_log10f.h"
#include "test_invsqrtf.h"
#include "test_sqrtf.h"
#include "plot.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <cxxopts.hpp>

std::string m_outFile;
std::string m_approxFunc = "sqrtf";
bool m_plot = false;

bool readArguments(int argc, char **&argv)
{
    cxxopts::Options options("approx", "Test transcendental function approximations");
    options.allow_unrecognised_options();
    options.add_options()("h,help", "Print help")("p,plot", "Plot results using GNUplot")("f,function", "Name of function to test. Supported: log10f, invsqrtf and sqrtf", cxxopts::value<std::string>())("o,outfile", "Result CSV file name. Will be overwritten.", cxxopts::value<std::string>())("positional", "", cxxopts::value<std::vector<std::string>>());
    options.parse_positional({"outfile", "positional"});
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
        m_plot = true;
    }
    // get first positional argument as output file
    if (result.count("outfile"))
    {
        m_outFile = result["outfile"].as<std::string>();
    }
    else
    {
        std::cout << "No output file passed!" << std::endl;
        return false;
    }

    return true;
}

void printUsage()
{
    // 80 chars:  --------------------------------------------------------------------------------
    std::cout << "approx - Test transcendental function approximations" << std::endl;
    std::cout << "Usage: approx (-h, -p, -f FUNC) OUTFILE" << std::endl;
    std::cout << "OUTFILE: Result HTML file name. File will be overwritten." << std::endl;
    std::cout << "-h: Print usage help." << std::endl;
    std::cout << "-f FUNC: Function to test. Supported: log10f, invsqrtf and sqrtf." << std::endl;
    std::cout << "-p: Plot test results using GNUplot." << std::endl;
    std::cout << "Example: approx -f sqrtf bla.html" << std::endl;
}

// ----- main -------------------------------------------------------------------------------------

template <typename ResultT>
void plot(const Result<ResultT> &results)
{
    // plot results to file using gnuplot
    if (m_plot && !results.empty())
    {
        plot(results);
    }
}

int main(int argc, char **argv)
{
#ifdef DEBUG
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
    if (m_approxFunc == "log10f")
    {
        Log10Test log10Test(std::make_pair(0, 65535), 100000);
        auto results = log10Test.runTests();
        std::cout << results;
        plot(results);
    }
    else if (m_approxFunc == "invsqrtf")
    {
        InvSqrtfTest invSqrtTest(std::make_pair(0, 2), 100000);
        auto results = invSqrtTest.runTests();
        std::cout << results;
        plot(results);
    }
    else if (m_approxFunc == "sqrtf")
    {
        SqrtfTest sqrtTest(std::make_pair(0, 65535), 100000);
        auto results = sqrtTest.runTests();
        std::cout << results;
        plot(results);
    }
    else
    {
        std::cout << "Unsupported function \"" << m_approxFunc << "\"" << std::endl;
        return -2;
    }
    return 0;
}
