#pragma once

#include <map>
#include <memory>
#include <functional>

namespace HBE {

struct InterfaceTypeInfo final
{
    std::string type_name;
};

class IInterface
{
public:
    virtual ~IInterface() = default;

    virtual InterfaceTypeInfo getInterfaceTypeInfo() const = 0;
};

using IInterfacePtr  = std::shared_ptr<IInterface>;
using IInterfaceUPtr = std::unique_ptr<IInterface>;
using IInterfaceWPtr = std::weak_ptr<IInterface>;

template <typename T>
using TInterfaceWPtr = std::weak_ptr<T>;

} // namespace HBE
