#pragma once

#include <string>

#include "component/IComponent.hpp"
#include "component/ComponentConfig.hpp"

namespace HBE {

class IComponentManager
{
public:
    virtual ~IComponentManager() = default;

    virtual void registerComponent(ComponentConfig const config) = 0;

    virtual IComponentPtr getComponent(std::string const cmp_name) = 0;

    virtual void initAndStartComponents() const = 0;
};

using IComponentManagerPtr  = std::shared_ptr<IComponentManager>;
using IComponentManagerUPtr = std::unique_ptr<IComponentManager>;

} // namespace HBE
