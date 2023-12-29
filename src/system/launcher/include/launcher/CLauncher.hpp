/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#include <string>
#include <cstdint>
#include <cstdlib>

#include "managers/ComponentManagerFactory.hpp"

namespace System
{
///
/// @brief this class encapsulate component launcher and serve for start all components
///
class CLauncher final
{
public:

    struct AppDebugInfo
    {
        uint32_t debug_level;
        uint32_t debug_sink;
    };

public:
    ///
    /// @brief constructor executor class
    ///
    explicit CLauncher(
        AppDebugInfo const& debug_data,
        std::string const&  cfg);

    ///
    /// @brief destructor executor class
    ///
    ~CLauncher() noexcept;

    ///
    /// @brief CLauncher remove copy constructor
    ///
    CLauncher(const CLauncher&) = delete;

    ///
    /// @brief CLauncher remove operator '='
    ///
    CLauncher& operator=(const CLauncher&) & = delete;

    ///
    /// @brief start application
    ///
    int startApplication();

private:
    const uint32_t    m_debug_level;
    const uint32_t    m_debug_sink;
    const std::string m_config_file;
};
} // System
