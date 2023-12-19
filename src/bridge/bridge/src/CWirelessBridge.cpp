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
    , m_dev_cfg{std::make_shared<CDeviceConfigParser>(cfg.get<std::string>("config.bridge_cfg_path"))}
    , m_mqtt_listener{
        m_mqtt_conn,
        boost::bind(&CWirelessBridge::mqttEventProcessFunc, this, _1)
    }
{
    printDebug("CWirelessBridge/%s: created...", __FUNCTION__);
}

CWirelessBridge::~CWirelessBridge() noexcept
{
    printDebug("CWirelessBridge/%s: deleted.", __FUNCTION__);
}

///------- IComponent implementation-------------

void CWirelessBridge::initComponent()
{

}

void CWirelessBridge::startComponent()
{
    // create device list
    createDeviceList();
    // subscribe all topics
    mqttSubscribe();
    // start device processing
    m_executor->execute_cyclic(std::bind(&CWirelessBridge::processDeviceList, this), std::chrono::milliseconds{1000});
}

///------- implementation-------------

void CWirelessBridge::createDeviceList()
{
    const std::vector<CDeviceConfigParser::RouterDeviceItem>& gtw_table = m_dev_cfg->getGwtTable();

    // subscribe all topics
    for (auto r_itm: gtw_table)
    {
        printDebug("CMqttConnection/%s: process : %s", __FUNCTION__, r_itm.dev_name.c_str());
        // subscribe main topic
        m_device_items.emplace(std::make_pair(r_itm.dev_name, DevServiceBlk{0, HBE::getSystemTimeMsec()}));
    }
}

void CWirelessBridge::mqttSubscribe() const
{
    const std::vector<CDeviceConfigParser::RouterDeviceItem>& gtw_table = m_dev_cfg->getGwtTable();

    // subscribe all topics
    for (auto r_itm: gtw_table)
    {
        printDebug("CMqttConnection/%s: process : %s", __FUNCTION__, r_itm.dev_name.c_str());
        // subscribe main topic
        m_mqtt_conn->subscribeTopic(r_itm.input_mqtt_topic);
        // subscribe inputs
        for (auto r_itm_inp: r_itm.input_mapping)
        {
            printDebug("CMqttConnection/%s: process inputs : %s", __FUNCTION__, r_itm_inp.name.c_str());
            if (true == r_itm_inp.mqtt_subscribe)
            {
                m_mqtt_conn->subscribeTopic(r_itm_inp.mqtt_topic);
            }
        }
        // subscribe outputs
        for (auto r_itm_out: r_itm.output_mapping)
        {
            printDebug("CMqttConnection/%s: process outputs : %s", __FUNCTION__, r_itm_out.name.c_str());
            if (true == r_itm_out.mqtt_subscribe)
            {
                printDebug("CMqttConnection/%s: subscribe outputs : %s", __FUNCTION__, r_itm_out.mqtt_topic.c_str());
                m_mqtt_conn->subscribeTopic(r_itm_out.mqtt_topic);
            }
        }
    }
}

void CWirelessBridge::mqttEventProcessFunc(const std::unique_ptr<Mqtt::TMqttEventVariant>& ev)
{
    try
    {
        printDebug("CWirelessBridge/%s: got an event...", __FUNCTION__);

        Mqtt::TMqttEventVariant mqtt_ev{*ev};

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

void CWirelessBridge::processDeviceList()
{
    try
    {
        HBE::SysTimeMsec itr_time = HBE::getSystemTimeMsec();

        printDebug("CWirelessBridge/%s: iteration time = %d", __FUNCTION__, itr_time.count());

        for(auto item = m_device_items.begin(); item != m_device_items.end(); ++item)
        {
            boost::optional<CDeviceConfigParser::RouterDeviceItem> gtw_device{m_dev_cfg->getGwtItem(item->first)};

            if (gtw_device)
            {
                /// check, whether the time to wait connection has finished
                if (itr_time > item->second.time_to_attempt)
                {
                    /// check attempt counter
                    if (item->second.spent_attempt < gtw_device.get().node_connection_attempt)
                    {
                        printDebug("CWirelessBridge/%s: set red flag for '%s', attempt %i", __FUNCTION__, item->first.c_str(), item->second.spent_attempt);

                        item->second.spent_attempt++;
                        item->second.time_to_attempt = itr_time + std::chrono::seconds{gtw_device.get().node_timeout_in_sec};
                    }
                    else
                    {
                        if (item->second.spent_attempt++ == gtw_device.get().node_connection_attempt)
                        {
                            printWarning("CWirelessBridge/%s: device %s, all attempt exhausted !!!", __FUNCTION__, item->first.c_str());
                            // update status topic
                            m_mqtt_conn->setTopic(gtw_device.get().status_mqtt_topic, "error");
                        }
                    }
                }
            }
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