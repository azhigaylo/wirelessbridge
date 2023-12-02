#pragma once

#include <map>
#include <string>

#include "interface/IInterface.hpp"

namespace HBE {

using DependenciesInfo = std::map<std::string, InterfaceTypeInfo>;
using ResolvedDependencies = std::map<std::string, IInterfaceWPtr>;

} // namespace HBE
