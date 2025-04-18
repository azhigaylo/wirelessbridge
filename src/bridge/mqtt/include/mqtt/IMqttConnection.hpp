/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#pragma once

#include <string>

#include "common/HbeMacro.hpp"
#include "interface/TInterface.hpp"

#include "MqttEvents.hpp"
#include "events/EventLoopHolder.hpp"

namespace Mqtt{

class IMqttConnection : public HBE::TInterface<IMqttConnection>
                      , public Common::TEventLoopHolder<std::unique_ptr<TMqttEventVariant>>
{
public:
    ///
    /// @brief IMqttConnection default destructor
    ///
    virtual ~IMqttConnection() = default;

    ///
    /// @brief subscribe on mqtt topic
    /// @param topic mqtt topic name
    ///
    virtual void subscribeTopic(const std::string& topic) = 0;

    ///
    /// @brief publish mqtt topic
    /// @param topic mqtt topic name
    /// @param topmsgic message string
    ///
    virtual void setTopic(const std::string& topic, const std::string& msg) = 0;
};

using IMqttConnectionPtr  = std::shared_ptr<IMqttConnection>;

} // namespace Mqtt

DECLARE_INTERFACE(Mqtt::IMqttConnection);



