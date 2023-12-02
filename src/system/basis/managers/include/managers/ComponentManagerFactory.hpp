#pragma once

#include "IComponentManager.hpp"
#include "helpers/IComponentConfigParser.hpp"

namespace HBE {

class ComponentManagerFactory
{
public:
    static IComponentManagerPtr createInstance();
    static IComponentConfigParserPtr createConfiguration(std::string const cfg_path);
};

} // namespace HBE
