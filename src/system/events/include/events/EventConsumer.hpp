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
/// @brief tamplete event consumer class
/// @tparam T Type of the queue elements
///
template <typename T>
class TEventConsumer
{
public:
    ///
    /// @brief destructor
    ///
    virtual ~TEventConsumer() noexcept = default;

    ///
    /// @brief get event from communication queue
    /// @return first element in queue
    ///
    virtual std::unique_ptr<T> consumeEvent() = 0;
};

} /// namespase Common