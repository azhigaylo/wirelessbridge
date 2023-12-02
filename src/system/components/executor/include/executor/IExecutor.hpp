/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#pragma once

#include <boost/function.hpp>

#include "common/HbeMacro.hpp"
#include "interface/TInterface.hpp"

namespace System
{
///
/// @brief base class for asynchronous executor.
class IExecutor : public HBE::TInterface<IExecutor>
{
public:
    ///
    /// @brief destructor for IExecutor
    ///
    virtual ~IExecutor()noexcept = default;

    ///
    /// @brief set start routine on execution
    /// @param exec_f function for asynchronous execution
    ///
    virtual void execute(std::function<void()> exec_f) = 0;

    ///
    /// @brief set start routine on execution
    /// @param exec_f function for cyclic asynchronous execution
    /// @return cyclic function id
    ///
    virtual uint32_t execute_cyclic(std::function<void()> exec_f, std::chrono::milliseconds time) = 0;

    ///
    /// @brief stop cyclic function id
    /// @param cyclic_id cyclic function id
    ///
    virtual void stop_cyclic(uint32_t cyclic_id) = 0;
};

using IExecutorPtr  = std::shared_ptr<IExecutor>;
using IExecutorWPtr = std::shared_ptr<IExecutor>;

} // System

DECLARE_INTERFACE(System::IExecutor);