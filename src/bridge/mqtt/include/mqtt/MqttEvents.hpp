/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#pragma once


#include <string>
#include <vector>
#include <iostream>
#include <boost/variant.hpp>

namespace Mqtt{

///
/// @brief event which informs about communication error
///
class CMqttOnlineEvent
{
public:
    ///
    /// @brief constructor
    /// @param bool communication status
    ///
    explicit CMqttOnlineEvent(bool const status)
    : m_comm_status{status}{}

    ///
    /// @brief get communication status
    /// @return bool communication status (true - online / false - offline)
    ///
    bool getCommStatus()const {return m_comm_status;}

private:
    bool const m_comm_status;
};

///
/// @brief event which informs about topic update
///
class CMqttTopicUpdateEvent
{
public:
    ///
    /// @brief constructor
    /// @param bool communication status
    ///
    explicit CMqttTopicUpdateEvent(std::string const topic, std::string const message)
    : m_topic{topic}
    , m_message{message} {}

    ///
    /// @brief get updated topic
    /// @return std::string const topic data
    ///
    std::string const getTopic()const {return m_topic;}

    ///
    /// @brief get updated topic
    /// @return std::string message data
    ///
    std::string const getMessage()const {return m_message;}

private:
    std::string const m_topic;
    std::string const m_message;
};

typedef boost::variant<CMqttOnlineEvent,
                       CMqttTopicUpdateEvent> TMqttEventVariant;

} // namespace Mqtt