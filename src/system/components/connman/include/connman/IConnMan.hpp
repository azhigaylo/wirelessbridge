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

#include "IConnectable.hpp"

namespace System
{
class IConnMan : public HBE::TInterface<IConnMan>
{
public:
    ///
    /// @brief IConnMan default destructor
    ///
    virtual ~IConnMan() = default;

    ///
    /// @brief subscribe on reconnection
    /// @param name component name, needed to easy trace analysis
    /// @param client client's pointer
    ///
    virtual void subscribe(std::string name, IConnectableWPtr const client) = 0;

    ///
    /// @brief unsubscribe on reconnection
    /// @param name component name, needed to easy trace analysis
    ///
    virtual void unsubscribe(std::string name) = 0;
};

using IConnManPtr = std::shared_ptr<IConnMan>;

} // namespace System

DECLARE_INTERFACE(System::IConnMan);


