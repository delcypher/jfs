; RUN: %jfs-smt2cxx %s > %t.cpp
; RUN: %cxx-rt-syntax %t.cpp
; RUN: %FileCheck -input-file=%t.cpp %s
(declare-fun a () (_ BitVec 8))
(declare-fun b () (_ BitVec 8))
(declare-fun c () (_ BitVec 8))
; CHECK: [[SSA0:[a-z_0-9]+]] = (a.bvadd(b)).bvadd(c);
; CHECK: bool [[SSA2:[a-z_0-9]+]] = [[SSA0]] == {{[a-z_0-9]+}};
; CHECK: if ([[SSA2]]) {}
(assert (= (bvadd a b c) (_ bv0 8)))
(check-sat)
