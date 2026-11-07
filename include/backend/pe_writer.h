#pragma once

#include <cstdint>
#include <vector>

#include "backend/mc.h"

namespace quant::codegen::pe {

/*
 * Write an x86-64 PE32+ executable. Unlike the ELF writer, this resolves
 * relocations internally and creates a runnable .exe with an import table
 * for DLLs referenced by mc::Symbol.
 */
std::vector<uint8_t> write(const mc::Object& obj, uint64_t image_base);

} // namespace quant::codegen::pe
