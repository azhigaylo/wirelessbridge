#pragma once

#include <map>
#include <tuple>
#include <memory>

#include "DependenciesTypes.hpp"

namespace HBE {

template <typename... Dependencies>
class TDepends final
{
public:
    using TTypes = std::tuple<Dependencies...>;

    explicit TDepends(const ResolvedDependencies dependencies)
    : m_dependnecies{cast<Dependencies>(dependencies)...}
    {
    }

    template <typename Dependency>
    TInterfaceWPtr<typename Dependency::Type> getInterface() const noexcept
    {
        return std::get<Dependency>(m_dependnecies).instance;
    }

private:

    template <typename Dependency>
    Dependency cast(const ResolvedDependencies dependencies)
    {
        auto const it = dependencies.find(Dependency::getName());
        if (it == dependencies.end())
        {
            const std::string error = "Cannot find instance for dependency " + Dependency::getName();
            throw std::runtime_error(error);
        }

        auto interface_instance = it->second.lock(); // wptr on interface
        auto const actual_info = interface_instance->getInterfaceTypeInfo();
        auto const requested_info = TInterface<typename Dependency::Type>{}.getInterfaceTypeInfo();

        if (actual_info.type_name != requested_info.type_name)
        {
            const std::string error = "Type mismatch in dependency " + Dependency::getName() +
                                    ". Requested: " + requested_info.type_name +
                                    ", Actual: " + actual_info.type_name;
            throw std::runtime_error(error);
        }

        return Dependency{std::dynamic_pointer_cast<typename Dependency::Type>(interface_instance)};
    }

private:
    TTypes m_dependnecies;
};

} // namespace HBE
