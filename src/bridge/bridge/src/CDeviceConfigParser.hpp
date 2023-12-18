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

namespace WBridge
{

class CDeviceConfigParser final
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

        std::vector<RouterDeviceChannel> input_mapping;
        std::vector<RouterDeviceChannel> output_mapping;
    };

    ///
    /// @brief constructor
    /// @param cfg_path configuration path
    ///
    explicit CDeviceConfigParser(std::string const cfg_path);

    ///
    /// @brief CMqttConnection default destructor
    ///
    ~CDeviceConfigParser() noexcept;

    const std::vector<RouterDeviceItem>& getGwtTable()const {return m_wb_vector; }

    uint32_t getDeviceAmount()const {return m_device_amount;}

private:
    uint32_t m_device_amount;
    std::vector<RouterDeviceItem> m_wb_vector;
};

using CDeviceConfigParserPtr = std::shared_ptr<CDeviceConfigParser>;

} //namespace WBridge

