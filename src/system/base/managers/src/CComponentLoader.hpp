/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#pragma once

#include <string>
#include <functional>

#include "managers/IComponentLoader.hpp"

namespace HBE {

class CComponentLoader : public IComponentLoader
{
public:
    IComponentFactoryPtr loadComponent(std::string path) override;

private:
    using CreateComponentFactoryFnSignature = IComponentFactoryPtr();
    using CreateComponentFactoryFn = std::function<CreateComponentFactoryFnSignature>;

    struct Library
    {
        std::shared_ptr<CreateComponentFactoryFn> library_fn;
        IComponentFactoryPtr factory;
    };

private: // fields
    std::map<std::string, Library> m_libraries;
};

} // namespace HBE