#pragma once

#include <string>

#include "component/IComponentFactory.hpp"

namespace HBE {

class IComponentLoader
{
public:
    virtual ~IComponentLoader() = default;

    virtual IComponentFactoryPtr loadComponent(std::string path) = 0;
};

using IComponentLoaderPtr  = std::shared_ptr<IComponentLoader>;
using IComponentLoaderUPtr = std::unique_ptr<IComponentLoader>;

} // namespace HBE
