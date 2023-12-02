#include <chrono>

namespace HBE {

using SysTimeUsec = std::chrono::microseconds;
using SysTimeMsec = std::chrono::milliseconds;
///
/// @brief system time provide
/// @return steady clock time in microseconds
///
SysTimeUsec getSystemTimeUsec();

///
/// @brief system time provide
/// @return steady clock time in miliseconds
///
SysTimeMsec getSystemTimeMsec();

} // namespace HBE

