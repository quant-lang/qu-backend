#pragma once

#include <cstdint>
#include <vector>

#include "qu-ir/ir.h"
#include "backend/mc.h"
#include "backend/target.h"

namespace quant::codegen {

// Full native pipeline: IR -> instruction selection -> machine code -> object.
struct NativeBackend {
    // emit_start controls whether the backend synthesizes the program entry
    // point (_start) in addition to the user functions.
    std::vector<uint8_t> generate(const IRProgram& program, bool emit_start,
                                  const Target& target);
};

} // namespace quant::codegen
