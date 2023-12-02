#pragma once

#include "IInterface.hpp"

namespace HBE {

template <typename InterfaceName>
class TInterface : public IInterface
{
public:
    InterfaceTypeInfo getInterfaceTypeInfo() const override
    {
        return TDefinedInterface<InterfaceName>::getInterfaceTypeInfo();
    }
};

} // namespace HBE