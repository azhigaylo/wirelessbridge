#include "common/SysTime.hpp"

#include <chrono>

namespace HBE {

SysTimeUsec getSystemTimeUsec()
{
    return std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now().time_since_epoch());
}

SysTimeMsec getSystemTimeMsec()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch());
}

} // namespace HBE

