#pragma once

#include <map>
#include <memory>
#include <string>

#include "interface/IInterface.hpp"

namespace HBE {

using ProvidedInterfaces = std::map<std::string, IInterfaceWPtr>;
using ProvidedInterfacesInfo = std::map<std::string, InterfaceTypeInfo>;

class IComponent
{
public:
    virtual ~IComponent() = default;

    ///
    /// @brief register my interfaces
    ///
    virtual void registerInterfaces() = 0;

    ///
    /// @brief init component, subscribe for events, connect to interfaces and so on
    ///
    virtual void initComponent() = 0;

    ///
    /// @brief start component, do component main cycle
    ///
    virtual void startComponent() = 0;

    ///
    /// @brief return provided interfaces list
    ///
    virtual ProvidedInterfaces const& getProvidedInterfaces() = 0;
};

using IComponentPtr  = std::shared_ptr<IComponent>;
using IComponentUPtr = std::unique_ptr<IComponent>;
using IComponentWPtr = std::weak_ptr<IComponent>;
} // namespace HBE