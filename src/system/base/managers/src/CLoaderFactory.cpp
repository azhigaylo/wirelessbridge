#include "CLoaderFactory.hpp"

#include "CComponentLoader.hpp"

namespace HBE {

IComponentLoaderPtr CLoaderFactory::createLoaderInstance()
{
    return std::make_shared<CComponentLoader>();
}

} // namespace HBE
