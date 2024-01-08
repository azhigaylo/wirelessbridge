/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#pragma once

#include <vector>

#include "managers/IComponentManager.hpp"

#include "component/DependenciesTypes.hpp"
#include "managers/IComponentLoader.hpp"
#include "managers/ILoaderFactory.hpp"

namespace HBE {

class CComponentManager
: public IComponentManager
{
    using ComponentsRegistry = std::map<std::string, IComponentPtr>;

public:
    explicit CComponentManager(ILoaderFactoryPtr int_man_fct);

    ~CComponentManager() final;

    void registerComponent(ComponentConfig const config) final;

    IComponentPtr getComponent(std::string const cmp_name) final;

    void initAndStartComponents() const final;

private:
    ResolvedDependencies resolveDependencies(DependenciesInfo const& dependencies) const;

private:
    IComponentLoaderPtr m_loader;
    ComponentsRegistry m_registered_components;
};

} // namespace HBE


//     IFileHandlerFactoryPtr handler_factory);