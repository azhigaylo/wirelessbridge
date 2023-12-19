#pragma once

#include "IComponentLoader.hpp"

namespace HBE {

class ILoaderFactory
{
public:
    virtual ~ILoaderFactory() = default;

    virtual IComponentLoaderPtr createLoaderInstance() = 0;
};

using ILoaderFactoryPtr  = std::shared_ptr<ILoaderFactory>;
using ILoaderFactoryUPtr = std::unique_ptr<ILoaderFactory>;

} // namespace HBE
