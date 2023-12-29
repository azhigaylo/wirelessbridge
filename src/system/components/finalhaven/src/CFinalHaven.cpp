/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#include "CFinalHaven.hpp"

#include <iostream>

#include "loger/slog.h"

namespace System {

CFinalHaven::CFinalHaven(const ConfigType& /*cfg*/, const HBE::ResolvedDependencies /*dependencies*/)
    : m_err_queue (std::chrono::milliseconds(static_cast<int>(intCfg::err_queue_timeout_ms)))
{
    // Setup signal handlers
    signalHandlerWrapper = std::bind(&CFinalHaven::signalHandlerLocal,
                                 this,
                                 std::placeholders::_1);
    configureSignalHandlers();

    printDebug("CFinalHaven/%s: created...", __FUNCTION__);
}

CFinalHaven::~CFinalHaven()
{
    printDebug("CFinalHaven/%s: deleted.", __FUNCTION__);
}

void CFinalHaven::report(ErrorLevel err_level, std::string error_descr)
{
    std::cout << __func__ << ": error = " << error_descr.c_str() << std::endl;
    m_err_queue.push(std::make_unique<HbeError>(std::make_tuple(err_level, error_descr)));
}

std::future<HbeError> CFinalHaven::waitAction()
{
    return std::async(std::launch::async,[this]()
    {
        boost::optional<std::unique_ptr<HbeError>> action_item;

        do
        {
            std::future<boost::optional<std::unique_ptr<HbeError>>> fut{m_err_queue.pop()};
            action_item = fut.get();
        }
        while(!action_item);

        return *action_item.get();
    });
}

void CFinalHaven::configureSignalHandlers()
{
    struct sigaction sa;
    sa.sa_handler = signalHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);
}

void CFinalHaven::signalHandlerLocal(int sig)
{
    switch (sig)
    {
    case SIGTERM:
    case SIGINT:
    {
        std::cout << __func__ << ": User signal obtained [Signal: " << sig << " ], exit..." << std::endl;
        m_err_queue.push(std::make_unique<HbeError>(std::make_tuple(ErrorLevel::user_level, "Application termination requested by user")));
        break;
    }
    default:
    {
        std::cerr << __func__ << ": Fatal signal obtained [Signal: " << sig << " ], exit..." << std::endl;
        m_err_queue.push(std::make_unique<HbeError>(std::make_tuple(ErrorLevel::user_level, "Fatal signal obtained")));
    }
    }
}

} // namespace System

DEFINE_COMPONENT(System::CFinalHaven);
