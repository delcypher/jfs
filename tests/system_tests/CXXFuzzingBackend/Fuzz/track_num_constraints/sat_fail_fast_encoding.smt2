; RUN: rm -f %t-stats.yml
; RUN: %jfs -cxx -branch-encoding=fail-fast -stats-file=%t-stats.yml -record-max-num-satisfied-constraints %s | %FileCheck -check-prefix=CHECK-SAT %s
; RUN: %yaml-syntax-check %t-stats.yml
; RUN: %FileCheck -check-prefix=CHECK-STATS -input-file=%t-stats.yml %s
; CHECK-SAT: {{^sat}}

; CHECK-STATS
; CHECK-STATS: name: CXXProgramBuilderPassImpl
; CHECK-STATS-NEXT: num_constraints: 8
; CHECK-STATS: name: runtime_fuzzing_stats
; CHECK-STATS-NEXT: jfs_max_num_const_sat: 8


(set-logic QF_BV)
(set-info :source |
Bit-vector benchmarks from Dawson Engler's tool contributed by Vijay Ganesh
(vganesh@stanford.edu).  Translated into SMT-LIB format by Clark Barrett using
CVC3.

|)
(set-info :smt-lib-version 2.0)
(set-info :category "industrial")
(set-info :status sat)
(declare-fun buffer_0 () (_ BitVec 8))
(declare-fun buffer_1 () (_ BitVec 8))
(declare-fun buffer_2 () (_ BitVec 8))
(assert (not (= ((_ sign_extend 24) buffer_0) (_ bv0 32))))
(assert (not (= ((_ sign_extend 24) buffer_0) (_ bv43 32))))
(assert (= ((_ sign_extend 24) buffer_0) (_ bv37 32)))
(assert (not (= ((_ sign_extend 24) buffer_1) (_ bv0 32))))
(assert (not (= ((_ sign_extend 24) buffer_1) (_ bv37 32))))
(assert (not (= ((_ sign_extend 24) buffer_1) (_ bv45 32))))
(assert (not (= ((_ sign_extend 24) buffer_1) (_ bv48 32))))
(assert (bvsle (_ bv48 32) ((_ sign_extend 24) buffer_1)))
(assert (bvsle ((_ sign_extend 24) buffer_1) (_ bv57 32)))
(assert (bvsle (_ bv48 32) ((_ sign_extend 24) buffer_2)))
(assert (not (bvsle ((_ sign_extend 24) buffer_2) (_ bv57 32))))
(assert (not (= ((_ sign_extend 24) buffer_2) (_ bv115 32))))
(assert (not (= ((_ sign_extend 24) buffer_2) (_ bv100 32))))
(assert (= ((_ sign_extend 24) buffer_2) (_ bv120 32)))
(assert (not (bvsle (bvadd (_ bv0 32) (bvadd ((_ sign_extend 24) buffer_1) (bvneg (_ bv48 32)))) (_ bv7 32))))
(assert (not (bvslt (_ bv0 32) (bvadd (bvadd (bvadd (_ bv0 32) (bvadd ((_ sign_extend 24) buffer_1) (bvneg (_ bv48 32)))) (bvneg (_ bv7 32))) (bvneg (_ bv1 32))))))
(check-sat)
(exit)
