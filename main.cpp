// Test spped and precision of transcendental function approximations

#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#if defined(__GNUC__) || defined(__clang__)
    #include <experimental/filesystem>
    namespace FS_NAMESPACE = std::experimental::filesystem;
#elif defined(_MSC_VER)
    #include <filesystem>
    namespace FS_NAMESPACE = std::tr2::sys;
#endif

#include "cxxopts/include/cxxopts.hpp"
#include "test_sqrtf.h"

std::string m_outFile;
std::string m_approxFunc = "sqrtf";

bool readArguments(int argc, char **&argv)
{
    cxxopts::Options options("approx", "Test transcendental function approximations");
    options.allow_unrecognised_options();
    options.add_options()
        ("h,help", "Print help")
        ("f,function", "Name of function to test. Currently only sqrtf is supported", cxxopts::value<std::string>())
        ("o,outfile", "Result CSV file name. Will be overwritten.", cxxopts::value<std::string>())
        ("positional", "", cxxopts::value<std::vector<std::string>>());
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
    std::cout << "Usage: approx (-h, -f FUNC) OUTFILE" << std::endl;
    std::cout << "OUTFILE: Result HTML file name. File will be overwritten." << std::endl;
    std::cout << "-h: Print usage help." << std::endl;
    std::cout << "-f FUNC: Function to test. Currently only \"sqrtf\" is supported." << std::endl;
    std::cout << "Example: approx -f sqrt bla.html" << std::endl;
}

// ----- main -------------------------------------------------------------------------------------

int main(int argc, char **argv)
{
    // check arguments
    if (argc < 3 || !readArguments(argc, argv))
    {
        printUsage();
        return -1;
    }
    if (m_approxFunc == "sqrtf")
    {
        SqrtfTest sqrtTest(std::make_pair(0, 2), 1000000);
        auto results = sqrtTest.runTests();
        std::cout << results;
    }
    else
    {
        std::cout << "Unsupported function \"" << m_approxFunc << "\"" << std::endl;
        return -2;
    }
    return 0;
}