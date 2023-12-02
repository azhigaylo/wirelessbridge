/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#pragma once

#include <csignal>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "finalhaven/IFinalHaven.hpp"

#include "component/TDepends.hpp"
#include "component/TComponent.hpp"
#include "interface/TInterfaces.hpp"

#include "common/TUniQueue.hpp"

IMPLEMENT_INTERFACE(FinalHaven, System::IFinalHaven);

namespace System {

struct CFinalHavenAnnotation
{
    using Interfaces = HBE::TInterfaces<FinalHaven>;
    using Depends    = HBE::TDepends<>;
    using ConfigType = boost::property_tree::ptree;
};

///
/// @brief
///
class CFinalHaven final
    : public IFinalHaven
    , public HBE::TComponent<CFinalHavenAnnotation>
    , public std::enable_shared_from_this<CFinalHaven>
{

enum class intCfg
{
    err_queue_timeout_ms  = 500
};

public:
    ///
    /// @brief constructor
    ///
    explicit CFinalHaven(
        const ConfigType& cfg,
        const HBE::ResolvedDependencies dependencies);

    ///
    /// @brief CFinalHaven default destructor
    ///
    ~CFinalHaven() noexcept final;

    ///
    /// @brief CFinalHaven remove copy constructor
    ///
    CFinalHaven(const CFinalHaven&) = delete;

    ///
    /// @brief CFinalHaven remove operator '='
    ///
    CFinalHaven& operator=(const CFinalHaven&) & = delete;

    ///------- IComponent implementation

    ///
    /// @brief register my interfaces
    ///
    void registerInterfaces() override {REGISTER_INTERFACE(FinalHaven)}

    ///
    /// @brief init component, subscribe for events, connect to interfaces and so on
    ///
    void initComponent() override {};

    ///
    /// @brief start component, do component main cycle
    ///
    void startComponent() override {};

public:
    ///------- IFinalHaven implementation
    ///
    /// @brief error report
    /// @param err_level level
    /// @param error_descr description
    ///
    void report(ErrorLevel err_level, std::string error_descr) override;

    ///
    /// @brief wait error
    /// @return error description
    ///
    std::future<HbeError> waitAction() override;

private:
    ///
    /// @brief local signal handler, used for init signalHandlerWrapper
    /// @param sig system signal
    ///
    void signalHandlerLocal(int sig);

    ///
    /// @brief global signal handler
    /// @param sig system signal
    ///
    static void signalHandler(int sig) {signalHandlerWrapper(sig);}

    ///
    /// @brief init signal handler to process SIGTERM, SIGINT signals
    ///
    void configureSignalHandlers();

private:
    HBE::TUniQueue<std::unique_ptr<HbeError>> m_err_queue;
    static inline std::function<void(int)> signalHandlerWrapper = nullptr;
};

} // namespace System
