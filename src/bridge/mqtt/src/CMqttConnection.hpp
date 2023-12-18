/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#pragma once

#include "mqtt/IMqttConnection.hpp"

#include <atomic>
#include <mosquittopp.h>

#include "component/TDepends.hpp"
#include "component/TComponent.hpp"
#include "interface/TInterfaces.hpp"

#include "finalhaven/IFinalHaven.hpp"
#include "connman/IConnMan.hpp"
#include "connman/IConnectable.hpp"

#include "common/EventLoop.hpp"

IMPLEMENT_INTERFACE(MqttConn, Mqtt::IMqttConnection);

DEFINE_DEPENDENCY(FinalHaven, System::IFinalHaven);
DEFINE_DEPENDENCY(ConnMan, System::IConnMan);

namespace Mqtt {

struct CMqttConnectionAnnotation
{
    using Interfaces = HBE::TInterfaces<MqttConn>;
    using Depends    = HBE::TDepends<ConnMan, FinalHaven>;
    using ConfigType = boost::property_tree::ptree;
};

///
/// @brief
///
class CMqttConnection final
    : public IMqttConnection
    , public System::IConnectable
    , public mosqpp::mosquittopp
    , public HBE::TComponent<CMqttConnectionAnnotation>
    , public std::enable_shared_from_this<CMqttConnection>
{

    struct CMqttConfig
    {
        std::string mqtt_broker;
        std::string mqtt_broker_port;
        std::string mqtt_username;
        std::string mqtt_password;
    };

public:
    ///
    /// @brief constructor
    ///
    explicit CMqttConnection(
        const ConfigType& cfg,
        const HBE::ResolvedDependencies dependencies);

    ///
    /// @brief CMqttConnection default destructor
    ///
    ~CMqttConnection() noexcept final;

    ///
    /// @brief CMqttConnection remove copy constructor
    ///
    CMqttConnection(const CMqttConnection&) = delete;

    ///
    /// @brief CMqttConnection remove operator '='
    ///
    CMqttConnection& operator=(const CMqttConnection&) & = delete;

    ///------- IComponent implementation

    ///
    /// @brief register my interfaces
    ///
    void registerInterfaces() final {REGISTER_INTERFACE(MqttConn)}

    ///
    /// @brief init component, subscribe for events, connect to interfaces and so on
    ///
    void initComponent() final;

    ///
    /// @brief start component, do component main cycle
    ///
    void startComponent() final;

public:
    ///------- IConnectable implementation

    ///
    /// @brief Connecting to the remove server
    ///
    void connect() final;

    ///
    /// @brief Disconnect from the remote server
    ///
    void disconnect() final;

    ///
    /// @brief Indicates connection status
    ///
    System::IConnectable::EConnectionStatus getState() const final;

    ///
    /// @brief Indicates connection status
    ///
    System::IConnectable::ConnectionInfo getClientInfo() const final;


    ///------- IMqttConnection implementation

    ///
    /// @brief get event queue context
    /// @return context which contain event queue
    ///
    const std::shared_ptr<Common::TEventConsumer<TMqttEventVariant>> getEventContext() const final;

    ///
    /// @brief subscribe on mqtt topic
    /// @param topic mqtt topic name
    ///
    void subscribeTopic(const std::string& topic) final;

    ///
    /// @brief publish mqtt topic
    /// @param topic mqtt topic name
    /// @param topmsgic message string
    ///
    void setTopic(const std::string& topic, const std::string& msg) final;

private:
    void on_connect(int rc) final;
    void on_message(const struct mosquitto_message* message) final;
    void on_disconnect(int rc) final;
    void on_error() final;

private:
    Depends                m_dependencies;
    System::IFinalHavenPtr m_final_haven;
    System::IConnManPtr    m_conn_man;

    std::shared_ptr<Common::TEventLoop<TMqttEventVariant>> m_event_loop;
    std::atomic <System::IConnectable::EConnectionStatus> m_conn_status;

    CMqttConfig m_mqtt_cfg;
    std::vector<std::string> m_topics_list;
};
} // namespace MqttConn