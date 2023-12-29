/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#include <atomic>
#include <thread>
#include <csignal>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include "loger/slog.h"

#include "component/IComponent.hpp"
#include "component/IComponentFactory.hpp"
#include "managers/ComponentManagerFactory.hpp"

#include "finalhaven/IFinalHaven.hpp"
#include "connman/IConnMan.hpp"

//#include "bridge/WirelessBridgeImpl.hpp"

int main(int argc, const char** argv)
{
    uint32_t debug_level;
    uint32_t debug_sink;
    boost::filesystem::path config_file;
    boost::filesystem::path bridge_table_file;

    boost::program_options::options_description desc("HomeBrain core <-> MQTT Wireless Bridge Component Options");
    desc.add_options()
        ("help,h", "produce help message")
        ("debug,d", boost::program_options::value<uint32_t>(&debug_level), "debug level 0-4(err/wr/info/dbg")
        ("sink,s",  boost::program_options::value<uint32_t>(&debug_sink), "debug sink 0-1(console/dlt")
        ("config,c",boost::program_options::value<boost::filesystem::path>(&config_file)->default_value("bridge_table.json"),
                    std::string("Specify bridge table path. By default: 'bridge_table.json'").c_str())
        ("table,t", boost::program_options::value<boost::filesystem::path>(&bridge_table_file)->default_value("bridge_config.json"),
                    std::string("Specify config path. By default: 'bridge_config.json'").c_str());

    boost::program_options::variables_map vm;
    try
    {
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
        boost::program_options::notify(vm);
    }
    catch (const boost::program_options::error& e)
    {
        std::cerr << "Error while parsing: '" << e.what() << "'" << std::endl;
        return EXIT_FAILURE;
    }

    if (vm.count("help"))
    {
        std::cout << desc << std::endl;
        return EXIT_SUCCESS;
    }

    if (vm.count("debug"))
    {
        setDbgLevel(debug_level);
    }

    if (vm.count("sink"))
    {
        setDbgSink(debug_sink);
    }

    if (TRACE_SINK_DLT == getDbgSink())
    {
        initDlt();
        printDebug("WirelessBridge/%s: Dlt inited...", __FUNCTION__);
    }

    printDebug("WirelessBridge/%s: debug level = %i", __FUNCTION__, debug_level);
    printDebug("WirelessBridge/%s: debug sink = %i", __FUNCTION__, debug_sink);
    printDebug("WirelessBridge/%s: started with cfg = %s", __FUNCTION__, config_file.string().c_str());

    printDebug("WirelessBridge/%s: Wireless Bridge Starting...", __FUNCTION__);

    try
    {
        using namespace HBE;
        using namespace System;

        IComponentManagerPtr m_component_manager = ComponentManagerFactory::createInstance();
        IComponentConfigParserPtr m_component_configuration = ComponentManagerFactory::createConfiguration(config_file.string());

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
                    printDebug("WirelessBridge/%s: interface IFinalHaven fond...", __FUNCTION__);

                    bool exit_request = false;
                    while(!exit_request)
                    {
                        std::future<HbeError> fut = fh->waitAction();

                        ErrorLevel  lvl;
                        std::string descr;
                        std::tie(lvl, descr) = fut.get();

                        printDebug("WirelessBridge/%s: got an action = %s", __FUNCTION__, descr.c_str());

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
        printDebug("WirelessBridge/%s: Error description: %s", __FUNCTION__, e.what());
        printDebug("WirelessBridge/%s: Wireless Bridge closed", __FUNCTION__);
        if (TRACE_SINK_DLT == getDbgSink())
        {
            deinitDlt();
            printDebug("WirelessBridge/%s: Dlt deinited...", __FUNCTION__);
        }
        return 1;
    }

    // after "exit" action
    printDebug("WirelessBridge/%s: Wireless Bridge Stopped...", __FUNCTION__);

    if (TRACE_SINK_DLT == getDbgSink())
    {
        deinitDlt();
        printDebug("WirelessBridge/%s: Dlt deinited...", __FUNCTION__);
    }

    return EXIT_SUCCESS;
}
