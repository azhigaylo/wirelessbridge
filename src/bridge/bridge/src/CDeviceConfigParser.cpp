/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#include "CDeviceConfigParser.hpp"

#include <memory>
#include <iostream>
#include <stdexcept>
#include <algorithm>

#include <boost/filesystem.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "common/slog.h"

namespace WBridge
{

namespace Tbl
{
    const std::string c_wb_dev_list                = "wireless_list";
    const std::string c_wb_dev_name                = "dev_name";
    const std::string c_wb_input_mqtt_topic        = "input_mqtt_topic";
    const std::string c_wb_output_mqtt_topic       = "output_mqtt_topic";
    const std::string c_wb_status_mqtt_topic       = "status_mqtt_topic";
    const std::string c_wb_node_timeout_in_sec     = "node_timeout_in_sec";
    const std::string c_wb_node_connection_attempt = "node_connection_attempt";
    const std::string c_wb_input_mapping           = "input_mapping";
    const std::string c_wb_output_mapping          = "output_mapping";

    const std::string c_wb_subconfig_number            = "number";
    const std::string c_wb_subconfig_name              = "name";
    const std::string c_wb_subconfig_mqtt_topic        = "mqtt_topic";
    const std::string c_wb_subconfig_mqtt_subscribe    = "mqtt_subscribe";
    const std::string c_wb_subconfig_value_mapping     = "value_mapping";
    const std::string c_wb_subconfig_mapping_value_int = "value";
    const std::string c_wb_subconfig_mapping_value_str = "mapp_to";
}

CDeviceConfigParser::CDeviceConfigParser(std::string const cfg_path)
{
    if (!boost::filesystem::exists(cfg_path))
    {
        throw std::runtime_error(std::string("device configuration unavailable: file is not exist: ") + cfg_path );
    }
    else
    {
        try
        {
            namespace pt = boost::property_tree;
            pt::ptree dev_ptree;
            pt::read_json(cfg_path, dev_ptree);

            printDebug("CDeviceConfigParser/%s: device cfg path = %s", __FUNCTION__, cfg_path.c_str());

            // create discret point rouring table
            for (pt::ptree::value_type &wb_item : dev_ptree.get_child(Tbl::c_wb_dev_list))
            {
                RouterDeviceItem router_item;

                router_item.dev_name = wb_item.second.get<std::string>(Tbl::c_wb_dev_name);
                router_item.input_mqtt_topic = wb_item.second.get<std::string>(Tbl::c_wb_input_mqtt_topic);
                router_item.output_mqtt_topic = wb_item.second.get<std::string>(Tbl::c_wb_output_mqtt_topic);
                router_item.status_mqtt_topic = wb_item.second.get<std::string>(Tbl::c_wb_status_mqtt_topic);
                router_item.node_timeout_in_sec = wb_item.second.get<uint32_t>(Tbl::c_wb_node_timeout_in_sec);
                router_item.node_connection_attempt = wb_item.second.get<uint32_t>(Tbl::c_wb_node_connection_attempt);

                printDebug("CGtwTableParser/%s: found device d_name = %s", __FUNCTION__, router_item.dev_name.c_str());

                // create discret point rouring table
                for (pt::ptree::value_type &wb_item_inputs : wb_item.second.get_child(Tbl::c_wb_input_mapping))
                {
                    RouterDeviceChannel router_item_input;

                    router_item_input.number = wb_item_inputs.second.get<uint32_t>(Tbl::c_wb_subconfig_number);
                    router_item_input.name = wb_item_inputs.second.get<std::string>(Tbl::c_wb_subconfig_name);
                    router_item_input.mqtt_topic = wb_item_inputs.second.get<std::string>(Tbl::c_wb_subconfig_mqtt_topic);
                    router_item_input.mqtt_subscribe = wb_item_inputs.second.get<bool>(Tbl::c_wb_subconfig_mqtt_subscribe);

                    printDebug("CGtwTableParser/%s: input number = %i", __FUNCTION__, router_item_input.number);

                    // create inputs value mapping
                    for (pt::ptree::value_type &wb_item_inputs_mapping : wb_item_inputs.second.get_child(Tbl::c_wb_subconfig_value_mapping))
                    {
                        uint16_t value_int = wb_item_inputs_mapping.second.get<uint16_t>(Tbl::c_wb_subconfig_mapping_value_int);
                        std::string value_str = wb_item_inputs_mapping.second.get<std::string>(Tbl::c_wb_subconfig_mapping_value_str);

                        router_item_input.value_mapping.push_back(std::make_pair(value_int, value_str));

                        printDebug("CGtwTableParser/%s: int = %i <-> str = %s", __FUNCTION__, value_int, value_str.c_str());
                    }
                    // add inputs
                    router_item.input_mapping.push_back(router_item_input);
                }

                // create discret point rouring table
                for (pt::ptree::value_type &wb_item_outputs : wb_item.second.get_child(Tbl::c_wb_output_mapping))
                {
                    RouterDeviceChannel router_item_output;

                    router_item_output.number = wb_item_outputs.second.get<uint32_t>(Tbl::c_wb_subconfig_number);
                    router_item_output.name = wb_item_outputs.second.get<std::string>(Tbl::c_wb_subconfig_name);
                    router_item_output.mqtt_topic = wb_item_outputs.second.get<std::string>(Tbl::c_wb_subconfig_mqtt_topic);
                    router_item_output.mqtt_subscribe = wb_item_outputs.second.get<bool>(Tbl::c_wb_subconfig_mqtt_subscribe);

                    printDebug("CGtwTableParser/%s: output number = %i", __FUNCTION__, router_item_output.number);

                    // create inputs value mapping
                    for (pt::ptree::value_type &wb_item_outputs_mapping : wb_item_outputs.second.get_child(Tbl::c_wb_subconfig_value_mapping))
                    {
                        uint16_t value_int = wb_item_outputs_mapping.second.get<uint16_t>(Tbl::c_wb_subconfig_mapping_value_int);
                        std::string value_str = wb_item_outputs_mapping.second.get<std::string>(Tbl::c_wb_subconfig_mapping_value_str);

                        router_item_output.value_mapping.push_back(std::make_pair(value_int, value_str));

                        printDebug("CGtwTableParser/%s: int = %i <-> str = %s", __FUNCTION__, value_int, value_str.c_str());
                    }
                    // add inputs
                    router_item.output_mapping.push_back(router_item_output);
                }
                m_wb_vector.push_back(router_item);
            }
        }
        catch (const std::exception& e)
        {
            printError("CDeviceConfigParser/%s: Error during parsing config: %s", __FUNCTION__, cfg_path.c_str());
            printError("CDeviceConfigParser/%s: Error description: %s", __FUNCTION__, e.what());

            throw std::runtime_error("configuration error: config parsing error" );
        }
    }
}

boost::optional<CDeviceConfigParser::RouterDeviceItem> CDeviceConfigParser::getGwtItem(std::string const name) const
{
    boost::optional<RouterDeviceItem> item{};

    std::vector<RouterDeviceItem>::const_iterator m_wb_vector_itr{std::find_if(
        m_wb_vector.begin(), m_wb_vector.end(), [name](RouterDeviceItem const data) {
            return name == data.dev_name;
        })};

    if (m_wb_vector_itr != m_wb_vector.end())
    {

        item = boost::make_optional(*m_wb_vector_itr);
    }

    return item;
}

CDeviceConfigParser::~CDeviceConfigParser() noexcept
{
    printDebug("CDeviceConfigParser/%s: removed...", __FUNCTION__);
}

} //namespase WBridge
