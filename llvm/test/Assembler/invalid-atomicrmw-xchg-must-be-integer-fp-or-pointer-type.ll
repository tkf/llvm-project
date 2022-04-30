; RUN: not llvm-as -disable-output %s 2>&1 | FileCheck %s

; CHECK: error: atomicrmw xchg operand must be an integer, floating point, or pointer type
define void @f(<3 x i1>* %ptr) {
  atomicrmw xchg <3 x i1>* %ptr, <3 x i1> <i1 1, i1 2, i1 3> seq_cst
  ret void
}
