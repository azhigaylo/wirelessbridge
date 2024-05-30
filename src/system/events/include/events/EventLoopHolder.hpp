/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#pragma once

#include <memory>

#include "EventLoop.hpp"

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
    /// @brief get event consumer
    /// @return event consumer
    ///
    const std::shared_ptr<Common::TEventConsumer<T>> getEventConsumer() const
    {
        return m_event_loop;
    }

protected:
    ///
    /// @brief get event sender
    /// @return event sender
    ///
    const std::shared_ptr<Common::TEventSender<T>> getEventSender() const
    {
        return m_event_loop;
    }

private:

    const std::shared_ptr<Common::TEventLoop<T>> m_event_loop{std::make_shared<Common::TEventLoop<T>>()};
};

} /// namespace Common