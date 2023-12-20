#include "CComponentManager.hpp"

#include <sstream>

#include "common/slog.h"
#include "managers/CComponentRegisterException.hpp"

namespace HBE {

CComponentManager::CComponentManager(ILoaderFactoryPtr loader_fct)
    : m_loader (loader_fct->createLoaderInstance())
{
    printDebug("CComponentManager/%s: created...", __FUNCTION__);
}

CComponentManager::~CComponentManager()
{
    printDebug("CComponentManager/%s: closing...", __FUNCTION__);
}

void CComponentManager::registerComponent(ComponentConfig const config)
{
    try
    {
        printDebug("CComponentManager/%s: '%s'", __FUNCTION__, config.name.c_str());

        // create component factory
        IComponentFactoryPtr component_factory = m_loader->loadComponent(config.lib_path);

        // get component dependency
        ResolvedDependencies resolved_dep = resolveDependencies(component_factory->getDependencies());

        // create component
        IComponentPtr component = component_factory->createInstance(config.config, resolved_dep);

        // register all components interfaces
        component->registerInterfaces();

        // get all component intefaces and map them to component
        ProvidedInterfaces const& provided_interfaces = component->getProvidedInterfaces();

        for (const std::pair<std::string, IInterfaceWPtr> interface_pair : provided_interfaces)
        {
            IInterfacePtr interface_ptr = interface_pair.second.lock();

            std::stringstream ss;
            ss << "Component '" << config.name << "' implements interface '" << interface_pair.first
            << "' of type " << interface_ptr->getInterfaceTypeInfo().type_name;

            printDebug("CComponentManager/%s: %s", __FUNCTION__, ss.str().c_str());

            m_registered_components.insert({config.name, component});
        }
    }
    catch (std::exception &e)
    {
        printError("CComponentManager/%s: error during load component = %s", __FUNCTION__, config.name.c_str());
        throw CComponentRegisterException(e.what());
    }
}

IComponentPtr CComponentManager::getComponent(std::string const cmp_name)
{
    IComponentPtr cmp_ptr{nullptr};

    if (false == m_registered_components.empty())
    {
        auto cmp = m_registered_components.find(cmp_name);

        if (cmp != m_registered_components.end())
        {
            cmp_ptr = cmp->second;
        }
        else
        {
            std::stringstream ss;
            ss << "Component '" << cmp_name << "' is absent in registry";
            printWarning("CComponentManager/%s: %s", __FUNCTION__, ss.str().c_str());
            throw CComponentRegisterException(ss.str());
        }
    }
    else
    {
        std::stringstream ss;
        ss << "Component map is empty";
        printWarning("CComponentManager/%s: %s", __FUNCTION__, ss.str().c_str());
        throw CComponentRegisterException(ss.str());
    }

    return cmp_ptr;
}

void CComponentManager::initAndStartComponents() const
{

    // @brief init component, subscribe for events, connect to interfaces and so on
    for (const std::pair<std::string, IComponentPtr> component_pair : m_registered_components)
    {
        try
        {
            component_pair.second->initComponent();
        }
        catch (std::exception &e)
        {
            printError("CComponentManager/%s: error during init component = %s", __FUNCTION__, component_pair.first.c_str());
            throw CComponentRegisterException(e.what());
        }
    }

    // @brief start component, do component main cycle
    for (const std::pair<std::string, IComponentPtr> component_pair : m_registered_components)
    {
        try
        {
            component_pair.second->startComponent();
        }
        catch (std::exception &e)
        {
            printError("CComponentManager/%s: error during start component = %s", __FUNCTION__, component_pair.first.c_str());
            throw CComponentRegisterException(e.what());
        }
    }
}

ResolvedDependencies CComponentManager::resolveDependencies(DependenciesInfo const& dependencies) const
{
    ResolvedDependencies resolved;
    for (const std::pair<std::string, InterfaceTypeInfo>& dependency : dependencies)
    {
        std::stringstream ss_dep;
        ss_dep << "process dependency : "  << " '" << dependency.first << " - " << dependency.second.type_name << "' ";
        printDebug("CComponentManager/%s: %s", __FUNCTION__, ss_dep.str().c_str());

        // try to find nested interface in component map
        bool result = false;
        for (const std::pair<std::string, IComponentPtr> component_pair : m_registered_components)
        {
            ProvidedInterfaces const& provided_interfaces = component_pair.second->getProvidedInterfaces();

            auto interface_itr = provided_interfaces.find(dependency.first);
            if (interface_itr != provided_interfaces.end())
            {
                IInterfacePtr interface_ptr = interface_itr->second.lock();

                if (dependency.second.type_name == interface_ptr->getInterfaceTypeInfo().type_name )
                {
                    printDebug("CComponentManager/%s: dependency satisfied", __FUNCTION__);
                    resolved.insert({dependency.first, interface_itr->second});
                    result = true;
                }
            }
        }
        // if dependency not satisfied - throw ...
        if (false == result)
        {
            std::stringstream ss_wrn;
            ss_wrn << "dependency : "  << " '" << dependency.first << " - " << dependency.second.type_name << "' NOT satisfied !!!";
            printWarning("CComponentManager/%s: %s", __FUNCTION__, ss_wrn.str().c_str());
            throw CComponentRegisterException(ss_wrn.str());
        }
    }
    return resolved;
}

} // namespace HBE



