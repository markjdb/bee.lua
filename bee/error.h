#pragma once

#include <system_error>

namespace bee {
    std::system_error make_crterror(const char* message = nullptr);
    std::system_error make_syserror(const char* message = nullptr);
    std::system_error make_neterror(const char* message = nullptr);
    std::system_error make_error(int err, const char* message = nullptr);
    std::string error_message(const std::error_code& ec);
}
