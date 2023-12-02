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
#include <boost/optional.hpp>

namespace Common{

///
/// @brief  template queue, support thread safe functionality
/// @tparam T Type of the queue elements
///
template<typename T>
class ThreadQueue
{
public:
    ///
    /// @brief default constructor
    ///
    ThreadQueue() = default;

    ///
    /// @brief default destructor
    ///
    ~ThreadQueue() = default;

    ///
    /// @brief operator =
    ///
    ThreadQueue& operator=(ThreadQueue<T> const& q) = delete;

    ///
    /// @brief move assignment operator, deleted to hide move assignment
    ///
    ThreadQueue& operator=(ThreadQueue<T>&& q) = delete;

    ///
    /// @brief copy constructor
    ///
    ThreadQueue(ThreadQueue<T> const& q)
    {
        std::lock_guard<std::mutex> lock(m_queue_mutex);
        m_queue = q.m_queue;
    }

    ///
    /// @brief move constructor
    ///
    ThreadQueue(ThreadQueue<T>&& q)
    {
        std::lock_guard<std::mutex> lock(m_queue_mutex);
        m_queue = std::move(q.m_queue);
    }

    ///
    /// @brief safe queue size
    /// @return uint32_t queue size
    ///
    uint32_t size() const
    {
        std::lock_guard<std::mutex> lock(m_queue_mutex);
        return static_cast<uint32_t>(m_queue.size());
    }

    ///
    /// @brief safe push element to queue
    /// @tparam T Type of the queue element by reference
    ///
    void push(const T &item)
    {
        std::lock_guard<std::mutex> lock(m_queue_mutex);
        m_queue.push(item);
    }

    ///
    /// @brief safe pop element from queue
    /// @tparam T Type of the queue element
    ///
    boost::optional<T> pop()
    {
        boost::optional<T> result;
        std::lock_guard<std::mutex> lock(m_queue_mutex);
        if (!m_queue.empty())
        {
            result = m_queue.front();
            m_queue.pop();
        }
        return result;
    }

private:
    std::queue<T> m_queue;
    mutable std::mutex m_queue_mutex;
};

} /// namespase Common