/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#pragma once

#include <string>

namespace WBridge
{


            "input_mqtt_topic": "home\/external\/weather_station\/status\/weather_station",
            "output_mqtt_topic": "home\/external\/weather_station\/control\/weather_station",
            "status_mqtt_topic": "home\/status\/mqtt\/communication\/weather_station",
            "node_timeout_in_sec": "10",


class CDeviceConfigParser final
{

public:

    struct router_item_t
    {
        typedef std::pair <std::uint16_t, std::string> mapp_item_t;

        bool        topic_sub;
        uint32_t    number;
        std::string mqtt_topic;
        std::vector<mapp_item_t> mapping;
    };

    typedef std::pair <std::string, router_item_t> bridge_item_t;

    ///
    /// @brief constructor
    /// @param cfg_path configuration path
    ///
    explicit CDeviceConfigParser(std::string const cfg_path);

    ///
    /// @brief CMqttConnection default destructor
    ///
    ~CDeviceConfigParser() noexcept final;

    ///
    /// @brief CMqttConnection remove copy and operators
    ///
    CDeviceConfigParser(const CDeviceConfigParser&) = delete;
    CDeviceConfigParser& operator=(const CDeviceConfigParser&) = delete;


    const std::vector<bridge_item_t>& getGwtTable()const {return m_bridge_vector; }

    uint32_t getDeviceAmount()const {return m_device_amount;}

/*
    ///
    /// @brief get list of the components name
    /// @return list of the components name
    ///
    std::vector<std::string> getComponentList() const final;

    ///
    /// @brief get config for selected component
    /// @param cmp_name component name
    /// @return component library path
    ///
    std::string getComponentLib(std::string const cmp_name) final;

    ///
    /// @brief get config for selected component
    /// @param cmp_name component name
    /// @return component configuration
    ///
    boost::property_tree::ptree getComponentConfig(std::string const cmp_name) final;
*/

private:

    std::vector<bridge_item_t> m_bridge_vector;
    uint32_t m_device_amount;
};

} //namespace WBridge

