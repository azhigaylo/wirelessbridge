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

namespace WBridge
{
class IWirelessBridge : public HBE::TInterface<IWirelessBridge>
{
public:
    ///
    /// @brief IWirelessBridge default destructor
    ///
    virtual ~IWirelessBridge() = default;
};

using IWirelessBridgePtr = std::shared_ptr<IWirelessBridge>;

} // namespace WBridge

DECLARE_INTERFACE(WBridge::IWirelessBridge);


