/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <boost/optional.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace HBE
{

class IComponentConfigParser
{
public:
    ///
    /// @brief IMqttConnection default destructor
    ///
    virtual ~IComponentConfigParser() noexcept = default;

    ///
    /// @brief get list of the components name
    /// @return list of the components name
    ///
    virtual std::vector<std::string> getComponentList() const = 0;

    ///
    /// @brief get config for selected component
    /// @param cmp_name component name
    /// @return component library path
    ///
    virtual std::string getComponentLib(std::string const cmp_name) = 0;

    ///
    /// @brief get config for selected component
    /// @param cmp_name component name
    /// @return component configuration
    ///
    virtual boost::property_tree::ptree getComponentConfig(std::string const cmp_name) = 0;
};

using IComponentConfigParserPtr  = std::shared_ptr<IComponentConfigParser>;
using IComponentConfigParserUPtr = std::unique_ptr<IComponentConfigParser>;

} /// namespace HBE

