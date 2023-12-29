/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#include "helpers/CComponentConfigParser.hpp"

#include <memory>
#include <iostream>
#include <stdexcept>

#include <boost/filesystem.hpp>

#include "loger/slog.h"

namespace HBE
{

namespace Tbl
{
    const std::string c_cmp_list = "components";
    const std::string c_cmp_name = "name";
    const std::string c_cmp_lib_path = "lib_path";
    const std::string c_cmp_config   = "config";
}

CComponentConfigParser::CComponentConfigParser(std::string const cfg_path)
{

    if (!boost::filesystem::exists(cfg_path))
    {
        throw std::runtime_error(std::string("configuration unavailable: file is not exist: ") + cfg_path );
    }
    else
    {
        try
        {
            namespace pt = boost::property_tree;
            pt::ptree cmp_ptree;
            pt::read_json(cfg_path, cmp_ptree);

            // create discret point rouring table
            for (const auto& cmp_item : cmp_ptree.get_child(Tbl::c_cmp_list))
            {
                std::string cmp_name = cmp_item.second.get<std::string>(Tbl::c_cmp_name);
                std::string cmp_lib_path = cmp_item.second.get<std::string>(Tbl::c_cmp_lib_path);
                pt::ptree cfg_ptree = cmp_item.second.get_child(Tbl::c_cmp_config);

                m_components_name.push_back(cmp_name);
                m_components_map.insert({cmp_name, std::make_pair(cmp_lib_path, cfg_ptree)});

                printDebug("CComponentConfigParser/%s: found component = %s(%s)", __FUNCTION__, cmp_name.c_str(), cmp_lib_path.c_str());
            }
        }
        catch (const std::exception& e)
        {
            printError("CComponentConfigParser/%s: Error during parsing config: %s", __FUNCTION__, cfg_path.c_str());
            printError("CComponentConfigParser/%s: Error description: %s", __FUNCTION__, e.what());

            throw std::runtime_error("configuration error: config parsing error" );
        }
    }
}

CComponentConfigParser::~CComponentConfigParser() noexcept
{
    printDebug("CComponentConfigParser/%s: removed...", __FUNCTION__);
}

std::vector<std::string> CComponentConfigParser::getComponentList() const
{
    return m_components_name;
}

std::string CComponentConfigParser::getComponentLib(std::string const cmp_name)
{
    std::string lib_path;

    auto cmp_item = m_components_map.find(cmp_name);
    if (cmp_item != m_components_map.end())
    {
        lib_path = cmp_item->second.first;
    }
    else
    {
        throw std::runtime_error("configuration error: unknown component name" );
    }

    return lib_path;
}


boost::property_tree::ptree CComponentConfigParser::getComponentConfig(std::string const cmp_name)
{
    boost::property_tree::ptree cmp_config;

    auto cmp_item = m_components_map.find(cmp_name);
    if (cmp_item != m_components_map.end())
    {
        cmp_config = cmp_item->second.second;
    }
    else
    {
        throw std::runtime_error("configuration error: unknown component name" );
    }

    return cmp_config;
}

} //namespase HBE
