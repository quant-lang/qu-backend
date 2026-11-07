#include "backend/code_emitter.h"
#include "backend_error.h"

namespace quant::codegen {
    CodeEmitter::CodeEmitter(x86::Emitter& e, mc::Object& o, mc::SymbolTable& s)
        : emitter_(&e), object_(&o), symbols_(&s), arch_(mc::TargetArch::X86_64) {}
    CodeEmitter::CodeEmitter(aarch64::Emitter& e, mc::Object& o, mc::SymbolTable& s)
        : emitter_(&e), object_(&o), symbols_(&s), arch_(mc::TargetArch::AARCH64) {}

    x86::Emitter& CodeEmitter::x86() {
        if (arch_ != mc::TargetArch::X86_64)
            detail::fail("CodeEmitter: expected x86-64");
        return *static_cast<quant::codegen::x86::Emitter*>(emitter_);
    }
    aarch64::Emitter& CodeEmitter::aarch64() {
        if (arch_ != mc::TargetArch::AARCH64)
            detail::fail("CodeEmitter: expected AArch64");
        return *static_cast<quant::codegen::aarch64::Emitter*>(emitter_);
    }
    mc::Object& CodeEmitter::object() { return *object_; }
    mc::SymbolTable& CodeEmitter::symbols() { return *symbols_; }
} // namespace quant::codegen
