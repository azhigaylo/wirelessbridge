/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#pragma once

#include <stdint.h>

#include <queue>
#include <mutex>
#include <string>
#include <atomic>
#include <iostream>
#include <condition_variable>

#include "loger/slog.h"

namespace Common{

///
/// @brief template blocking queue, support blocking functionality
/// @tparam T Type of the queue elements
///
template <typename T>
class TBlockingQueue
{
public:

    ///
    /// @brief constructor
    /// @param timeout - timeout for blocking
    ///
    explicit TBlockingQueue(
        const std::chrono::milliseconds timeout = std::chrono::milliseconds(100)
    )
    : m_timeout(timeout)
    {

    }

    ///
    /// @brief destructor
    ///
    ~TBlockingQueue() noexcept
    {
        printDebug("TBlockingQueue/%s:  TBlockingQueue deleted ...", __FUNCTION__);
    }

    ///
    /// @brief copy constructor
    ///
    TBlockingQueue(TBlockingQueue<T> const&) = delete;

    ///
    /// @brief move constructor
    ///
    TBlockingQueue(TBlockingQueue<T>&&) = delete;

    ///
    /// @brief operator =
    ///
    TBlockingQueue& operator=(TBlockingQueue<T> const&) = delete;

    ///
    /// @brief move assignment operator, deleted to hide move assignment
    ///
    TBlockingQueue& operator=(TBlockingQueue<T> &&) = delete;

    ///
    /// @brief blocking pop element from queue
    /// @tparam T Type of the queue element
    ///
    void pop(T& item)
    {
        std::unique_lock<std::mutex> lock{m_queue_mutex};
        m_queue_cond_var.wait_for(lock, m_timeout, [&]() { return !m_queue.empty(); });

        if (m_queue.size())
        {
            item = std::move(m_queue.front());
            m_queue.pop();
        }
    }

    ///
    /// @brief push element to queue
    /// @tparam T Type of the queue element by reference
    ///
    void push(T&& item)
    {
        std::unique_lock<std::mutex> lock(m_queue_mutex);
        m_queue.push(std::move(item));
        lock.unlock();
        m_queue_cond_var.notify_one();
    }

    ///
    /// @brief get number of element in queue
    /// @return size_t - number of element in queue
    ///
    size_t count() const
    {
        return m_queue.size();
    }

    private:
    std::queue<T> m_queue;
    std::mutex m_queue_mutex;
    std::condition_variable m_queue_cond_var;
    std::chrono::milliseconds m_timeout;
};

} /// namespase Common