/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#pragma once

#include <memory>
#include <utility>
#include <iostream>

#include "EventSender.hpp"
#include "BlockingQueue.hpp"
#include "EventConsumer.hpp"

namespace Common{

///
/// @brief event loop which implements queue
///
template <typename T>
class TEventLoop final
    : public TEventConsumer<T>
    , public TEventSender<T>
{
public:
    ///
    /// @brief destructor for HudContext
    ///
    virtual ~TEventLoop() noexcept final = default;

    ///
    /// @brief add event in communication queue
    /// @param event - HUD event
    ///
    void sendEvent(T ev) final
    {
        m_queue.push(std::forward<T>(ev));
    }

    ///
    /// @brief get event from communication queue
    /// @return first element in queue
    ///
    T consumeEvent() final
    {
        T ev;
        m_queue.pop(ev);
        return (ev);
    }

private:

    TBlockingQueue<T> m_queue;
};

} /// namespase Common