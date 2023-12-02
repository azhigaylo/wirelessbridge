/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#include "CWirelessBridge.hpp"

#include <iostream>

#include "common/slog.h"

namespace WBridge {

CWirelessBridge::CWirelessBridge(const ConfigType& cfg, const HBE::ResolvedDependencies dependencies)
    : m_dependencies{dependencies}
    , m_final_haven{m_dependencies.getInterface<FinalHaven>().lock()}
    , m_executor{m_dependencies.getInterface<Executor>().lock()}
    , m_mqtt_conn{m_dependencies.getInterface<MqttConn>().lock()}
    , m_mqtt_listener(
        boost::bind(&CWirelessBridge::mqttEventProcessFunc, this, _1),
        m_mqtt_conn->getEventLoop())
{
    // get device config path
    m_dev_cfg_path = cfg.get<std::string>("config.bridge_cfg_path");

    printDebug("CWirelessBridge/%s: device cfg path = %s", __FUNCTION__, m_dev_cfg_path.c_str());





}

CWirelessBridge::~CWirelessBridge() noexcept
{
    printDebug("CWirelessBridge/%s: deleted ...", __FUNCTION__);
}

///------- IComponent implementation-------------

void CWirelessBridge::initComponent()
{

}

void CWirelessBridge::startComponent()
{
    m_mqtt_conn->subscribeTopic("home/lights/sitting_room");
    m_mqtt_conn->subscribeTopic("home/lights/living_room");
    m_mqtt_conn->subscribeTopic("home/lights/bed_room");
}

///------- implementation-------------

void CWirelessBridge::mqttEventProcessFunc(const std::unique_ptr<Mqtt::TMqttEventVariant>& ev)
{
    printDebug("CWirelessBridge/%s: got an event...", __FUNCTION__);

    Mqtt::TMqttEventVariant mqtt_ev{*ev};
    try
    {
        if (mqtt_ev.type() == typeid(Mqtt::CMqttOnlineEvent))
        {
            bool val = boost::get<Mqtt::CMqttOnlineEvent>(mqtt_ev).getCommStatus();

            printDebug("CWirelessBridge/%s: mqtt online status = %i", __FUNCTION__, val);
        }else
        if (mqtt_ev.type() == typeid(Mqtt::CMqttTopicUpdateEvent))
        {
            std::string topic = boost::get<Mqtt::CMqttTopicUpdateEvent>(mqtt_ev).getTopic();
            std::string message = boost::get<Mqtt::CMqttTopicUpdateEvent>(mqtt_ev).getMessage();

            printDebug("CWirelessBridge/%s: topic: %s | msg: %s", __FUNCTION__, topic.c_str(), message.c_str());
        }
    }
    catch (const std::exception& e)
    {
        std::stringstream ss_msg{};
        ss_msg << __func__ << ": " << "Error: " << e.what() << std::endl;

        // store exception and push it to error manager through iterate method
        m_final_haven->report(System::ErrorLevel::component_level, ss_msg.str());
    }
}

} // namespace WBridge

DEFINE_COMPONENT(WBridge::CWirelessBridge);