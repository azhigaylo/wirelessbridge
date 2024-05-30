/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#pragma once

#include <memory>

namespace Common{

///
/// @brief template event sender class
/// @tparam T Type of the queue elements
///
template <typename T>
class TEventSender
{
public:
    ///
    /// @brief destructor
    ///
    virtual ~TEventSender() noexcept = default;

    ///
    /// @brief get event from communication queue
    /// @return first element in queue
    ///
    virtual void sendEvent(std::unique_ptr<T>) = 0;
};

} /// namespase Common