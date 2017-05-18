; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: llc < %s -mtriple=i686-unknown-unknown -mattr=+sse2 | FileCheck %s --check-prefix=X86
; RUN: llc < %s -mtriple=x86_64-unknown-unknown -mattr=+sse2 | FileCheck %s --check-prefix=X64
; RUN: llc < %s -mtriple=x86_64-unknown-unknown -mattr=+f16c | FileCheck %s --check-prefix=F16C

define <1 x half> @ir_fadd_v1f16(<1 x half> %arg0, <1 x half> %arg1) nounwind {
; X86-LABEL: ir_fadd_v1f16:
; X86:       # BB#0:
; X86-NEXT:    subl $28, %esp
; X86-NEXT:    movss {{.*#+}} xmm0 = mem[0],zero,zero,zero
; X86-NEXT:    movss %xmm0, (%esp)
; X86-NEXT:    calll __gnu_f2h_ieee
; X86-NEXT:    movzwl %ax, %eax
; X86-NEXT:    movl %eax, (%esp)
; X86-NEXT:    calll __gnu_h2f_ieee
; X86-NEXT:    fstpt {{[0-9]+}}(%esp) # 10-byte Folded Spill
; X86-NEXT:    movss {{.*#+}} xmm0 = mem[0],zero,zero,zero
; X86-NEXT:    movss %xmm0, (%esp)
; X86-NEXT:    calll __gnu_f2h_ieee
; X86-NEXT:    movzwl %ax, %eax
; X86-NEXT:    movl %eax, (%esp)
; X86-NEXT:    fldt {{[0-9]+}}(%esp) # 10-byte Folded Reload
; X86-NEXT:    fstps {{[0-9]+}}(%esp)
; X86-NEXT:    calll __gnu_h2f_ieee
; X86-NEXT:    fstps {{[0-9]+}}(%esp)
; X86-NEXT:    movss {{.*#+}} xmm0 = mem[0],zero,zero,zero
; X86-NEXT:    addss {{[0-9]+}}(%esp), %xmm0
; X86-NEXT:    movss %xmm0, {{[0-9]+}}(%esp)
; X86-NEXT:    flds {{[0-9]+}}(%esp)
; X86-NEXT:    addl $28, %esp
; X86-NEXT:    retl
;
; X64-LABEL: ir_fadd_v1f16:
; X64:       # BB#0:
; X64-NEXT:    pushq %rax
; X64-NEXT:    movss %xmm0, {{[0-9]+}}(%rsp) # 4-byte Spill
; X64-NEXT:    movaps %xmm1, %xmm0
; X64-NEXT:    callq __gnu_f2h_ieee
; X64-NEXT:    movzwl %ax, %edi
; X64-NEXT:    callq __gnu_h2f_ieee
; X64-NEXT:    movss %xmm0, (%rsp) # 4-byte Spill
; X64-NEXT:    movss {{[0-9]+}}(%rsp), %xmm0 # 4-byte Reload
; X64-NEXT:    # xmm0 = mem[0],zero,zero,zero
; X64-NEXT:    callq __gnu_f2h_ieee
; X64-NEXT:    movzwl %ax, %edi
; X64-NEXT:    callq __gnu_h2f_ieee
; X64-NEXT:    addss (%rsp), %xmm0 # 4-byte Folded Reload
; X64-NEXT:    popq %rax
; X64-NEXT:    retq
;
; F16C-LABEL: ir_fadd_v1f16:
; F16C:       # BB#0:
; F16C-NEXT:    vcvtps2ph $4, %xmm1, %xmm1
; F16C-NEXT:    vcvtph2ps %xmm1, %xmm1
; F16C-NEXT:    vcvtps2ph $4, %xmm0, %xmm0
; F16C-NEXT:    vcvtph2ps %xmm0, %xmm0
; F16C-NEXT:    vaddss %xmm1, %xmm0, %xmm0
; F16C-NEXT:    retq
  %retval = fadd <1 x half> %arg0, %arg1
  ret <1 x half> %retval
}

define <2 x half> @ir_fadd_v2f16(<2 x half> %arg0, <2 x half> %arg1) nounwind {
; X86-LABEL: ir_fadd_v2f16:
; X86:       # BB#0:
; X86-NEXT:    subl $64, %esp
; X86-NEXT:    movss {{.*#+}} xmm0 = mem[0],zero,zero,zero
; X86-NEXT:    movss %xmm0, (%esp)
; X86-NEXT:    calll __gnu_f2h_ieee
; X86-NEXT:    movzwl %ax, %eax
; X86-NEXT:    movl %eax, (%esp)
; X86-NEXT:    calll __gnu_h2f_ieee
; X86-NEXT:    fstpt {{[0-9]+}}(%esp) # 10-byte Folded Spill
; X86-NEXT:    movss {{.*#+}} xmm0 = mem[0],zero,zero,zero
; X86-NEXT:    movss %xmm0, (%esp)
; X86-NEXT:    calll __gnu_f2h_ieee
; X86-NEXT:    movzwl %ax, %eax
; X86-NEXT:    movl %eax, (%esp)
; X86-NEXT:    calll __gnu_h2f_ieee
; X86-NEXT:    fstpt {{[0-9]+}}(%esp) # 10-byte Folded Spill
; X86-NEXT:    movss {{.*#+}} xmm0 = mem[0],zero,zero,zero
; X86-NEXT:    movss %xmm0, (%esp)
; X86-NEXT:    calll __gnu_f2h_ieee
; X86-NEXT:    movzwl %ax, %eax
; X86-NEXT:    movl %eax, (%esp)
; X86-NEXT:    calll __gnu_h2f_ieee
; X86-NEXT:    fstpt {{[0-9]+}}(%esp) # 10-byte Folded Spill
; X86-NEXT:    movss {{.*#+}} xmm0 = mem[0],zero,zero,zero
; X86-NEXT:    movss %xmm0, (%esp)
; X86-NEXT:    calll __gnu_f2h_ieee
; X86-NEXT:    movzwl %ax, %eax
; X86-NEXT:    movl %eax, (%esp)
; X86-NEXT:    fldt {{[0-9]+}}(%esp) # 10-byte Folded Reload
; X86-NEXT:    fstps {{[0-9]+}}(%esp)
; X86-NEXT:    fldt {{[0-9]+}}(%esp) # 10-byte Folded Reload
; X86-NEXT:    fstps {{[0-9]+}}(%esp)
; X86-NEXT:    fldt {{[0-9]+}}(%esp) # 10-byte Folded Reload
; X86-NEXT:    fstps {{[0-9]+}}(%esp)
; X86-NEXT:    calll __gnu_h2f_ieee
; X86-NEXT:    fstps {{[0-9]+}}(%esp)
; X86-NEXT:    movss {{.*#+}} xmm0 = mem[0],zero,zero,zero
; X86-NEXT:    movss {{.*#+}} xmm1 = mem[0],zero,zero,zero
; X86-NEXT:    addss {{[0-9]+}}(%esp), %xmm1
; X86-NEXT:    addss {{[0-9]+}}(%esp), %xmm0
; X86-NEXT:    movss %xmm0, {{[0-9]+}}(%esp)
; X86-NEXT:    movss %xmm1, {{[0-9]+}}(%esp)
; X86-NEXT:    flds {{[0-9]+}}(%esp)
; X86-NEXT:    flds {{[0-9]+}}(%esp)
; X86-NEXT:    addl $64, %esp
; X86-NEXT:    retl
;
; X64-LABEL: ir_fadd_v2f16:
; X64:       # BB#0:
; X64-NEXT:    subq $24, %rsp
; X64-NEXT:    movss %xmm2, {{[0-9]+}}(%rsp) # 4-byte Spill
; X64-NEXT:    movss %xmm1, {{[0-9]+}}(%rsp) # 4-byte Spill
; X64-NEXT:    movss %xmm0, {{[0-9]+}}(%rsp) # 4-byte Spill
; X64-NEXT:    movaps %xmm3, %xmm0
; X64-NEXT:    callq __gnu_f2h_ieee
; X64-NEXT:    movzwl %ax, %edi
; X64-NEXT:    callq __gnu_h2f_ieee
; X64-NEXT:    movss %xmm0, {{[0-9]+}}(%rsp) # 4-byte Spill
; X64-NEXT:    movss {{[0-9]+}}(%rsp), %xmm0 # 4-byte Reload
; X64-NEXT:    # xmm0 = mem[0],zero,zero,zero
; X64-NEXT:    callq __gnu_f2h_ieee
; X64-NEXT:    movzwl %ax, %edi
; X64-NEXT:    callq __gnu_h2f_ieee
; X64-NEXT:    movss %xmm0, {{[0-9]+}}(%rsp) # 4-byte Spill
; X64-NEXT:    movss {{[0-9]+}}(%rsp), %xmm0 # 4-byte Reload
; X64-NEXT:    # xmm0 = mem[0],zero,zero,zero
; X64-NEXT:    callq __gnu_f2h_ieee
; X64-NEXT:    movzwl %ax, %edi
; X64-NEXT:    callq __gnu_h2f_ieee
; X64-NEXT:    movss %xmm0, {{[0-9]+}}(%rsp) # 4-byte Spill
; X64-NEXT:    movss {{[0-9]+}}(%rsp), %xmm0 # 4-byte Reload
; X64-NEXT:    # xmm0 = mem[0],zero,zero,zero
; X64-NEXT:    callq __gnu_f2h_ieee
; X64-NEXT:    movzwl %ax, %edi
; X64-NEXT:    callq __gnu_h2f_ieee
; X64-NEXT:    addss {{[0-9]+}}(%rsp), %xmm0 # 4-byte Folded Reload
; X64-NEXT:    movss {{[0-9]+}}(%rsp), %xmm1 # 4-byte Reload
; X64-NEXT:    # xmm1 = mem[0],zero,zero,zero
; X64-NEXT:    addss {{[0-9]+}}(%rsp), %xmm1 # 4-byte Folded Reload
; X64-NEXT:    addq $24, %rsp
; X64-NEXT:    retq
;
; F16C-LABEL: ir_fadd_v2f16:
; F16C:       # BB#0:
; F16C-NEXT:    vcvtps2ph $4, %xmm3, %xmm3
; F16C-NEXT:    vcvtph2ps %xmm3, %xmm3
; F16C-NEXT:    vcvtps2ph $4, %xmm1, %xmm1
; F16C-NEXT:    vcvtph2ps %xmm1, %xmm1
; F16C-NEXT:    vaddss %xmm3, %xmm1, %xmm1
; F16C-NEXT:    vcvtps2ph $4, %xmm2, %xmm2
; F16C-NEXT:    vcvtph2ps %xmm2, %xmm2
; F16C-NEXT:    vcvtps2ph $4, %xmm0, %xmm0
; F16C-NEXT:    vcvtph2ps %xmm0, %xmm0
; F16C-NEXT:    vaddss %xmm2, %xmm0, %xmm0
; F16C-NEXT:    retq
  %retval = fadd <2 x half> %arg0, %arg1
  ret <2 x half> %retval
}
