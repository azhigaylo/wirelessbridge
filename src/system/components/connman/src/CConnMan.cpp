#include "CConnMan.hpp"

#include <thread>
#include <iostream>

#include "common/slog.h"

namespace System {

CConnMan::CConnMan(const ConfigType& /*cfg*/, const HBE::ResolvedDependencies dependencies)
    : m_dependencies{dependencies}
    , m_final_haven(m_dependencies.getInterface<FinalHaven>().lock())
    , m_asinc_executor{m_dependencies.getInterface<Executor>().lock()}
{
    m_asinc_executor->execute_cyclic(std::bind(&CConnMan::connectionHandler, this), std::chrono::milliseconds{100});
}

CConnMan::~CConnMan()
{
    printDebug("CConnMan/%s: deleted ...", __FUNCTION__);
}

void CConnMan::subscribe(std::string name, IConnectableWPtr const client)
{
    std::unique_lock<std::mutex> lock(m_connection_mutex);
    bool itm = m_connectable_items.insert(std::make_pair(name, ConServiceBlk{client, 0, std::chrono::milliseconds{0}})).second;

    if (false == itm)
    {
        m_final_haven->report(ErrorLevel::system_level, std::string("CConnMan error - duble insertion !"));
    }
    else
    {
        printDebug("CConnMan/%s:  component '%s' subscribed", __FUNCTION__, name.c_str());
    }
}

void CConnMan::unsubscribe(std::string name)
{
    std::unique_lock<std::mutex> lock(m_connection_mutex);

    auto conn_itr = m_connectable_items.find(name);
    if (conn_itr != m_connectable_items.end())
    {
        m_connectable_items.erase(conn_itr);
    }

    printDebug("CConnMan/%s:  component '%s' unsubscribed", __FUNCTION__, name.c_str());
}

void CConnMan::connectionHandler()
{
    HBE::SysTimeMsec itr_time = HBE::getSystemTimeMsec();

    printDebug("CConnMan/%s: iteration time = %i", __FUNCTION__, itr_time.count());

    /// process all connections
    std::unique_lock<std::mutex> lock(m_connection_mutex);
    for(auto item = m_connectable_items.begin(); item != m_connectable_items.end(); ++item)
    {
        if (IConnectablePtr conn_ptr = item->second.client.lock())
        {
            IConnectable::EConnectionStatus const conn_status = conn_ptr->getState();
            if (IConnectable::EConnectionStatus::DISCONNECTED == conn_status)
            {
                /// check, whether the time to wait connection has finished
                if (itr_time > item->second.time_to_attempt)
                {
                    /// check attempt counter
                    if (item->second.spent_attempt < conn_ptr->getClientInfo().connection_attempt)
                    {
                        printDebug("CConnMan/%s: call reconnect for '%s', attempt %i", __FUNCTION__, item->first.c_str(), item->second.spent_attempt);

                        conn_ptr->connect();

                        item->second.spent_attempt++;
                        item->second.time_to_attempt = itr_time +
                                                      conn_ptr->getClientInfo().connection_timeout +
                                                      conn_ptr->getClientInfo().connection_cycle_timeout;
                    }
                    else
                    {
                        if (item->second.spent_attempt++ == conn_ptr->getClientInfo().connection_attempt)
                        {
                            std::stringstream ss;
                            ss << "connection '"<< item->first << "' , all attempt exhausted !!!";
                            printError("CConnMan/%s: %s", __FUNCTION__, ss.str().c_str());

                            m_final_haven->report(ErrorLevel::system_level, ss.str());
                        }
                    }
                }
            }
            else
            {
                if (IConnectable::EConnectionStatus::CONNECTED == conn_status)
                {
                    // reset attempt counter
                    item->second.spent_attempt = 0;
                }
            }
        }
    }
}

} // namespace System

DEFINE_COMPONENT(System::CConnMan);