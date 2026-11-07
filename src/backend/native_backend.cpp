#include "backend/native_backend.h"

#include "backend/mc.h"
#include "backend_error.h"

#ifdef QUANT_HAS_ELF
#include "backend/elf_writer.h"
#endif
#ifdef QUANT_HAS_PE
#include "backend/pe_writer.h"
#endif
#ifdef QUANT_HAS_X86_64
#include "backend/isel.h"
#endif
#ifdef QUANT_HAS_AARCH64
#include "backend/aarch64_isel.h"
#endif

namespace quant::codegen {

namespace {

[[noreturn]] void backend_not_compiled(const char* name) {
    detail::fail(std::string(name) +
                 " code generator is not compiled into this library "
                 "(enable it via -DQUANT_BACKENDS=...)");
}

} // namespace

std::vector<uint8_t> NativeBackend::generate(const IRProgram& program, bool emit_start,
                                              const Target& target) {
    mc::Object obj;
    if (target.arch() == mc::TargetArch::AARCH64) {
#ifdef QUANT_HAS_AARCH64
        AArch64ISel isel(target);
        isel.should_emit_start = emit_start;
        isel.generate(program);
        obj = std::move(isel.obj);
#else
        backend_not_compiled("AArch64");
#endif
    } else {
#ifdef QUANT_HAS_X86_64
        ISel isel(emit_start, target);
        isel.generate(program);
        obj = std::move(isel.obj);
#else
        backend_not_compiled("x86-64");
#endif
    }
    if (target.object_format() == mc::ObjectFormat::PE) {
#ifdef QUANT_HAS_PE
        return pe::write(obj, target.image_base());
#else
        backend_not_compiled("PE writer");
#endif
    }
#ifdef QUANT_HAS_ELF
    return elf::write(obj, target.arch());
#else
    backend_not_compiled("ELF writer");
#endif
}

} // namespace quant::codegen
