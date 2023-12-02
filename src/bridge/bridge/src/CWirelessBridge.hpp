/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#pragma once

#include "bridge/IWirelessBridge.hpp"

#include "component/TDepends.hpp"
#include "component/TComponent.hpp"
#include "interface/TInterfaces.hpp"

#include "finalhaven/IFinalHaven.hpp"
#include "executor/IExecutor.hpp"
#include "mqtt/IMqttConnection.hpp"

#include "common/EventListener.hpp"

IMPLEMENT_INTERFACE(WirelessBridge, WBridge::IWirelessBridge);

DEFINE_DEPENDENCY(FinalHaven, System::IFinalHaven);
DEFINE_DEPENDENCY(Executor, System::IExecutor);
DEFINE_DEPENDENCY(MqttConn, Mqtt::IMqttConnection);

namespace WBridge {

struct CWirelessBridgeAnnotation
{
    using Interfaces = HBE::TInterfaces<WirelessBridge>;
    using Depends    = HBE::TDepends<MqttConn, Executor, FinalHaven>;
    using ConfigType = boost::property_tree::ptree;
};

///
/// @brief
///
class CWirelessBridge final
    : public IWirelessBridge
    , public HBE::TComponent<CWirelessBridgeAnnotation>
    , public std::enable_shared_from_this<CWirelessBridge>
{

public:
    ///
    /// @brief constructor
    ///
    explicit CWirelessBridge(
        const ConfigType& cfg,
        const HBE::ResolvedDependencies dependencies);

    ///
    /// @brief CWirelessBridge default destructor
    ///
    ~CWirelessBridge() noexcept final;

    ///
    /// @brief CWirelessBridge remove copy constructor
    ///
    CWirelessBridge(const CWirelessBridge&) = delete;

    ///
    /// @brief CWirelessBridge remove operator '='
    ///
    CWirelessBridge& operator=(const CWirelessBridge&) & = delete;

    ///------- IComponent implementation

    ///
    /// @brief register my interfaces
    ///
    void registerInterfaces() override {REGISTER_INTERFACE(WirelessBridge)}

    ///
    /// @brief init component, subscribe for events, connect to interfaces and so on
    ///
    void initComponent() override;

    ///
    /// @brief start component, do component main cycle
    ///
    void startComponent() override;

private:

    ///
    /// @brief callback from mqtt interface
    ///
    void mqttEventProcessFunc(const std::unique_ptr<Mqtt::TMqttEventVariant>& ev);

private:
    Depends                  m_dependencies;
    System::IFinalHavenPtr   m_final_haven;
    System::IExecutorPtr     m_executor;
    Mqtt::IMqttConnectionPtr m_mqtt_conn;

     std::string             m_dev_cfg_path;

    Common::TEventListener<Mqtt::TMqttEventVariant> m_mqtt_listener;
};
} // namespace WBridge