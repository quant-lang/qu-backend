#pragma once

#include <string>

namespace quant::codegen::detail {

/*
 * Fatal backend error (invalid IR, unsupported encoding, unresolved symbol).
 * Kept private to the library so the backend does not depend on the compiler's
 * logging subsystem.
 */
[[noreturn]] void fail(const std::string& message);

} // namespace quant::codegen::detail
