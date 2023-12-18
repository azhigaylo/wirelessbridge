/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#pragma once

#include <memory>

#include "EventConsumer.hpp"

namespace Common{

///
/// @brief event loop which implements queue
///
template <typename T>
class TEventLoopHolder
{
public:

    ///
    /// @brief destructor for HudContext
    ///
    virtual ~TEventLoopHolder() noexcept = default;

    ///
    /// @brief get event queue context
    /// @return context which contain event queue
    ///
    virtual const std::shared_ptr<Common::TEventConsumer<T>> getEventContext() const = 0;
};

} /// namespase Common