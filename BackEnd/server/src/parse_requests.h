#pragma once

#define BOOST_BIND_GLOBAL_PLACEHOLDERS

#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <vector>
#include <string>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace Capstone {

    bool parse_request(std::string receivedData, std::size_t bytes);

    std::string extract_json(std::string receivedData);

    std::vector<std::vector<double>> parse_matrix(std::string matrixData);
}


