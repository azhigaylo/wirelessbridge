#pragma once

#include <memory>
#include <boost/property_tree/json_parser.hpp>

#include "IComponent.hpp"
#include "DependenciesTypes.hpp"

namespace HBE {

class IComponentFactory
{
public:
    virtual ~IComponentFactory() = default;

    virtual ProvidedInterfacesInfo getProvidedInterfaces() = 0;

    virtual DependenciesInfo getDependencies() = 0;

    virtual IComponentPtr createInstance(boost::property_tree::ptree config, const HBE::ResolvedDependencies dependencies) = 0;
};

using IComponentFactoryPtr  = std::shared_ptr<IComponentFactory>;
using IComponentFactoryUPtr = std::unique_ptr<IComponentFactory>;

} // namespace HBE
