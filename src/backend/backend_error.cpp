#include "backend_error.h"

#include <cstdio>
#include <cstdlib>

namespace quant::codegen::detail {

[[noreturn]] void fail(const std::string& message) {
    std::fprintf(stderr, "quant-backend: %s\n", message.c_str());
    std::fflush(stderr);
    std::abort();
}

} // namespace quant::codegen::detail
