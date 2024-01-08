/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#include "managers/ComponentManagerFactory.hpp"

#include "CLoaderFactory.hpp"
#include "CComponentManager.hpp"
#include "helpers/CComponentConfigParser.hpp"

namespace HBE {

IComponentConfigParserPtr ComponentManagerFactory::createConfiguration(std::string const cfg_path)
{
    return std::make_shared<CComponentConfigParser>(cfg_path);
}

IComponentManagerPtr ComponentManagerFactory::createInstance()
{
    return std::make_shared<CComponentManager>(std::make_shared<CLoaderFactory>());
}

} // namespace HBE
