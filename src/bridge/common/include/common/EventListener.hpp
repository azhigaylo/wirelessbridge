/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#pragma once

#include <memory>
#include <sstream>
#include <iostream>
#include <boost/function.hpp>
#include <boost/thread/scoped_thread.hpp>

#include "EventConsumer.hpp"

namespace Common{

///
/// @brief context which implements queue
///
template <typename T>
class TEventListener
{
typedef boost::function<void (const std::unique_ptr<T>&)> TEventPrcFunc;

public:
    ///
    /// @brief destructor for HudContext
    ///
    TEventListener(
        TEventPrcFunc func,
        std::shared_ptr<Common::TEventConsumer<T>> consumer
    )
    : m_interrupted{false}
    , m_event_prc_func(func)
    , m_consumer{consumer}
    , m_event_thread{std::make_unique<boost::scoped_thread<>>(boost::bind(&TEventListener::eventLoop, this))}
    {

    }

    ///
    /// @brief destructor for HudContext
    ///
    virtual ~TEventListener() noexcept final
    {
        m_interrupted = true;
    }

    ///
    /// @brief copy constructor
    ///
    TEventListener(TEventListener const&) = delete;

    ///
    /// @brief move constructor
    ///
    TEventListener(TEventListener&&) = delete;

    ///
    /// @brief operator =
    ///
    TEventListener& operator=(TEventListener const&) = delete;

    ///
    /// @brief move assignment operator, deleted to hide move assignment
    ///
    TEventListener& operator=(TEventListener &&) = delete;

    ///
    /// @brief add event in communication queue
    /// @param event - HUD event
    ///

    ///
    /// @brief get event from communication queue
    /// @return first element in queue
    ///

private:

    ///
    /// @brief processing events
    ///
    void eventLoop() noexcept
    {
        try
        {
            while (!m_interrupted.load(std::memory_order_relaxed))
            {
                std::unique_ptr<T> ev{m_consumer->getEvent()};
                if (ev)
                {
                    m_event_prc_func(ev);
                }
            }
        }
        catch (const std::exception& e)
        {
            std::stringstream ss_msg;
            ss_msg << __func__ << ": " << "Error: " << e.what() << std::endl;
            std::cout << ss_msg.str();
        }
    }

    std::atomic_bool m_interrupted;
    TEventPrcFunc m_event_prc_func;
    std::shared_ptr<Common::TEventConsumer<T>> m_consumer;
    std::unique_ptr<boost::scoped_thread<>> m_event_thread;
};

} /// namespase Common