/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#include <mutex>
#include <chrono>
#include <unordered_map>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>

#include "executor/IExecutor.hpp"

#include "component/TDepends.hpp"
#include "component/TComponent.hpp"
#include "interface/TInterfaces.hpp"
#include "finalhaven/IFinalHaven.hpp"

IMPLEMENT_INTERFACE(Executor, System::IExecutor);

DEFINE_DEPENDENCY(FinalHaven, System::IFinalHaven);

namespace System
{

struct CExecutorAnnotation
{
    using Interfaces = HBE::TInterfaces<Executor>;
    using Depends    = HBE::TDepends<FinalHaven>;
    using ConfigType = boost::property_tree::ptree;
};

///
/// @brief this class encapsulate boost asio workers inside and serve for parallel processing of subroutines.
///
class CExecutor final
    : public IExecutor
    , public HBE::TComponent<CExecutorAnnotation>
    , public std::enable_shared_from_this<CExecutor>
{
using timeout_func = boost::function<void(const boost::system::error_code&, uint32_t)>;
using timer_data   = std::tuple<std::shared_ptr<boost::asio::deadline_timer>,
                                timeout_func,
                                boost::posix_time::milliseconds,
                                std::function<void()>>;
using timer_pair   = std::pair<uint32_t, timer_data>;

public:
    ///
    /// @brief constructor executor class
    ///
    explicit CExecutor(
        const ConfigType& cfg,
        const HBE::ResolvedDependencies dependencies);

    ///
    /// @brief destructor executor class
    ///
    ~CExecutor() noexcept override;

    ///
    /// @brief CExecutor remove copy constructor
    ///
    CExecutor(const CExecutor&) = delete;

    ///
    /// @brief CExecutor remove operator '='
    ///
    CExecutor& operator=(const CExecutor&) & = delete;

    ///------- IComponent implementation

    ///
    /// @brief register my interfaces
    ///
    void registerInterfaces() override {REGISTER_INTERFACE(Executor)}

    ///
    /// @brief init component, subscribe for events, connect to interfaces and so on
    ///
    void initComponent() override {};

    ///
    /// @brief start component, do component main cycle
    ///
    void startComponent() override;

public:
    ///------- IExecutor implementation
    ///
    /// @brief set start routine on execution
    /// @param exec_f function for asynchronous execution
    ///
    void execute(std::function<void()> exec_f) final;

    ///
    /// @brief set start routine on execution
    /// @param exec_f function for cyclic asynchronous execution
    /// @return cyclic function id
    ///
    uint32_t execute_cyclic(std::function<void()> exec_f, std::chrono::milliseconds time) final;

    ///
    /// @brief stop cyclic function id
    /// @param cyclic_id cyclic function id
    ///
    void stop_cyclic(uint32_t cyclic_id) final;

private:
    ///
    /// @brief create worker threads and start io_service
    ///
    void startExecution();

    ///
    /// @brief stop io_service and wait for all worker threads
    ///
    void stopExecution();

    ///
    /// @brief boost asio WorkerThread
    /// @param io_service io_service handler
    ///
    void WorkerThread(std::shared_ptr<boost::asio::io_service> io_service) noexcept;

private:
    Depends        m_dependencies;
    IFinalHavenPtr m_final_haven;

    boost::thread_group m_worker_threads; /// worker threads where runs boost asio run
    std::shared_ptr<boost::asio::io_service> m_io_service; /// boost asio io service instance
    std::shared_ptr<boost::asio::io_service::work> m_work_object; /// static boost asio worker

    std::mutex m_cycle_mutex;
    std::unordered_map<uint32_t, timer_data> m_cycle_items;
};
} // System
