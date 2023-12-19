#pragma once

#include <map>
#include <string>
#include <boost/property_tree/json_parser.hpp>

namespace HBE {

struct ComponentConfig
{
    std::string name;
    std::string lib_path;
    boost::property_tree::ptree config;
};

} // namespace HBE
