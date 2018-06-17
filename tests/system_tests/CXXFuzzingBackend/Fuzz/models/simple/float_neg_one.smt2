; RUN: %jfs %jfs_no_opt_options -libfuzzer-seed=1 -get-model %s | %FileCheck %s
; FIXME: Make LibPureRandomFuzzer work on this example?
; REQUIRES: LibFuzzer
(declare-const a Float32)
(assert (= a (fp #b1 #x7f #b00000000000000000000000)))
(check-sat)
; CHECK: {{^sat}}
; CHECK-NEXT: (
; CHECK-NEXT: (define-fun a () (_ FloatingPoint 8 24) (fp #b1 #x7f #b00000000000000000000000))
; CHECK-NEXT: )
