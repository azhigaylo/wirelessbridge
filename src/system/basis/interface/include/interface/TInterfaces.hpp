#pragma once

#include <memory>

#include "component/IComponent.hpp"

namespace HBE {

template <typename... InterfaceDescriptions>
class TInterfaces final
{
public:
    using TTypes = std::tuple<InterfaceDescriptions...>;

public:
    template <typename InterfaceDescription>
    void provideInterface(TInterfaceWPtr<typename InterfaceDescription::Type> interface)
    {
        m_interfaces.insert({InterfaceDescription::getName(), interface});
    }

    ProvidedInterfaces const& getProvidedInterfaces()
    {
        return m_interfaces;
    }

private:
    ProvidedInterfaces m_interfaces;
};

} // namespace HBE
