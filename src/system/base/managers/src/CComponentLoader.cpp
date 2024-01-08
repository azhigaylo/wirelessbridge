/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#include "CComponentLoader.hpp"

#include <boost/dll/import.hpp>

#include "loger/slog.h"
#include "managers/CComponentRegisterException.hpp"

namespace HBE {

IComponentFactoryPtr CComponentLoader::loadComponent(std::string path)
{
    printDebug("CComponentLoader/%s: load component = %s", __FUNCTION__, path.c_str());

    Library& lib = m_libraries[path];
    if (!lib.library_fn)
    {
        CreateComponentFactoryFn func = boost::dll::import_alias<CreateComponentFactoryFnSignature>(
            path, "createComponentFactory", boost::dll::load_mode::append_decorations | boost::dll::load_mode::load_with_altered_search_path);

        lib.library_fn = std::make_shared<CreateComponentFactoryFn>(func);
        lib.factory = (*lib.library_fn)();
    }
    else
    {
        printError("CComponentManager/%s:  component reload = %s", __FUNCTION__, path.c_str());
        throw CComponentRegisterException("component reload");
    }

    return lib.factory;
}

} // namespace HBE