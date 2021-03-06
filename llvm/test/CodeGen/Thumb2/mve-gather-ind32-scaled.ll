; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: llc -mtriple=thumbv8.1m.main-arm-none-eabi -mattr=+mve.fp -enable-arm-maskedgatscat %s -o 2>/dev/null - | FileCheck %s

define arm_aapcs_vfpcc <4 x i32> @zext_scaled_i16_i32(i16* %base, <4 x i32>* %offptr) {
; CHECK-LABEL: zext_scaled_i16_i32:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vldrw.u32 q0, [r1]
; CHECK-NEXT:    vshl.i32 q0, q0, #1
; CHECK-NEXT:    vadd.i32 q0, q0, r0
; CHECK-NEXT:    vmov r2, s0
; CHECK-NEXT:    vmov r0, s1
; CHECK-NEXT:    vmov r3, s2
; CHECK-NEXT:    vmov r1, s3
; CHECK-NEXT:    ldrh r2, [r2]
; CHECK-NEXT:    ldrh r0, [r0]
; CHECK-NEXT:    vmov.32 q0[0], r2
; CHECK-NEXT:    ldrh r3, [r3]
; CHECK-NEXT:    vmov.32 q0[1], r0
; CHECK-NEXT:    ldrh r1, [r1]
; CHECK-NEXT:    vmov.32 q0[2], r3
; CHECK-NEXT:    vmov.32 q0[3], r1
; CHECK-NEXT:    vmovlb.u16 q0, q0
; CHECK-NEXT:    bx lr
entry:
  %offs = load <4 x i32>, <4 x i32>* %offptr, align 4
  %ptrs = getelementptr inbounds i16, i16* %base, <4 x i32> %offs
  %gather = call <4 x i16> @llvm.masked.gather.v4i16.v4p0i16(<4 x i16*> %ptrs, i32 2, <4 x i1> <i1 true, i1 true, i1 true, i1 true>, <4 x i16> undef)
  %gather.zext = zext <4 x i16> %gather to <4 x i32>
  ret <4 x i32> %gather.zext
}

define arm_aapcs_vfpcc <4 x i32> @sext_scaled_i16_i32(i16* %base, <4 x i32>* %offptr) {
; CHECK-LABEL: sext_scaled_i16_i32:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vldrw.u32 q0, [r1]
; CHECK-NEXT:    vshl.i32 q0, q0, #1
; CHECK-NEXT:    vadd.i32 q0, q0, r0
; CHECK-NEXT:    vmov r2, s0
; CHECK-NEXT:    vmov r0, s1
; CHECK-NEXT:    vmov r3, s2
; CHECK-NEXT:    vmov r1, s3
; CHECK-NEXT:    ldrh r2, [r2]
; CHECK-NEXT:    ldrh r0, [r0]
; CHECK-NEXT:    vmov.32 q0[0], r2
; CHECK-NEXT:    ldrh r3, [r3]
; CHECK-NEXT:    vmov.32 q0[1], r0
; CHECK-NEXT:    ldrh r1, [r1]
; CHECK-NEXT:    vmov.32 q0[2], r3
; CHECK-NEXT:    vmov.32 q0[3], r1
; CHECK-NEXT:    vmovlb.s16 q0, q0
; CHECK-NEXT:    bx lr
entry:
  %offs = load <4 x i32>, <4 x i32>* %offptr, align 4
  %ptrs = getelementptr inbounds i16, i16* %base, <4 x i32> %offs
  %gather = call <4 x i16> @llvm.masked.gather.v4i16.v4p0i16(<4 x i16*> %ptrs, i32 2, <4 x i1> <i1 true, i1 true, i1 true, i1 true>, <4 x i16> undef)
  %gather.sext = sext <4 x i16> %gather to <4 x i32>
  ret <4 x i32> %gather.sext
}

define arm_aapcs_vfpcc <4 x i32> @scaled_i32_i32(i32* %base, <4 x i32>* %offptr) {
; CHECK-LABEL: scaled_i32_i32:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vldrw.u32 q0, [r1]
; CHECK-NEXT:    vshl.i32 q0, q0, #2
; CHECK-NEXT:    vadd.i32 q1, q0, r0
; CHECK-NEXT:    vldrw.u32 q0, [q1]
; CHECK-NEXT:    bx lr
entry:
  %offs = load <4 x i32>, <4 x i32>* %offptr, align 4
  %ptrs = getelementptr inbounds i32, i32* %base, <4 x i32> %offs
  %gather = call <4 x i32> @llvm.masked.gather.v4i32.v4p0i32(<4 x i32*> %ptrs, i32 4, <4 x i1> <i1 true, i1 true, i1 true, i1 true>, <4 x i32> undef)
  ret <4 x i32> %gather
}

; TODO: scaled_f16_i32

define arm_aapcs_vfpcc <4 x float> @scaled_f32_i32(i32* %base, <4 x i32>* %offptr) {
; CHECK-LABEL: scaled_f32_i32:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vldrw.u32 q0, [r1]
; CHECK-NEXT:    vshl.i32 q0, q0, #2
; CHECK-NEXT:    vadd.i32 q1, q0, r0
; CHECK-NEXT:    vldrw.u32 q0, [q1]
; CHECK-NEXT:    bx lr
entry:
  %offs = load <4 x i32>, <4 x i32>* %offptr, align 4
  %i32_ptrs = getelementptr inbounds i32, i32* %base, <4 x i32> %offs
  %ptrs = bitcast <4 x i32*> %i32_ptrs to <4 x float*>
  %gather = call <4 x float> @llvm.masked.gather.v4f32.v4p0f32(<4 x float*> %ptrs, i32 4, <4 x i1> <i1 true, i1 true, i1 true, i1 true>, <4 x float> undef)
  ret <4 x float> %gather
}

define arm_aapcs_vfpcc <4 x i32> @unsigned_scaled_b_i32_i16(i32* %base, <4 x i16>* %offptr) {
; CHECK-LABEL: unsigned_scaled_b_i32_i16:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vldrh.u32 q0, [r1]
; CHECK-NEXT:    vshl.i32 q0, q0, #2
; CHECK-NEXT:    vadd.i32 q1, q0, r0
; CHECK-NEXT:    vldrw.u32 q0, [q1]
; CHECK-NEXT:    bx lr
entry:
  %offs = load <4 x i16>, <4 x i16>* %offptr, align 2
  %offs.zext = zext <4 x i16> %offs to <4 x i32>
  %ptrs = getelementptr inbounds i32, i32* %base, <4 x i32> %offs.zext
  %gather = call <4 x i32> @llvm.masked.gather.v4i32.v4p0i32(<4 x i32*> %ptrs, i32 4, <4 x i1> <i1 true, i1 true, i1 true, i1 true>, <4 x i32> undef)
  ret <4 x i32> %gather
}

define arm_aapcs_vfpcc <4 x i32> @signed_scaled_i32_i16(i32* %base, <4 x i16>* %offptr) {
; CHECK-LABEL: signed_scaled_i32_i16:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vldrh.s32 q0, [r1]
; CHECK-NEXT:    vshl.i32 q0, q0, #2
; CHECK-NEXT:    vadd.i32 q1, q0, r0
; CHECK-NEXT:    vldrw.u32 q0, [q1]
; CHECK-NEXT:    bx lr
entry:
  %offs = load <4 x i16>, <4 x i16>* %offptr, align 2
  %offs.sext = sext <4 x i16> %offs to <4 x i32>
  %ptrs = getelementptr inbounds i32, i32* %base, <4 x i32> %offs.sext
  %gather = call <4 x i32> @llvm.masked.gather.v4i32.v4p0i32(<4 x i32*> %ptrs, i32 4, <4 x i1> <i1 true, i1 true, i1 true, i1 true>, <4 x i32> undef)
  ret <4 x i32> %gather
}

define arm_aapcs_vfpcc <4 x float> @a_unsigned_scaled_f32_i16(i32* %base, <4 x i16>* %offptr) {
; CHECK-LABEL: a_unsigned_scaled_f32_i16:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vldrh.u32 q0, [r1]
; CHECK-NEXT:    vshl.i32 q0, q0, #2
; CHECK-NEXT:    vadd.i32 q1, q0, r0
; CHECK-NEXT:    vldrw.u32 q0, [q1]
; CHECK-NEXT:    bx lr
entry:
  %offs = load <4 x i16>, <4 x i16>* %offptr, align 2
  %offs.zext = zext <4 x i16> %offs to <4 x i32>
  %i32_ptrs = getelementptr inbounds i32, i32* %base, <4 x i32> %offs.zext
  %ptrs = bitcast <4 x i32*> %i32_ptrs to <4 x float*>
  %gather = call <4 x float> @llvm.masked.gather.v4f32.v4p0f32(<4 x float*> %ptrs, i32 4, <4 x i1> <i1 true, i1 true, i1 true, i1 true>, <4 x float> undef)
  ret <4 x float> %gather
}

define arm_aapcs_vfpcc <4 x float> @b_signed_scaled_f32_i16(i32* %base, <4 x i16>* %offptr) {
; CHECK-LABEL: b_signed_scaled_f32_i16:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vldrh.s32 q0, [r1]
; CHECK-NEXT:    vshl.i32 q0, q0, #2
; CHECK-NEXT:    vadd.i32 q1, q0, r0
; CHECK-NEXT:    vldrw.u32 q0, [q1]
; CHECK-NEXT:    bx lr
entry:
  %offs = load <4 x i16>, <4 x i16>* %offptr, align 2
  %offs.sext = sext <4 x i16> %offs to <4 x i32>
  %i32_ptrs = getelementptr inbounds i32, i32* %base, <4 x i32> %offs.sext
  %ptrs = bitcast <4 x i32*> %i32_ptrs to <4 x float*>
  %gather = call <4 x float> @llvm.masked.gather.v4f32.v4p0f32(<4 x float*> %ptrs, i32 4, <4 x i1> <i1 true, i1 true, i1 true, i1 true>, <4 x float> undef)
  ret <4 x float> %gather
}

define arm_aapcs_vfpcc <4 x i32> @zext_signed_scaled_i16_i16(i16* %base, <4 x i16>* %offptr) {
; CHECK-LABEL: zext_signed_scaled_i16_i16:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vldrh.s32 q0, [r1]
; CHECK-NEXT:    vshl.i32 q0, q0, #1
; CHECK-NEXT:    vadd.i32 q0, q0, r0
; CHECK-NEXT:    vmov r2, s0
; CHECK-NEXT:    vmov r0, s1
; CHECK-NEXT:    vmov r3, s2
; CHECK-NEXT:    vmov r1, s3
; CHECK-NEXT:    ldrh r2, [r2]
; CHECK-NEXT:    ldrh r0, [r0]
; CHECK-NEXT:    vmov.32 q0[0], r2
; CHECK-NEXT:    ldrh r3, [r3]
; CHECK-NEXT:    vmov.32 q0[1], r0
; CHECK-NEXT:    ldrh r1, [r1]
; CHECK-NEXT:    vmov.32 q0[2], r3
; CHECK-NEXT:    vmov.32 q0[3], r1
; CHECK-NEXT:    vmovlb.u16 q0, q0
; CHECK-NEXT:    bx lr
entry:
  %offs = load <4 x i16>, <4 x i16>* %offptr, align 2
  %offs.sext = sext <4 x i16> %offs to <4 x i32>
  %ptrs = getelementptr inbounds i16, i16* %base, <4 x i32> %offs.sext
  %gather = call <4 x i16> @llvm.masked.gather.v4i16.v4p0i16(<4 x i16*> %ptrs, i32 2, <4 x i1> <i1 true, i1 true, i1 true, i1 true>, <4 x i16> undef)
  %gather.zext = zext <4 x i16> %gather to <4 x i32>
  ret <4 x i32> %gather.zext
}

define arm_aapcs_vfpcc <4 x i32> @sext_signed_scaled_i16_i16(i16* %base, <4 x i16>* %offptr) {
; CHECK-LABEL: sext_signed_scaled_i16_i16:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vldrh.s32 q0, [r1]
; CHECK-NEXT:    vshl.i32 q0, q0, #1
; CHECK-NEXT:    vadd.i32 q0, q0, r0
; CHECK-NEXT:    vmov r2, s0
; CHECK-NEXT:    vmov r0, s1
; CHECK-NEXT:    vmov r3, s2
; CHECK-NEXT:    vmov r1, s3
; CHECK-NEXT:    ldrh r2, [r2]
; CHECK-NEXT:    ldrh r0, [r0]
; CHECK-NEXT:    vmov.32 q0[0], r2
; CHECK-NEXT:    ldrh r3, [r3]
; CHECK-NEXT:    vmov.32 q0[1], r0
; CHECK-NEXT:    ldrh r1, [r1]
; CHECK-NEXT:    vmov.32 q0[2], r3
; CHECK-NEXT:    vmov.32 q0[3], r1
; CHECK-NEXT:    vmovlb.s16 q0, q0
; CHECK-NEXT:    bx lr
entry:
  %offs = load <4 x i16>, <4 x i16>* %offptr, align 2
  %offs.sext = sext <4 x i16> %offs to <4 x i32>
  %ptrs = getelementptr inbounds i16, i16* %base, <4 x i32> %offs.sext
  %gather = call <4 x i16> @llvm.masked.gather.v4i16.v4p0i16(<4 x i16*> %ptrs, i32 2, <4 x i1> <i1 true, i1 true, i1 true, i1 true>, <4 x i16> undef)
  %gather.sext = sext <4 x i16> %gather to <4 x i32>
  ret <4 x i32> %gather.sext
}

define arm_aapcs_vfpcc <4 x i32> @zext_unsigned_scaled_i16_i16(i16* %base, <4 x i16>* %offptr) {
; CHECK-LABEL: zext_unsigned_scaled_i16_i16:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vldrh.u32 q0, [r1]
; CHECK-NEXT:    vshl.i32 q0, q0, #1
; CHECK-NEXT:    vadd.i32 q0, q0, r0
; CHECK-NEXT:    vmov r2, s0
; CHECK-NEXT:    vmov r0, s1
; CHECK-NEXT:    vmov r3, s2
; CHECK-NEXT:    vmov r1, s3
; CHECK-NEXT:    ldrh r2, [r2]
; CHECK-NEXT:    ldrh r0, [r0]
; CHECK-NEXT:    vmov.32 q0[0], r2
; CHECK-NEXT:    ldrh r3, [r3]
; CHECK-NEXT:    vmov.32 q0[1], r0
; CHECK-NEXT:    ldrh r1, [r1]
; CHECK-NEXT:    vmov.32 q0[2], r3
; CHECK-NEXT:    vmov.32 q0[3], r1
; CHECK-NEXT:    vmovlb.u16 q0, q0
; CHECK-NEXT:    bx lr
entry:
  %offs = load <4 x i16>, <4 x i16>* %offptr, align 2
  %offs.zext = zext <4 x i16> %offs to <4 x i32>
  %ptrs = getelementptr inbounds i16, i16* %base, <4 x i32> %offs.zext
  %gather = call <4 x i16> @llvm.masked.gather.v4i16.v4p0i16(<4 x i16*> %ptrs, i32 2, <4 x i1> <i1 true, i1 true, i1 true, i1 true>, <4 x i16> undef)
  %gather.zext = zext <4 x i16> %gather to <4 x i32>
  ret <4 x i32> %gather.zext
}

define arm_aapcs_vfpcc <4 x i32> @sext_unsigned_scaled_i16_i16(i16* %base, <4 x i16>* %offptr) {
; CHECK-LABEL: sext_unsigned_scaled_i16_i16:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vldrh.u32 q0, [r1]
; CHECK-NEXT:    vshl.i32 q0, q0, #1
; CHECK-NEXT:    vadd.i32 q0, q0, r0
; CHECK-NEXT:    vmov r2, s0
; CHECK-NEXT:    vmov r0, s1
; CHECK-NEXT:    vmov r3, s2
; CHECK-NEXT:    vmov r1, s3
; CHECK-NEXT:    ldrh r2, [r2]
; CHECK-NEXT:    ldrh r0, [r0]
; CHECK-NEXT:    vmov.32 q0[0], r2
; CHECK-NEXT:    ldrh r3, [r3]
; CHECK-NEXT:    vmov.32 q0[1], r0
; CHECK-NEXT:    ldrh r1, [r1]
; CHECK-NEXT:    vmov.32 q0[2], r3
; CHECK-NEXT:    vmov.32 q0[3], r1
; CHECK-NEXT:    vmovlb.s16 q0, q0
; CHECK-NEXT:    bx lr
entry:
  %offs = load <4 x i16>, <4 x i16>* %offptr, align 2
  %offs.zext = zext <4 x i16> %offs to <4 x i32>
  %ptrs = getelementptr inbounds i16, i16* %base, <4 x i32> %offs.zext
  %gather = call <4 x i16> @llvm.masked.gather.v4i16.v4p0i16(<4 x i16*> %ptrs, i32 2, <4 x i1> <i1 true, i1 true, i1 true, i1 true>, <4 x i16> undef)
  %gather.sext = sext <4 x i16> %gather to <4 x i32>
  ret <4 x i32> %gather.sext
}

define arm_aapcs_vfpcc <4 x i32> @unsigned_scaled_b_i32_i8(i32* %base, <4 x i8>* %offptr) {
; CHECK-LABEL: unsigned_scaled_b_i32_i8:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vldrb.u32 q0, [r1]
; CHECK-NEXT:    vshl.i32 q0, q0, #2
; CHECK-NEXT:    vadd.i32 q1, q0, r0
; CHECK-NEXT:    vldrw.u32 q0, [q1]
; CHECK-NEXT:    bx lr
entry:
  %offs = load <4 x i8>, <4 x i8>* %offptr, align 1
  %offs.zext = zext <4 x i8> %offs to <4 x i32>
  %ptrs = getelementptr inbounds i32, i32* %base, <4 x i32> %offs.zext
  %gather = call <4 x i32> @llvm.masked.gather.v4i32.v4p0i32(<4 x i32*> %ptrs, i32 4, <4 x i1> <i1 true, i1 true, i1 true, i1 true>, <4 x i32> undef)
  ret <4 x i32> %gather
}

define arm_aapcs_vfpcc <4 x i32> @signed_scaled_i32_i8(i32* %base, <4 x i8>* %offptr) {
; CHECK-LABEL: signed_scaled_i32_i8:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vldrb.s32 q0, [r1]
; CHECK-NEXT:    vshl.i32 q0, q0, #2
; CHECK-NEXT:    vadd.i32 q1, q0, r0
; CHECK-NEXT:    vldrw.u32 q0, [q1]
; CHECK-NEXT:    bx lr
entry:
  %offs = load <4 x i8>, <4 x i8>* %offptr, align 1
  %offs.sext = sext <4 x i8> %offs to <4 x i32>
  %ptrs = getelementptr inbounds i32, i32* %base, <4 x i32> %offs.sext
  %gather = call <4 x i32> @llvm.masked.gather.v4i32.v4p0i32(<4 x i32*> %ptrs, i32 4, <4 x i1> <i1 true, i1 true, i1 true, i1 true>, <4 x i32> undef)
  ret <4 x i32> %gather
}

define arm_aapcs_vfpcc <4 x float> @a_unsigned_scaled_f32_i8(i32* %base, <4 x i8>* %offptr) {
; CHECK-LABEL: a_unsigned_scaled_f32_i8:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vldrb.u32 q0, [r1]
; CHECK-NEXT:    vshl.i32 q0, q0, #2
; CHECK-NEXT:    vadd.i32 q1, q0, r0
; CHECK-NEXT:    vldrw.u32 q0, [q1]
; CHECK-NEXT:    bx lr
entry:
  %offs = load <4 x i8>, <4 x i8>* %offptr, align 1
  %offs.zext = zext <4 x i8> %offs to <4 x i32>
  %i32_ptrs = getelementptr inbounds i32, i32* %base, <4 x i32> %offs.zext
  %ptrs = bitcast <4 x i32*> %i32_ptrs to <4 x float*>
  %gather = call <4 x float> @llvm.masked.gather.v4f32.v4p0f32(<4 x float*> %ptrs, i32 4, <4 x i1> <i1 true, i1 true, i1 true, i1 true>, <4 x float> undef)
  ret <4 x float> %gather
}

define arm_aapcs_vfpcc <4 x float> @b_signed_scaled_f32_i8(i32* %base, <4 x i8>* %offptr) {
; CHECK-LABEL: b_signed_scaled_f32_i8:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vldrb.s32 q0, [r1]
; CHECK-NEXT:    vshl.i32 q0, q0, #2
; CHECK-NEXT:    vadd.i32 q1, q0, r0
; CHECK-NEXT:    vldrw.u32 q0, [q1]
; CHECK-NEXT:    bx lr
entry:
  %offs = load <4 x i8>, <4 x i8>* %offptr, align 1
  %offs.sext = sext <4 x i8> %offs to <4 x i32>
  %i32_ptrs = getelementptr inbounds i32, i32* %base, <4 x i32> %offs.sext
  %ptrs = bitcast <4 x i32*> %i32_ptrs to <4 x float*>
  %gather = call <4 x float> @llvm.masked.gather.v4f32.v4p0f32(<4 x float*> %ptrs, i32 4, <4 x i1> <i1 true, i1 true, i1 true, i1 true>, <4 x float> undef)
  ret <4 x float> %gather
}

define arm_aapcs_vfpcc <4 x i32> @zext_signed_scaled_i16_i8(i16* %base, <4 x i8>* %offptr) {
; CHECK-LABEL: zext_signed_scaled_i16_i8:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vldrb.s32 q0, [r1]
; CHECK-NEXT:    vshl.i32 q0, q0, #1
; CHECK-NEXT:    vadd.i32 q0, q0, r0
; CHECK-NEXT:    vmov r2, s0
; CHECK-NEXT:    vmov r0, s1
; CHECK-NEXT:    vmov r3, s2
; CHECK-NEXT:    vmov r1, s3
; CHECK-NEXT:    ldrh r2, [r2]
; CHECK-NEXT:    ldrh r0, [r0]
; CHECK-NEXT:    vmov.32 q0[0], r2
; CHECK-NEXT:    ldrh r3, [r3]
; CHECK-NEXT:    vmov.32 q0[1], r0
; CHECK-NEXT:    ldrh r1, [r1]
; CHECK-NEXT:    vmov.32 q0[2], r3
; CHECK-NEXT:    vmov.32 q0[3], r1
; CHECK-NEXT:    vmovlb.u16 q0, q0
; CHECK-NEXT:    bx lr
entry:
  %offs = load <4 x i8>, <4 x i8>* %offptr, align 1
  %offs.sext = sext <4 x i8> %offs to <4 x i32>
  %ptrs = getelementptr inbounds i16, i16* %base, <4 x i32> %offs.sext
  %gather = call <4 x i16> @llvm.masked.gather.v4i16.v4p0i16(<4 x i16*> %ptrs, i32 2, <4 x i1> <i1 true, i1 true, i1 true, i1 true>, <4 x i16> undef)
  %gather.zext = zext <4 x i16> %gather to <4 x i32>
  ret <4 x i32> %gather.zext
}

define arm_aapcs_vfpcc <4 x i32> @sext_signed_scaled_i16_i8(i16* %base, <4 x i8>* %offptr) {
; CHECK-LABEL: sext_signed_scaled_i16_i8:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vldrb.s32 q0, [r1]
; CHECK-NEXT:    vshl.i32 q0, q0, #1
; CHECK-NEXT:    vadd.i32 q0, q0, r0
; CHECK-NEXT:    vmov r2, s0
; CHECK-NEXT:    vmov r0, s1
; CHECK-NEXT:    vmov r3, s2
; CHECK-NEXT:    vmov r1, s3
; CHECK-NEXT:    ldrh r2, [r2]
; CHECK-NEXT:    ldrh r0, [r0]
; CHECK-NEXT:    vmov.32 q0[0], r2
; CHECK-NEXT:    ldrh r3, [r3]
; CHECK-NEXT:    vmov.32 q0[1], r0
; CHECK-NEXT:    ldrh r1, [r1]
; CHECK-NEXT:    vmov.32 q0[2], r3
; CHECK-NEXT:    vmov.32 q0[3], r1
; CHECK-NEXT:    vmovlb.s16 q0, q0
; CHECK-NEXT:    bx lr
entry:
  %offs = load <4 x i8>, <4 x i8>* %offptr, align 1
  %offs.sext = sext <4 x i8> %offs to <4 x i32>
  %ptrs = getelementptr inbounds i16, i16* %base, <4 x i32> %offs.sext
  %gather = call <4 x i16> @llvm.masked.gather.v4i16.v4p0i16(<4 x i16*> %ptrs, i32 2, <4 x i1> <i1 true, i1 true, i1 true, i1 true>, <4 x i16> undef)
  %gather.sext = sext <4 x i16> %gather to <4 x i32>
  ret <4 x i32> %gather.sext
}

define arm_aapcs_vfpcc <4 x i32> @zext_unsigned_scaled_i16_i8(i16* %base, <4 x i8>* %offptr) {
; CHECK-LABEL: zext_unsigned_scaled_i16_i8:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vldrb.u32 q0, [r1]
; CHECK-NEXT:    vshl.i32 q0, q0, #1
; CHECK-NEXT:    vadd.i32 q0, q0, r0
; CHECK-NEXT:    vmov r2, s0
; CHECK-NEXT:    vmov r0, s1
; CHECK-NEXT:    vmov r3, s2
; CHECK-NEXT:    vmov r1, s3
; CHECK-NEXT:    ldrh r2, [r2]
; CHECK-NEXT:    ldrh r0, [r0]
; CHECK-NEXT:    vmov.32 q0[0], r2
; CHECK-NEXT:    ldrh r3, [r3]
; CHECK-NEXT:    vmov.32 q0[1], r0
; CHECK-NEXT:    ldrh r1, [r1]
; CHECK-NEXT:    vmov.32 q0[2], r3
; CHECK-NEXT:    vmov.32 q0[3], r1
; CHECK-NEXT:    vmovlb.u16 q0, q0
; CHECK-NEXT:    bx lr
entry:
  %offs = load <4 x i8>, <4 x i8>* %offptr, align 1
  %offs.zext = zext <4 x i8> %offs to <4 x i32>
  %ptrs = getelementptr inbounds i16, i16* %base, <4 x i32> %offs.zext
  %gather = call <4 x i16> @llvm.masked.gather.v4i16.v4p0i16(<4 x i16*> %ptrs, i32 2, <4 x i1> <i1 true, i1 true, i1 true, i1 true>, <4 x i16> undef)
  %gather.zext = zext <4 x i16> %gather to <4 x i32>
  ret <4 x i32> %gather.zext
}

define arm_aapcs_vfpcc <4 x i32> @sext_unsigned_scaled_i16_i8(i16* %base, <4 x i8>* %offptr) {
; CHECK-LABEL: sext_unsigned_scaled_i16_i8:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vldrb.u32 q0, [r1]
; CHECK-NEXT:    vshl.i32 q0, q0, #1
; CHECK-NEXT:    vadd.i32 q0, q0, r0
; CHECK-NEXT:    vmov r2, s0
; CHECK-NEXT:    vmov r0, s1
; CHECK-NEXT:    vmov r3, s2
; CHECK-NEXT:    vmov r1, s3
; CHECK-NEXT:    ldrh r2, [r2]
; CHECK-NEXT:    ldrh r0, [r0]
; CHECK-NEXT:    vmov.32 q0[0], r2
; CHECK-NEXT:    ldrh r3, [r3]
; CHECK-NEXT:    vmov.32 q0[1], r0
; CHECK-NEXT:    ldrh r1, [r1]
; CHECK-NEXT:    vmov.32 q0[2], r3
; CHECK-NEXT:    vmov.32 q0[3], r1
; CHECK-NEXT:    vmovlb.s16 q0, q0
; CHECK-NEXT:    bx lr
entry:
  %offs = load <4 x i8>, <4 x i8>* %offptr, align 1
  %offs.zext = zext <4 x i8> %offs to <4 x i32>
  %ptrs = getelementptr inbounds i16, i16* %base, <4 x i32> %offs.zext
  %gather = call <4 x i16> @llvm.masked.gather.v4i16.v4p0i16(<4 x i16*> %ptrs, i32 2, <4 x i1> <i1 true, i1 true, i1 true, i1 true>, <4 x i16> undef)
  %gather.sext = sext <4 x i16> %gather to <4 x i32>
  ret <4 x i32> %gather.sext
}

declare <4 x i8>  @llvm.masked.gather.v4i8.v4p0i8(<4 x i8*>, i32, <4 x i1>, <4 x i8>)
declare <4 x i16> @llvm.masked.gather.v4i16.v4p0i16(<4 x i16*>, i32, <4 x i1>, <4 x i16>)
declare <4 x i32> @llvm.masked.gather.v4i32.v4p0i32(<4 x i32*>, i32, <4 x i1>, <4 x i32>)
declare <4 x half> @llvm.masked.gather.v4f16.v4p0f16(<4 x half*>, i32, <4 x i1>, <4 x half>)
declare <4 x float> @llvm.masked.gather.v4f32.v4p0f32(<4 x float*>, i32, <4 x i1>, <4 x float>)
