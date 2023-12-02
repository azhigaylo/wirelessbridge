/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#include "CMqttConnection.hpp"

#include <iostream>

#include "common/slog.h"

namespace Mqtt {

CMqttConnection::CMqttConnection(const ConfigType& cfg, const HBE::ResolvedDependencies dependencies)
    : m_dependencies{dependencies}
    , m_final_haven{m_dependencies.getInterface<FinalHaven>().lock()}
    , m_conn_man{m_dependencies.getInterface<ConnMan>().lock()}
    , m_event_loop{std::make_shared<Common::TEventLoop<TMqttEventVariant>>()}
    , m_conn_status{System::IConnectable::EConnectionStatus::DISCONNECTED}
{
    // store mqtt config
    m_mqtt_cfg.mqtt_broker      = cfg.get<std::string>("config.mqtt_broker");
    m_mqtt_cfg.mqtt_broker_port = cfg.get<std::string>("config.mqtt_broker_port");
    m_mqtt_cfg.mqtt_username    = cfg.get<std::string>("config.mqtt_username");
    m_mqtt_cfg.mqtt_password    = cfg.get<std::string>("config.mqtt_password");

    printDebug("CMqttConnection/%s: mqtt cfg = %s(%s)",__FUNCTION__,
        m_mqtt_cfg.mqtt_broker.c_str(),
        m_mqtt_cfg.mqtt_broker_port.c_str());
}

CMqttConnection::~CMqttConnection()
{
    // stop mosquitto internal loop
    mosquittopp::loop_stop(true);
    mosqpp::lib_cleanup();

    printDebug("CMqttConnection/%s: deleted ...", __FUNCTION__);
}

///------- IComponent implementation-------------

void CMqttConnection::initComponent()
{
    // subscribe connection manager
    m_conn_man->subscribe(std::string{"MqttConnection"}, shared_from_this());

    // start mosquitto internal loop
    mosqpp::lib_init();
    mosquittopp::loop_start();

    // set credentials
    mosquittopp::username_pw_set(m_mqtt_cfg.mqtt_username.c_str(), m_mqtt_cfg.mqtt_password.c_str());
}

void CMqttConnection::startComponent()
{

}

///------- IConnectable implementation-----------

void CMqttConnection::connect()
{
    printDebug("CMqttConnection/%s: call ...", __FUNCTION__);

    // connect to mqtt
    mosquittopp::connect_async(m_mqtt_cfg.mqtt_broker.c_str(), std::stoi(m_mqtt_cfg.mqtt_broker_port), 120);
}

void CMqttConnection::disconnect()
{
    // disconnect to mqtt
    mosquittopp::disconnect();
}

System::IConnectable::EConnectionStatus CMqttConnection::getState() const
{
    return m_conn_status;
}

System::IConnectable::ConnectionInfo CMqttConnection::getClientInfo() const
{
    return ConnectionInfo{};
}

///------- IMqttConnection implementation--------

const std::shared_ptr<Common::TEventConsumer<TMqttEventVariant>> CMqttConnection::getEventLoop() const
{
    return m_event_loop;
}

void CMqttConnection::subscribeTopic(const std::string& topic)
{
    if (System::IConnectable::EConnectionStatus::CONNECTED == m_conn_status)
    {
        printDebug("CMqttConnection/%s: subscribe on topic: %s", __FUNCTION__, topic.c_str());
        mosquittopp::subscribe(NULL, topic.c_str(), 1);
    }
    else
    {
        printDebug("CMqttConnection/%s: subscription postponed on topic: %s", __FUNCTION__, topic.c_str());
    }

    // store for reconnection case
    m_topics_list.push_back(topic);
}

void CMqttConnection::setTopic(const std::string& topic, const std::string& msg)
{
    if (System::IConnectable::EConnectionStatus::CONNECTED == m_conn_status)
    {
        printDebug("CMqttConnection/%s: send msg: '%s' to topic: %s ", __FUNCTION__, msg.c_str(), topic.c_str());

        publish(NULL, topic.c_str(), static_cast<int>(msg.size()), static_cast<const void*>(msg.c_str()), 1, true);
    }
    else
    {
        printDebug("CMqttConnection/%s: set topic: %s skipped", __FUNCTION__, topic.c_str());
    }
}

void CMqttConnection::on_connect(int rc)
{
    if (0 == rc)
    {
        printDebug("CMqttConnection/%s: connected to mqtt broker", __FUNCTION__);

        m_conn_status = System::IConnectable::EConnectionStatus::CONNECTED;

        // resubscribe all topics
        for (auto topic: m_topics_list)
        {
            printDebug("CMqttConnection/%s: subscribe on topic: %s", __FUNCTION__, topic.c_str());
            mosquittopp::subscribe(NULL, topic.c_str(), 1);
        }

        // send online event
        m_event_loop->sendEvent(
            std::make_unique<Mqtt::TMqttEventVariant>(Mqtt::CMqttOnlineEvent(true)));
    }
    else
    {
        printWarning("CMqttConnection/%s: connection error to mqtt broker!!!", __FUNCTION__);

        m_conn_status = System::IConnectable::EConnectionStatus::DISCONNECTED;
    }
}

void CMqttConnection::on_message(const struct mosquitto_message* message)
{
    m_event_loop->sendEvent(
        std::make_unique<Mqtt::TMqttEventVariant>(Mqtt::CMqttTopicUpdateEvent(
            std::string((char*)message->topic),
            std::string((char*)message->payload))));
}

void CMqttConnection::on_disconnect(int /*rc*/)
{
    printWarning("CMqttConnection/%s: lost communication with mqtt brocker", __FUNCTION__);

    m_conn_status = System::IConnectable::EConnectionStatus::DISCONNECTED;

    // send offline event
    m_event_loop->sendEvent(
        std::make_unique<Mqtt::TMqttEventVariant>(Mqtt::CMqttOnlineEvent(false)));
}

void CMqttConnection::on_error()
{
    printWarning("CMqttConnection/%s: mqtt internal error!!!", __FUNCTION__);

    m_final_haven->report(System::ErrorLevel::component_level, "mqtt internal error!!!");
}

} // namespace Mqtt

DEFINE_COMPONENT(Mqtt::CMqttConnection);