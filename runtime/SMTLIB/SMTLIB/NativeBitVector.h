//===----------------------------------------------------------------------===//
//
//                        JFS - The JIT Fuzzing Solver
//
// Copyright 2017 Daniel Liew
//
// This file is distributed under the MIT license.
// See LICENSE.txt for details.
//
//===----------------------------------------------------------------------===//
#ifndef JFS_RUNTIME_SMTLIB_NATIVE_BITVECTOR_H
#define JFS_RUNTIME_SMTLIB_NATIVE_BITVECTOR_H
#include "SMTLIB/BufferRef.h"
#include "SMTLIB/jassert.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint64_t jfs_nr_bitvector_ty;

#define JFS_NR_BITVECTOR_TY_BITWIDTH (sizeof(jfs_nr_bitvector_ty) * 8)

jfs_nr_bitvector_ty jfs_nr_make_bitvector(const uint8_t* bufferData,
                                          const uint64_t bufferSize,
                                          const uint64_t lowBit,
                                          const uint64_t highBit);

#ifdef __cplusplus
}
#endif

#endif
