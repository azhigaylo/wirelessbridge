/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

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

