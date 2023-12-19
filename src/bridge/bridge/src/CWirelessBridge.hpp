/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#pragma once

#include <unordered_map>

#include "bridge/IWirelessBridge.hpp"

#include "component/TDepends.hpp"
#include "component/TComponent.hpp"
#include "interface/TInterfaces.hpp"

#include "common/SysTime.hpp"
#include "executor/IExecutor.hpp"
#include "finalhaven/IFinalHaven.hpp"

#include "mqtt/IMqttConnection.hpp"
#include "events/EventListener.hpp"
#include "CDeviceConfigParser.hpp"


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
    struct DevServiceBlk
    {
        uint32_t spent_attempt;
        HBE::SysTimeMsec time_to_attempt;
    };

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
    /// @brief create device list based on config
    ///
    void createDeviceList();

    ///
    /// @brief subscribe mqtt topics
    ///
    void mqttSubscribe() const;

    ///
    /// @brief callback from mqtt interface
    ///
    void mqttEventProcessFunc(const std::unique_ptr<Mqtt::TMqttEventVariant>& ev);

    ///
    /// @brief iterable method
    ///
    void processDeviceList();

private:
    Depends                  m_dependencies;
    System::IFinalHavenPtr   m_final_haven;
    System::IExecutorPtr     m_executor;
    Mqtt::IMqttConnectionPtr m_mqtt_conn;
    CDeviceConfigParserPtr   m_dev_cfg;

    Common::TEventHolderListener<Mqtt::TMqttEventVariant> m_mqtt_listener;
    std::unordered_map<std::string, DevServiceBlk> m_device_items;
};
} // namespace WBridge