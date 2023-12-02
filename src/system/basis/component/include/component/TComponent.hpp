#pragma once

#include <iostream>
#include "IComponent.hpp"

namespace HBE {

template <typename ComponentAnnotation>
class TComponent
    : public IComponent
    , public ComponentAnnotation
{
public:

    ProvidedInterfaces const& getProvidedInterfaces() override
    {
        return m_interfaces.getProvidedInterfaces();
    }

protected:
    template <typename InterfaceDescription>
    void provideInterface(std::weak_ptr<typename InterfaceDescription::Type> interface)
    {
        m_interfaces.template provideInterface<InterfaceDescription>(interface);
    }

private:
    typename ComponentAnnotation::Interfaces m_interfaces;
};

} // namespace HBE
