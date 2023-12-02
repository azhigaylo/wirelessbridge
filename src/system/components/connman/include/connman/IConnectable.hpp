/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */
#pragma once

#include <memory>
#include <chrono>
#include <functional>

namespace System
{

///
/// @brief this class is interface class for reconnectable submodules
///
class IConnectable
{
public:

    // Connection status enum
    enum class EConnectionStatus : uint32_t
    {
        DISCONNECTED = 0,
        CONNECTING,
        CONNECTED
    };

    struct ConnectionInfo
    {
        uint32_t connection_attempt = 4;  // number of connection attempt to mark connection as "fail"
        std::chrono::milliseconds connection_timeout = std::chrono::milliseconds(1000);  // time which are covers connecting attempt to server
        std::chrono::milliseconds connection_cycle_timeout = std::chrono::milliseconds(5000);  // time to wait before next reconnection cycle
    };

    ///
    /// @brief IConnectable default destructor
    ///
    virtual ~IConnectable() = default;

    ///
    /// @brief Connecting to the remove server
    ///
    virtual void connect() = 0;

    ///
    /// @brief Disconnect from the remote server
    ///
    virtual void disconnect() = 0;

    ///
    /// @brief Indicates connection status
    ///
    virtual EConnectionStatus getState() const = 0;

    ///
    /// @brief Indicates connection status
    ///
    virtual ConnectionInfo getClientInfo() const = 0;

};

using IConnectablePtr  = std::shared_ptr<IConnectable>;
using IConnectableUPtr = std::unique_ptr<IConnectable>;
using IConnectableWPtr = std::weak_ptr<IConnectable>;

} // namespace System

