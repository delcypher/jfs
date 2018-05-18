//===----------------------------------------------------------------------===//
//
//                        JFS - The JIT Fuzzing Solver
//
// Copyright 2018 J. Ryan Stinnett
//
// This file is distributed under the MIT license.
// See LICENSE.txt for details.
//
//===----------------------------------------------------------------------===//

#ifndef PRF_API_H
#define PRF_API_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// user-provided, required
int LLVMFuzzerTestOneInput(const uint8_t* Data, size_t Size);
typedef int TestOneInputT(const uint8_t* Data, size_t Size);
// user-provided, optional
void LLVMFuzzerAtExit();
typedef void AtExitT();

#ifdef __cplusplus
}
#endif // __cplusplus

namespace prf {

class API {
private:
  AtExitT* atExit;

public:
  API();
  TestOneInputT* TestOneInput;
  AtExitT AtExit;
};

} // prf

#endif // PRF_API_H