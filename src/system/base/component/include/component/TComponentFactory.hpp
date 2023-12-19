#pragma once

#include <sstream>
#include <iostream>
#include <initializer_list>

#include "IComponentFactory.hpp"
#include "interface/TInterface.hpp"
#include "interface/TInterfaces.hpp"

#define FOREACH_TEMPLATE_PACK(expression)                 \
    {                                                     \
        std::initializer_list<int>{((expression), 0)...}; \
    }

namespace HBE {
namespace {

using InterfaceInfoMap = std::map<std::string, InterfaceTypeInfo>;

template <typename InterfaceDescription>
void pushInterface(InterfaceInfoMap& info)
{
    info.insert({InterfaceDescription::getName(),
                 TInterface<typename InterfaceDescription::Type>{}.getInterfaceTypeInfo()});
}

template <typename... InterfaceDescriptions>
InterfaceInfoMap fillInfo(std::tuple<InterfaceDescriptions...>*)
{
    InterfaceInfoMap mp;
    FOREACH_TEMPLATE_PACK(pushInterface<InterfaceDescriptions>(mp));
    return mp;
}

template <typename ComponentType, typename ConfigType>
struct InstanceCreatorPrivate
{
    IComponentPtr create(
        ConfigType const& config,
        const HBE::ResolvedDependencies dependencies)
    {
        return std::make_shared<ComponentType>(config, dependencies);
    }
};

template <typename ComponentType>
struct InstanceCreatorPrivate<ComponentType, void>
{
    IComponentPtr create(const HBE::ResolvedDependencies dependencies)
    {
        return std::make_shared<ComponentType>(dependencies);
    }
};

template <typename ComponentType, typename ConfigType>
struct InstanceCreator
{
    IComponentPtr create(boost::property_tree::ptree cfg, const HBE::ResolvedDependencies dependencies)
    {
        ConfigType config{cfg};
        return InstanceCreatorPrivate<ComponentType, ConfigType>{}.create(config, dependencies);
    }
};

template <typename ComponentType>
struct InstanceCreator<ComponentType, void>
{
    IComponentPtr create(boost::property_tree::ptree cfg, const HBE::ResolvedDependencies dependencies)
    {
        return InstanceCreatorPrivate<ComponentType, void >{}.create(dependencies);
    }
};

} // namespace

template <typename ComponentType>
class TComponentFactory : public IComponentFactory
{
public:
    ProvidedInterfacesInfo getProvidedInterfaces() override
    {
        return fillInfo(static_cast<typename ComponentType::Interfaces::TTypes*>(nullptr));
    }

    DependenciesInfo getDependencies() override
    {
        return fillInfo(static_cast<typename ComponentType::Depends::TTypes*>(nullptr));
    }

    IComponentPtr createInstance(boost::property_tree::ptree config, const HBE::ResolvedDependencies dependencies) override
    {
        return InstanceCreator<ComponentType, typename ComponentType::ConfigType>{}.create(config, dependencies);
    }

};

} // namespace HBE
