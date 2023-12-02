#pragma once

#include "managers/ILoaderFactory.hpp"

namespace HBE {

class CLoaderFactory : public ILoaderFactory
{
public:
    ~CLoaderFactory() = default;

    IComponentLoaderPtr createLoaderInstance() override;
};

} // namespace HBE

