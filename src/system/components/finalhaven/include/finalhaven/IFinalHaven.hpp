/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#pragma once

#include <tuple>
#include <future>
#include <string>

#include "common/HbeMacro.hpp"
#include "interface/TInterface.hpp"

namespace System
{

//using DependenciesInfo = std::map<std::string, InterfaceTypeInfo>;

enum class ErrorLevel: uint8_t
{
    user_level      = 0x00,   // program will be closed immediately because user action
    critical_level  = 0x01,   // program will be closed immediately
    system_level    = 0x02,   // program can work but system functionality will be limited
    component_level = 0x03    // programm can work but component functionality wiil be unstable
};

using HbeError = std::tuple<ErrorLevel, std::string>;

class IFinalHaven : public HBE::TInterface<IFinalHaven>
{
public:
    ///
    /// @brief IFinalHaven default destructor
    ///
    virtual ~IFinalHaven() = default;

    ///
    /// @brief error report
    /// @param err_level level
    /// @param error_descr description
    ///
    virtual void report(ErrorLevel err_level, std::string error_descr) = 0;

    ///
    /// @brief wait error
    /// @return error description
    ///
    virtual std::future<HbeError> waitAction() = 0;

};

using IFinalHavenPtr  = std::shared_ptr<IFinalHaven>;
using IFinalHavenWPtr = std::weak_ptr<IFinalHaven>;

} // namespace System

DECLARE_INTERFACE(System::IFinalHaven);


