#pragma once

#include "result.h"
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>

static const std::string HTML5BoilerPlateStart =
    " \
<!DOCTYPE html>\n \
<html lang=\"en\">\n \
<head>\n \
	<meta charset=\"utf-8\">\n \
    <style>\n \
        .center {\n \
            display: block;\n \
            margin-left: auto;\n \
            margin-right: auto;\n \
            width: 90 % ;\n \
            text-align: center;\n \
        }\n \
        .centercontainer {\n \
            margin: 0 auto;\n \
        }\n \
        #results {\n \
            border-collapse: collapse;\n \
            width: 100%;\n \
        }\n \
        #results td, #results th {\n \
            font-size: 80%;\n \
            text-align: center;\n \
            border: 1px solid #ddd;\n \
        }\n \
        #results tr:hover {\n \
            background-color: #ddd;\n \
        }\n \
        #results th {\n \
            padding-top: 12px;\n \
            padding-bottom: 12px;\n \
            background-color: #aaa;\n \
            color: white;\n \
        }\n \
    </style>\n \
	<title>approx results</title>\n \
</head>\n \
<body id=\"home\">";

static const std::string HTML5BoilerPlateEnd =
    " \
</body>\n \
</html>";

template <typename ResultT>
void td(std::ostream& os, const typename ResultT::Errors& e)
{
    os.precision(3);
    os << "<td>" << e.minimum << "</td>" << std::endl;
    os << "<td>" << e.maximum << "</td>" << std::endl;
    os << "<td>" << e.mean << "</td>" << std::endl;
    os << "<td>" << e.median << "</td>" << std::endl;
    os << "<td>" << e.variance << "</td>" << std::endl;
}

template <typename ResultT>
void td(std::ostream& os, const ResultT& r)
{
    os << "<tr>" << std::endl;
    os << "<td>" << r.description << "</td>" << std::endl;
    td<ResultT>(os, r.absoluteErrors);
    td<ResultT>(os, r.relativeErrors);
    os << "<td>" << r.stddev << "</td>" << std::endl;
    os << "<td>" << float(r.callNs - r.overheadNs) / float(r.samplesInRange) << "</td>" << std::endl;
    os << "</tr>" << std::endl;
}

template <typename ResultT>
void table(std::ostream& os, const std::vector<ResultT>& rs)
{
    os << "<div class=\"centercontainer\">" << std::endl;
    os << "<table id=\"results\">" << std::endl;
    os << "<thead>" << std::endl;
    os << "<tr>" << std::endl;
    os << "<th></th>" << std::endl;
    os << "<th colspan=5>Absolute error</th>" << std::endl;
    os << "<th colspan=5>Relative error</th>" << std::endl;
    os << "<th></th>" << std::endl;
    os << "<th></th>" << std::endl;
    os << "</tr>" << std::endl;
    os << "<tr>" << std::endl;
    os << "<th>Method</th>" << std::endl;
    os << "<th>Min.</th>" << std::endl;
    os << "<th>Max.</th>" << std::endl;
    os << "<th>Mean</th>" << std::endl;
    os << "<th>Median</th>" << std::endl;
    os << "<th>Var.</th>" << std::endl;
    os << "<th>Min.</th>" << std::endl;
    os << "<th>Max.</th>" << std::endl;
    os << "<th>Mean</th>" << std::endl;
    os << "<th>Median</th>" << std::endl;
    os << "<th>Var.</th>" << std::endl;
    os << "<th>stddev</th>" << std::endl;
    os << "<th>Execution time</br>[ns / call]</th>" << std::endl;
    os << "</tr>" << std::endl;
    os << "</thead>" << std::endl;
    for (const auto& r : rs)
    {
        td(os, r);
    }
    os << "</table>" << std::endl;
    os << "</div>" << std::endl;
}

template <typename ResultT>
void html(const std::vector<ResultT>& rs, const std::string& fileName, const std::string& plotFileName)
{
    const auto& fr = rs.front();
    std::ofstream htmlFile(fileName);
    htmlFile << HTML5BoilerPlateStart << std::endl;
    //htmlFile << "<h1 class=\"center\">Results for " << fr.suiteName << "</h1>" << std::endl;
    htmlFile << "<img src=\"" << plotFileName << "\" alt=\"result plot\" class=\"center\">" << std::endl;
    table(htmlFile, rs);
    htmlFile << HTML5BoilerPlateEnd;
}
