/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <boost/optional.hpp>

namespace WBridge
{

class CDeviceConfig final
{
public:
    struct RouterDeviceChannel
    {
        typedef std::pair <std::uint16_t, std::string> mapp_item_t;

        uint32_t    number;
        std::string name;
        std::string mqtt_topic;
        bool        mqtt_subscribe;
        std::vector<mapp_item_t> value_mapping;
    };

    struct RouterDeviceItem
    {
        std::string dev_name;
        std::string input_mqtt_topic;
        std::string output_mqtt_topic;
        std::string status_mqtt_topic;
        uint32_t    node_timeout_in_sec;
        uint32_t    node_connection_attempt;

        std::vector<RouterDeviceChannel> input_mapping;
        std::vector<RouterDeviceChannel> output_mapping;
    };

    ///
    /// @brief constructor
    /// @param cfg_path configuration path
    ///
    explicit CDeviceConfig(std::string const cfg_path);

    ///
    /// @brief CMqttConnection default destructor
    ///
    ~CDeviceConfig() noexcept;

    ///
    /// @brief constructor
    /// @return device list
    ///
    std::vector<RouterDeviceItem> const& getGwtTable()const {return m_wb_vector; }

    ///
    /// @brief constructor
    /// @param name device name
    /// @return device annotation if available
    ///
    boost::optional<RouterDeviceItem> getGwtItem(std::string const name) const;

private:
    std::vector<RouterDeviceItem> m_wb_vector;
};

using CDeviceConfigPtr = std::shared_ptr<CDeviceConfig>;

} //namespace WBridge

