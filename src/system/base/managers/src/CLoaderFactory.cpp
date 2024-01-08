/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#include "CLoaderFactory.hpp"

#include "CComponentLoader.hpp"

namespace HBE {

IComponentLoaderPtr CLoaderFactory::createLoaderInstance()
{
    return std::make_shared<CComponentLoader>();
}

} // namespace HBE
