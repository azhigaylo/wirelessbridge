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
#include "EventLoopHolder.hpp"

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
        std::shared_ptr<Common::TEventConsumer<T>> consumer,
        TEventPrcFunc func
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
                if (std::shared_ptr<Common::TEventConsumer<T>> consumer = m_consumer.lock(); consumer)
                {
                    std::unique_ptr<T> ev{consumer->consumeEvent()};
                    if (ev)
                    {
                        m_event_prc_func(ev);
                    }
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
    std::weak_ptr<Common::TEventConsumer<T>> m_consumer;
    std::unique_ptr<boost::scoped_thread<>> m_event_thread;
};

///
/// @brief context which implements queue
///
template <typename T>
class TEventHolderListener
{
typedef boost::function<void (const std::unique_ptr<T>&)> TEventPrcFunc;

public:
    ///
    /// @brief destructor for HudContext
    ///
    TEventHolderListener(
        std::shared_ptr<Common::TEventLoopHolder<T>> event_holder,
        TEventPrcFunc func
    )
    : m_event_prc_func(func)
    , m_event_holder{event_holder}
    , m_event_thread{std::make_unique<boost::scoped_thread<>>(boost::bind(&TEventHolderListener::eventLoop, this))}
    {

    }

    ///
    /// @brief destructor for HudContext
    ///
    virtual ~TEventHolderListener() noexcept final
    {
        m_interrupted = true;
    }

    ///
    /// @brief copy constructor
    ///
    TEventHolderListener(TEventHolderListener const&) = delete;

    ///
    /// @brief move constructor
    ///
    TEventHolderListener(TEventHolderListener&&) = delete;

    ///
    /// @brief operator =
    ///
    TEventHolderListener& operator=(TEventHolderListener const&) = delete;

    ///
    /// @brief move assignment operator, deleted to hide move assignment
    ///
    TEventHolderListener& operator=(TEventHolderListener &&) = delete;

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
                if (std::shared_ptr<Common::TEventLoopHolder<T>> holder = m_event_holder.lock(); holder)
                {
                    std::unique_ptr<T> ev{holder->getEventConsumer()->consumeEvent()};
                    if (ev)
                    {
                        m_event_prc_func(ev);
                    }
                }
                else break;
            }
        }
        catch (const std::exception& e)
        {
            std::stringstream ss_msg;
            ss_msg << __func__ << ": " << "Error: " << e.what() << std::endl;
            std::cout << ss_msg.str();
        }
    }

    std::atomic_bool m_interrupted{false};
    TEventPrcFunc m_event_prc_func;
    std::weak_ptr<Common::TEventLoopHolder<T>> m_event_holder;
    std::unique_ptr<boost::scoped_thread<>> m_event_thread;
};

} /// namespace Common
