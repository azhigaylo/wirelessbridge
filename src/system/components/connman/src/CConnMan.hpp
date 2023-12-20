/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#pragma once

#include <mutex>
#include <unordered_map>

#include "connman/IConnMan.hpp"

#include "component/TDepends.hpp"
#include "component/TComponent.hpp"
#include "interface/TInterfaces.hpp"
#include "finalhaven/IFinalHaven.hpp"
#include "executor/IExecutor.hpp"

#include "common/SysTime.hpp"

IMPLEMENT_INTERFACE(ConnMan, System::IConnMan);

DEFINE_DEPENDENCY(FinalHaven, System::IFinalHaven);
DEFINE_DEPENDENCY(Executor, System::IExecutor);

namespace System {

struct CConnManAnnotation
{
    using Interfaces = HBE::TInterfaces<ConnMan>;
    using Depends    = HBE::TDepends<FinalHaven, Executor>;
    using ConfigType = boost::property_tree::ptree;
};

///
/// @brief
///
class CConnMan final
    : public IConnMan
    , public HBE::TComponent<CConnManAnnotation>
    , public std::enable_shared_from_this<CConnMan>
{

    struct ConServiceBlk
    {
        IConnectableWPtr const client;
        uint32_t spent_attempt;
        HBE::SysTimeMsec time_to_attempt;
    };

public:
    ///
    /// @brief constructor
    ///
    explicit CConnMan(
        const ConfigType& cfg,
        const HBE::ResolvedDependencies dependencies);

    ///
    /// @brief CConnMan default destructor
    ///
    ~CConnMan() noexcept final;

    ///
    /// @brief CConnMan remove copy constructor
    ///
    CConnMan(const CConnMan&) = delete;

    ///
    /// @brief CConnMan remove operator '='
    ///
    CConnMan& operator=(const CConnMan&) & = delete;

    ///------- IComponent implementation

    ///
    /// @brief register my interfaces
    ///
    void registerInterfaces() override {REGISTER_INTERFACE(ConnMan)}

    ///
    /// @brief init component, subscribe for events, connect to interfaces and so on
    ///
    void initComponent() override {};

    ///
    /// @brief start component, do component main cycle
    ///
    void startComponent() override {};

public:
    ///------- IConnMan implementation

    ///
    /// @brief subscribe on reconnection
    /// @param name component name, needed to easy trace analysis
    /// @param client client's pointer
    ///
    void subscribe(std::string name, IConnectableWPtr const client) override;

    ///
    /// @brief unsubscribe on reconnection
    /// @param name component name, needed to easy trace analysis
    ///
    void unsubscribe(std::string name) override;

private:
    ///
    /// @brief main loop
    ///
    void connectionHandler();

private:
    Depends        m_dependencies;
    IFinalHavenPtr m_final_haven;
    IExecutorPtr   m_asinc_executor;
    uint32_t       m_cyclic_func_id;

    std::mutex m_connection_mutex;
    std::unordered_map<std::string, ConServiceBlk> m_connectable_items;
};
} // namespace System