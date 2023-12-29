
#include "CExecutor.hpp"

#include <memory>
#include <thread>
#include <iostream>
#include <boost/bind.hpp>

#include "loger/slog.h"

namespace System
{

CExecutor::CExecutor(const ConfigType& /*cfg*/, const HBE::ResolvedDependencies dependencies)
    : m_dependencies{dependencies}
    , m_final_haven(m_dependencies.getInterface<FinalHaven>().lock())
    , m_io_service{std::make_shared<boost::asio::io_service>()}
    , m_work_object{std::make_shared<boost::asio::io_service::work>(*m_io_service)}
{
    printDebug("CExecutor/%s: created...", __FUNCTION__);
}

CExecutor::~CExecutor()
{
    try
    {
        stopExecution();
    }
    catch( std::exception & ex )
    {
        printError("CExecutor/%s: Failed to stop in destructor: %s", __FUNCTION__, ex.what());
    }

    printDebug("CExecutor/%s: deleted.", __FUNCTION__);
}

void CExecutor::startComponent()
{
    startExecution();
}

void CExecutor::startExecution()
{
    /// create worker threads. one thread for one hw core
    for (uint32_t i{0}; i < boost::thread::hardware_concurrency(); i++)
    {
        m_worker_threads.create_thread(boost::bind(&CExecutor::WorkerThread, this, m_io_service));
        printDebug("CExecutor/%s: worker %i created", __FUNCTION__, static_cast<int32_t>(i));
    }
}

void CExecutor::stopExecution()
{
    /// stop asinc timers
    for (auto cycle_item : m_cycle_items)
    {
        std::shared_ptr<boost::asio::deadline_timer> timer;
        timeout_func timeout_f;
        boost::posix_time::milliseconds timeout_ms(0);
        std::function<void()> exec_function;
        std::tie(timer, timeout_f, timeout_ms, exec_function) = cycle_item.second;

        timer->cancel();
    }
    /// remove all cyclic operations
    {
       std::unique_lock<std::mutex> lock(m_cycle_mutex);
       m_cycle_items.clear();
    }

    /// send stop event for io_service processing
    if (false == m_io_service->stopped())
    {
        m_work_object.reset();
        m_io_service->stop();
        m_worker_threads.join_all(); /// wait all worker threads
    }
}

void CExecutor::execute(std::function<void()> exec_f)
{
    m_io_service->post(exec_f);
}

uint32_t CExecutor::execute_cyclic(std::function<void()> exec_f, std::chrono::milliseconds time)
{
    std::shared_ptr<boost::asio::deadline_timer> exec_timer =
        std::make_shared<boost::asio::deadline_timer>(
            *m_io_service,
            boost::posix_time::milliseconds{time.count()});

    timeout_func exec_func = boost::function<void(const boost::system::error_code&, uint32_t)>(
        [this](const boost::system::error_code& /*er*/, uint32_t cycle_func_id)
        {
            std::unique_lock<std::mutex> lock(m_cycle_mutex);
            {
                auto cycle_item = m_cycle_items.find(cycle_func_id);
                if (cycle_item != m_cycle_items.end())
                {
                    printDebug("CExecutor/%s: execution cyclic function id = %i", __FUNCTION__, cycle_func_id);

                    std::shared_ptr<boost::asio::deadline_timer> timer;
                    timeout_func timeout_f;
                    boost::posix_time::milliseconds timeout_ms(0);
                    std::function<void()> exec_function;
                    std::tie(timer, timeout_f, timeout_ms, exec_function) = cycle_item->second;

                    m_io_service->post(exec_function);
                    timer->expires_at(timer->expires_at() + timeout_ms);
                    timer->async_wait(boost::bind(timeout_f, boost::asio::placeholders::error, cycle_func_id));
                }
            }
        });

    std::unique_lock<std::mutex> lock(m_cycle_mutex);
    {
        auto itr = m_cycle_items.insert(
            std::make_pair(
                rand(),
                std::make_tuple(exec_timer, exec_func, boost::posix_time::milliseconds{time.count()}, exec_f)));

        exec_timer->async_wait(boost::bind(exec_func, boost::asio::placeholders::error, itr.first->first));
        return itr.first->first;
    }
}

void CExecutor::stop_cyclic(uint32_t cyclic_id)
{
    std::unique_lock<std::mutex> lock(m_cycle_mutex);
    {
        auto cycle_itr = m_cycle_items.find(cyclic_id);
        if (cycle_itr != m_cycle_items.end())
        {
            m_cycle_items.erase(cycle_itr);
            printDebug("CExecutor/%s: cyclic function id = %i stopped", __FUNCTION__, cyclic_id);
        }
    }
}

void CExecutor::WorkerThread(std::shared_ptr<boost::asio::io_service> io_service) noexcept
{
    printDebug("CExecutor/%s: worker started", __FUNCTION__);

    while( true )
    {
        try
        {
            boost::system::error_code ec;
            io_service->run(ec);

            if (ec)
            {
                std::stringstream ss;
                ss << "io_service error while operate: " << ec.value();
                printError("CExecutor/%s: %s", __FUNCTION__, ss.str().c_str());

                m_final_haven->report(ErrorLevel::critical_level, ss.str());
            }
            break;
        }
        catch(std::exception & ex)
        {
            std::stringstream ss;
            ss << "Error while processing asynchronous function: " << ex.what();
            printError("CExecutor/%s: %s", __FUNCTION__, ss.str().c_str());

            m_final_haven->report(ErrorLevel::system_level, ss.str());
        }
    }

    printDebug("CExecutor/%s: worker stopped", __FUNCTION__);
}

} // System

DEFINE_COMPONENT(System::CExecutor);