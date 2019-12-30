//===------------ target_impl.h - AMDGCN OpenMP GPU options ------ CUDA -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Definitions of target specific functions
//
//===----------------------------------------------------------------------===//
#ifndef __AMDGCN__
// The amdgcn deviceRTL can be compiled for nvptx. Including the corresponding
// target_impl file (which sets the same header guard) maintains this property
// while allowing use of the __kmpc_impl constructs in the nvptx subsections.
// This include can be dropped once sources are deduplicated to a common subdir
#include "../../nvptx/src/target_impl.h"
#endif

#ifndef _TARGET_IMPL_H_
#define _TARGET_IMPL_H_

#ifndef __AMDGCN__
#error "amdgcn target_impl.h expects to be compiled under __AMDGCN__"
#endif

#include <stdint.h>
#include <stddef.h>

#include "amdgcn_interface.h"
#include "cuda_shim.h"

#define DEVICE __attribute__((device))
#define INLINE inline DEVICE
#define NOINLINE __attribute__((noinline)) DEVICE
#define SHARED __attribute__((shared))
#define ALIGN(N) __attribute__((aligned(N)))

#include "hip_atomics.h"

////////////////////////////////////////////////////////////////////////////////
// Kernel options
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// The following def must match the absolute limit hardwired in the host RTL
// max number of threads per team
#define MAX_THREADS_PER_TEAM 1024

#define WARPSIZE 64

// The named barrier for active parallel threads of a team in an L1 parallel
// region to synchronize with each other.
#define L1_BARRIER (1)

// Maximum number of preallocated arguments to an outlined parallel/simd function.
// Anything more requires dynamic memory allocation.
#define MAX_SHARED_ARGS 20

// Maximum number of omp state objects per SM allocated statically in global
// memory.
#define OMP_STATE_COUNT 32
#define MAX_SM 64


#define OMP_ACTIVE_PARALLEL_LEVEL 128

// Data sharing related quantities, need to match what is used in the compiler.
enum DATA_SHARING_SIZES {
  // The maximum number of workers in a kernel.
  DS_Max_Worker_Threads = 960,
  // The size reserved for data in a shared memory slot.
  DS_Slot_Size = 256,
  // The slot size that should be reserved for a working warp.
  DS_Worker_Warp_Slot_Size = WARPSIZE * DS_Slot_Size,
  // The maximum number of warps in use
  DS_Max_Warp_Number = 16,
};

INLINE void __kmpc_impl_unpack(uint64_t val, uint32_t &lo, uint32_t &hi) {
  lo = (uint32_t)(val & UINT64_C(0x00000000FFFFFFFF));
  hi = (uint32_t)((val & UINT64_C(0xFFFFFFFF00000000)) >> 32);
}

INLINE uint64_t __kmpc_impl_pack(uint32_t lo, uint32_t hi) {
  return (((uint64_t)hi) << 32) | (uint64_t)lo;
}

static const __kmpc_impl_lanemask_t __kmpc_impl_all_lanes =
    UINT64_C(0xffffffffffffffff);

DEVICE __kmpc_impl_lanemask_t __kmpc_impl_lanemask_lt();

DEVICE __kmpc_impl_lanemask_t __kmpc_impl_lanemask_gt();

DEVICE uint32_t __kmpc_impl_smid();

INLINE double __kmpc_impl_get_wtick() { return ((double)1E-9); }

INLINE double __kmpc_impl_get_wtime() {
  return ((double)1.0 / 745000000.0) * __clock64();
}

INLINE uint64_t __kmpc_impl_ffs(uint64_t x) { return __builtin_ffsl(x); }

INLINE uint64_t __kmpc_impl_popc(uint64_t x) { return __builtin_popcountl(x); }

template <typename T> INLINE T __kmpc_impl_min(T x, T y) {
  return x < y ? x : y;
}

DEVICE __kmpc_impl_lanemask_t __kmpc_impl_activemask();

INLINE int32_t __kmpc_impl_shfl_sync(__kmpc_impl_lanemask_t, int32_t Var,
                                     int32_t SrcLane) {
  return __shfl(Var, SrcLane, WARPSIZE);
}

INLINE int32_t __kmpc_impl_shfl_down_sync(__kmpc_impl_lanemask_t, int32_t Var,
                                          uint32_t Delta, int32_t Width) {
  return __shfl_down(Var, Delta, Width);
}

INLINE void __kmpc_impl_syncthreads() { __builtin_amdgcn_s_barrier(); }

INLINE void __kmpc_impl_syncwarp(__kmpc_impl_lanemask_t) {
  // AMDGCN doesn't need to sync threads in a warp
}

INLINE void __kmpc_impl_named_sync(int barrier, uint32_t num_threads) {
  // we have protected the master warp from releasing from its barrier
  // due to a full workgroup barrier in the middle of a work function.
  // So it is ok to issue a full workgroup barrier here.
  __builtin_amdgcn_s_barrier();
}

EXTERN void __kmpc_impl_threadfence(void);
EXTERN void __kmpc_impl_threadfence_block(void);
EXTERN void __kmpc_impl_threadfence_system(void);

// Calls to the AMDGCN layer (assuming 1D layout)
EXTERN uint64_t __ockl_get_local_size(uint32_t);
EXTERN uint64_t __ockl_get_num_groups(uint32_t);
INLINE int GetThreadIdInBlock() { return __builtin_amdgcn_workitem_id_x(); }
INLINE int GetBlockIdInKernel() { return __builtin_amdgcn_workgroup_id_x(); }
INLINE int GetNumberOfBlocksInKernel() { return __ockl_get_num_groups(0); }
INLINE int GetNumberOfThreadsInBlock() { return __ockl_get_local_size(0); }

#endif
