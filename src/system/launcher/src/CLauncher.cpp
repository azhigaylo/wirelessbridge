/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#include "launcher/CLauncher.hpp"

#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "loger/slog.h"

#include "component/IComponent.hpp"
#include "component/IComponentFactory.hpp"
#include "managers/ComponentManagerFactory.hpp"

#include "finalhaven/IFinalHaven.hpp"

namespace System
{

CLauncher::CLauncher(AppDebugInfo const& debug_data, std::string const& cfg)
    : m_debug_level{debug_data.debug_level}
    , m_debug_sink(debug_data.debug_sink)
    , m_config_file{cfg}
{
    // set debug level
    setDbgLevel(m_debug_level);

    // set debug sink
    setDbgSink(m_debug_sink);

    if (TRACE_SINK_DLT == getDbgSink())
    {
        initDlt();
        printDebug("CLauncher/%s: Dlt inited...", __FUNCTION__);
    }

    printDebug("CLauncher/%s: debug level = %i", __FUNCTION__, m_debug_level);
    printDebug("CLauncher/%s: debug sink = %i", __FUNCTION__, m_debug_sink);
    printDebug("CLauncher/%s: started with cfg = %s", __FUNCTION__, m_config_file.c_str());
    printDebug("CLauncher/%s: created...", __FUNCTION__);
}

CLauncher::~CLauncher()
{
    if (TRACE_SINK_DLT == getDbgSink())
    {
        deinitDlt();
        printDebug("CLauncher/%s: Dlt deinited...", __FUNCTION__);
    }

    printDebug("CLauncher/%s: deleted.", __FUNCTION__);
}

int CLauncher::startApplication()
{
    printDebug("CLauncher/%s: application starting...", __FUNCTION__);

    try
    {
        using namespace HBE;
        using namespace System;

        IComponentManagerPtr m_component_manager = ComponentManagerFactory::createInstance();
        IComponentConfigParserPtr m_component_configuration = ComponentManagerFactory::createConfiguration(m_config_file);

        auto cmp_name_list = m_component_configuration->getComponentList();
        for (auto const cmp_name : cmp_name_list)
        {
            ComponentConfig cfg{
                cmp_name,
                m_component_configuration->getComponentLib(cmp_name),
                m_component_configuration->getComponentConfig(cmp_name)
            };
            m_component_manager->registerComponent(cfg);
        }

        {
            m_component_manager->initAndStartComponents();
        }

        {
            IComponentPtr m_final_haven = m_component_manager->getComponent("final_haven");

            ProvidedInterfaces const& provided_interfaces = m_final_haven->getProvidedInterfaces();

            auto interface_pair = provided_interfaces.find("FinalHaven");
            if (interface_pair != provided_interfaces.end())
            {
                System::IFinalHavenPtr fh = std::dynamic_pointer_cast<System::IFinalHaven>(interface_pair->second.lock());

                if (fh)
                {
                    printDebug("CLauncher/%s: interface IFinalHaven fond...", __FUNCTION__);

                    bool exit_request = false;
                    while(!exit_request)
                    {
                        std::future<HbeError> fut = fh->waitAction();

                        ErrorLevel  lvl;
                        std::string descr;
                        std::tie(lvl, descr) = fut.get();

                        printDebug("CLauncher/%s: got an action = %s", __FUNCTION__, descr.c_str());

                        switch (lvl)
                        {
                            case ErrorLevel::system_level: break;
                            case ErrorLevel::component_level: break;
                            case ErrorLevel::user_level:
                            case ErrorLevel::critical_level:
                            default:
                            {
                                exit_request = true;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    catch (const std::exception& e)
    {
        printDebug("CLauncher/%s: Error description: %s", __FUNCTION__, e.what());
        return EXIT_FAILURE;
    }

    // after "exit" action
    printDebug("CLauncher/%s: application stopped...", __FUNCTION__);

    return EXIT_SUCCESS;
}

} // namespace System
