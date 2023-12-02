/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#include "helpers/CDeviceConfigParser.hpp"

#include <memory>
#include <iostream>
#include <stdexcept>

#include <boost/filesystem.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "common/slog.h"

namespace HBE
{

namespace Tbl
{
    const std::string c_wb_input_mqtt_topic     = "input_mqtt_topic";
    const std::string c_wb_output_mqtt_topic    = "output_mqtt_topic";
    const std::string c_wb_status_mqtt_topic    = "status_mqtt_topic";
    const std::string c_wb_node_timeout_in_sec  = "node_timeout_in_sec";
    const std::string c_wb_input_mapping        = "input_mapping";
    const std::string c_wb_output_mapping       = "output_mapping";
    const std::string c_wb_subconfig_number     = "number";
    const std::string c_wb_subconfig_name       = "name";
    const std::string c_wb_subconfig_mqtt_topic = "mqtt_topic";
}

CComponentConfigParser::CComponentConfigParser(std::string const cfg_path)
{
    if (!boost::filesystem::exists(cfg_path))
    {
        throw std::runtime_error(std::string("device configuration unavailable: file is not exist: ") + cfg_path );
    }
    else
    {
        try
        {
            // create discret point rouring table
            for (pt::ptree::value_type &d_gtw_item : table_ptree.get_child(Tbl::c_gtw_table_d_routing))
            {
                router_item_t router_item;
                router_item.number = static_cast<uint32_t>(d_gtw_item.second.get<unsigned>(Tbl::c_gtw_item_d_number));
                router_item.mqtt_topic = d_gtw_item.second.get<std::string>(Tbl::c_gtw_item_topic);
                router_item.topic_sub = d_gtw_item.second.get<bool>(Tbl::c_gtw_item_subscription);

                printDebug("CGtwTableParser/%s: d_num = %i / s = %i / topic = %s", __FUNCTION__, router_item.number,
                                                                                               router_item.topic_sub,
                                                                                               router_item.mqtt_topic.c_str());
                // create discret point rouring table
                for (pt::ptree::value_type &d_item_mapping : d_gtw_item.second.get_child(Tbl::c_gtw_item_mapping))
                {
                   uint16_t value_int = static_cast<uint32_t>(d_item_mapping.second.get<unsigned>(Tbl::c_gtw_item_mapping_value_int));
                   std::string value_str = d_item_mapping.second.get<std::string>(Tbl::c_gtw_item_mapping_value_str);

                   router_item.mapping.push_back(std::make_pair(value_int, value_str));

                   printDebug("CGtwTableParser/%s: int = %i <-> str = %s", __FUNCTION__, value_int, value_str.c_str());
                }
                digitalCheckForMax(router_item.number);
                m_gwt_vector.push_back(std::make_pair(Tbl::c_gtw_table_d_routing, router_item));
            }


















/*
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
*/
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
