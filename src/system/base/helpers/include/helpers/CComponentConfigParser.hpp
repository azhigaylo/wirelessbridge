/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#pragma once

#include "IComponentConfigParser.hpp"

#include <string>
#include <unordered_map>
#include <boost/property_tree/json_parser.hpp>

namespace HBE
{

class CComponentConfigParser final : public IComponentConfigParser
{
using cmp_data_pair = std::pair<std::string, boost::property_tree::ptree>;

public:
    ///
    /// @brief constructor
    /// @param cfg_path configuration path
    ///
    explicit CComponentConfigParser(std::string const cfg_path);

    ///
    /// @brief CMqttConnection default destructor
    ///
    ~CComponentConfigParser() noexcept final;

    ///
    /// @brief CMqttConnection remove copy and operators
    ///
    CComponentConfigParser(const CComponentConfigParser&) = delete;
    CComponentConfigParser& operator=(const CComponentConfigParser&) = delete;

    ///
    /// @brief get list of the components name
    /// @return list of the components name
    ///
    std::vector<std::string> getComponentList() const final;

    ///
    /// @brief get config for selected component
    /// @param cmp_name component name
    /// @return component library path
    ///
    std::string getComponentLib(std::string const cmp_name) final;

    ///
    /// @brief get config for selected component
    /// @param cmp_name component name
    /// @return component configuration
    ///
    boost::property_tree::ptree getComponentConfig(std::string const cmp_name) final;

private:
    std::vector<std::string> m_components_name;
    std::unordered_map<std::string, cmp_data_pair> m_components_map;
};

} //namespace HBE

