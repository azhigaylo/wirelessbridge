#pragma once

#include <stdexcept>

namespace HBE {

class CComponentRegisterException : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

} // namespace HBE
