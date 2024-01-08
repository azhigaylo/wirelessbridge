/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include "launcher/CLauncher.hpp"

int main(int argc, const char** argv)
{
    uint32_t debug_level;
    uint32_t debug_sink;
    boost::filesystem::path config_file;
    boost::filesystem::path bridge_table_file;

    boost::program_options::options_description desc("MQTT Wireless Bridge Component Options");
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

    using namespace System;
    CLauncher app{CLauncher::AppDebugInfo{debug_level, debug_sink}, config_file.string()};

    return app.startApplication();;
}
