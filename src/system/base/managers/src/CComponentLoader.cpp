
#include "CComponentLoader.hpp"

#include <boost/dll/import.hpp>

namespace HBE {

IComponentFactoryPtr CComponentLoader::loadComponent(std::string path)
{
    Library& lib = m_libraries[path];
    if (!lib.library_fn)
    {
        CreateComponentFactoryFn func = boost::dll::import_alias<CreateComponentFactoryFnSignature>(
            path, "createComponentFactory", boost::dll::load_mode::append_decorations);

        lib.library_fn = std::make_shared<CreateComponentFactoryFn>(func);
        lib.factory = (*lib.library_fn)();
    }

    return lib.factory;
}

} // namespace HBE