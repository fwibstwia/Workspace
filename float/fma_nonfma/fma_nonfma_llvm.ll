; ModuleID = 'fma_nonfma_llvm.c'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: nounwind uwtable
define float @f(float %a, float %b, float %c) #0 {
  %1 = alloca float, align 4
  %2 = alloca float, align 4
  %3 = alloca float, align 4
  %g = alloca float, align 4
  store float %a, float* %1, align 4
  store float %b, float* %2, align 4
  store float %c, float* %3, align 4
  %4 = load float* %1, align 4
  %5 = load float* %2, align 4
  %6 = load float* %3, align 4
  %7 = call float @llvm.fmuladd.f32(float %4, float %5, float %6)
  store float %7, float* %g, align 4
  %8 = load float* %g, align 4
  ret float %8
}

; Function Attrs: nounwind readnone
declare float @llvm.fmuladd.f32(float, float, float) #1

; Function Attrs: nounwind uwtable
define i32 @main() #0 {
  %1 = alloca i32, align 4
  %a = alloca float, align 4
  %b = alloca float, align 4
  %c = alloca float, align 4
  store i32 0, i32* %1
  %2 = load float* %a, align 4
  %3 = load float* %b, align 4
  %4 = load float* %c, align 4
  %5 = call float @f(float %2, float %3, float %4)
  ret i32 0
}

attributes #0 = { nounwind uwtable "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone }

!llvm.ident = !{!0}

!0 = metadata !{metadata !"Ubuntu clang version 3.4.2- (branches/release_34) (based on LLVM 3.4.2)"}
