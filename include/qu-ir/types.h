#pragma once

namespace quant::quir {

/* Scalar type kinds understood by QuIR. Layout is shared with the compiler's
 * AST so values can be carried through the pipeline without conversion.
 */
enum class TypeKind {
    Void,
    Bool,

    I8,
    I16,
    I32,
    I64,

    U8,
    U16,
    U32,
    U64,

    F32,
    F64,

    Struct,
    Pointer,
    Reference,

    NullPtr,

    Generic,

    Enum,

    Count
};

// How a value is converted by IRCast.
enum class CastKind {
    ValueCast,   // as
    Bitcast,     // as!
    Format       // numeric as str: lowers to a qk_format_* call
};

} // namespace quant::quir
