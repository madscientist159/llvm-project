//===- AArch64RegisterBankInfo.cpp -------------------------------*- C++ -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
/// \file
/// This file implements the targeting of the RegisterBankInfo class for
/// AArch64.
/// \todo This should be generated by TableGen.
//===----------------------------------------------------------------------===//

#include "AArch64RegisterBankInfo.h"
#include "AArch64InstrInfo.h" // For XXXRegClassID.
#include "llvm/CodeGen/LowLevelType.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/GlobalISel/RegisterBank.h"
#include "llvm/CodeGen/GlobalISel/RegisterBankInfo.h"
#include "llvm/Target/TargetRegisterInfo.h"
#include "llvm/Target/TargetSubtargetInfo.h"

// This file will be TableGen'ed at some point.
#include "AArch64GenRegisterBankInfo.def"

using namespace llvm;

#ifndef LLVM_BUILD_GLOBAL_ISEL
#error "You shouldn't build this"
#endif

AArch64RegisterBankInfo::AArch64RegisterBankInfo(const TargetRegisterInfo &TRI)
    : RegisterBankInfo(AArch64::RegBanks, AArch64::NumRegisterBanks) {
  static bool AlreadyInit = false;
  // We have only one set of register banks, whatever the subtarget
  // is. Therefore, the initialization of the RegBanks table should be
  // done only once. Indeed the table of all register banks
  // (AArch64::RegBanks) is unique in the compiler. At some point, it
  // will get tablegen'ed and the whole constructor becomes empty.
  if (AlreadyInit)
    return;
  AlreadyInit = true;
  // Initialize the GPR bank.
  createRegisterBank(AArch64::GPRRegBankID, "GPR");
  // The GPR register bank is fully defined by all the registers in
  // GR64all + its subclasses.
  addRegBankCoverage(AArch64::GPRRegBankID, AArch64::GPR64allRegClassID, TRI);
  const RegisterBank &RBGPR = getRegBank(AArch64::GPRRegBankID);
  (void)RBGPR;
  assert(&AArch64::GPRRegBank == &RBGPR &&
         "The order in RegBanks is messed up");
  assert(RBGPR.covers(*TRI.getRegClass(AArch64::GPR32RegClassID)) &&
         "Subclass not added?");
  assert(RBGPR.getSize() == 64 && "GPRs should hold up to 64-bit");

  // Initialize the FPR bank.
  createRegisterBank(AArch64::FPRRegBankID, "FPR");
  // The FPR register bank is fully defined by all the registers in
  // GR64all + its subclasses.
  addRegBankCoverage(AArch64::FPRRegBankID, AArch64::QQQQRegClassID, TRI);
  const RegisterBank &RBFPR = getRegBank(AArch64::FPRRegBankID);
  (void)RBFPR;
  assert(&AArch64::FPRRegBank == &RBFPR &&
         "The order in RegBanks is messed up");
  assert(RBFPR.covers(*TRI.getRegClass(AArch64::QQRegClassID)) &&
         "Subclass not added?");
  assert(RBFPR.covers(*TRI.getRegClass(AArch64::FPR64RegClassID)) &&
         "Subclass not added?");
  assert(RBFPR.getSize() == 512 &&
         "FPRs should hold up to 512-bit via QQQQ sequence");

  // Initialize the CCR bank.
  createRegisterBank(AArch64::CCRRegBankID, "CCR");
  addRegBankCoverage(AArch64::CCRRegBankID, AArch64::CCRRegClassID, TRI);
  const RegisterBank &RBCCR = getRegBank(AArch64::CCRRegBankID);
  (void)RBCCR;
  assert(&AArch64::CCRRegBank == &RBCCR &&
         "The order in RegBanks is messed up");
  assert(RBCCR.covers(*TRI.getRegClass(AArch64::CCRRegClassID)) &&
         "Class not added?");
  assert(RBCCR.getSize() == 32 && "CCR should hold up to 32-bit");

  // Check that the TableGen'ed like file is in sync we our expectations.
  // First, the Idx.
  assert(AArch64::PartialMappingIdx::GPR32 ==
             AArch64::PartialMappingIdx::FirstGPR &&
         "GPR32 index not first in the GPR list");
  assert(AArch64::PartialMappingIdx::GPR64 ==
             AArch64::PartialMappingIdx::LastGPR &&
         "GPR64 index not last in the GPR list");
  assert(AArch64::PartialMappingIdx::FirstGPR <=
             AArch64::PartialMappingIdx::LastGPR &&
         "GPR list is backward");
  assert(AArch64::PartialMappingIdx::FPR32 ==
             AArch64::PartialMappingIdx::FirstFPR &&
         "FPR32 index not first in the FPR list");
  assert(AArch64::PartialMappingIdx::FPR512 ==
             AArch64::PartialMappingIdx::LastFPR &&
         "FPR512 index not last in the FPR list");
  assert(AArch64::PartialMappingIdx::FirstFPR <=
             AArch64::PartialMappingIdx::LastFPR &&
         "FPR list is backward");
  assert(AArch64::PartialMappingIdx::FPR32 + 1 ==
             AArch64::PartialMappingIdx::FPR64 &&
         AArch64::PartialMappingIdx::FPR64 + 1 ==
             AArch64::PartialMappingIdx::FPR128 &&
         AArch64::PartialMappingIdx::FPR128 + 1 ==
             AArch64::PartialMappingIdx::FPR256 &&
         AArch64::PartialMappingIdx::FPR256 + 1 ==
             AArch64::PartialMappingIdx::FPR512 &&
         "FPR indices not properly ordered");
// Now, the content.
#define CHECK_PARTIALMAP(Idx, ValStartIdx, ValLength, RB)                      \
  do {                                                                         \
    const PartialMapping &Map =                                                \
        AArch64::PartMappings[AArch64::PartialMappingIdx::Idx];                \
    (void) Map;                                                                \
    assert(Map.StartIdx == ValStartIdx && Map.Length == ValLength &&           \
           Map.RegBank == &RB && #Idx " is incorrectly initialized");          \
  } while (0)

  CHECK_PARTIALMAP(GPR32, 0, 32, RBGPR);
  CHECK_PARTIALMAP(GPR64, 0, 64, RBGPR);
  CHECK_PARTIALMAP(FPR32, 0, 32, RBFPR);
  CHECK_PARTIALMAP(FPR64, 0, 64, RBFPR);
  CHECK_PARTIALMAP(FPR128, 0, 128, RBFPR);
  CHECK_PARTIALMAP(FPR256, 0, 256, RBFPR);
  CHECK_PARTIALMAP(FPR512, 0, 512, RBFPR);

  assert(verify(TRI) && "Invalid register bank information");
}

unsigned AArch64RegisterBankInfo::copyCost(const RegisterBank &A,
                                           const RegisterBank &B,
                                           unsigned Size) const {
  // What do we do with different size?
  // copy are same size.
  // Will introduce other hooks for different size:
  // * extract cost.
  // * build_sequence cost.
  // TODO: Add more accurate cost for FPR to/from GPR.
  return RegisterBankInfo::copyCost(A, B, Size);
}

const RegisterBank &AArch64RegisterBankInfo::getRegBankFromRegClass(
    const TargetRegisterClass &RC) const {
  switch (RC.getID()) {
  case AArch64::FPR8RegClassID:
  case AArch64::FPR16RegClassID:
  case AArch64::FPR32RegClassID:
  case AArch64::FPR64RegClassID:
  case AArch64::FPR128RegClassID:
  case AArch64::FPR128_loRegClassID:
  case AArch64::DDRegClassID:
  case AArch64::DDDRegClassID:
  case AArch64::DDDDRegClassID:
  case AArch64::QQRegClassID:
  case AArch64::QQQRegClassID:
  case AArch64::QQQQRegClassID:
    return getRegBank(AArch64::FPRRegBankID);
  case AArch64::GPR32commonRegClassID:
  case AArch64::GPR32RegClassID:
  case AArch64::GPR32spRegClassID:
  case AArch64::GPR32sponlyRegClassID:
  case AArch64::GPR32allRegClassID:
  case AArch64::GPR64commonRegClassID:
  case AArch64::GPR64RegClassID:
  case AArch64::GPR64spRegClassID:
  case AArch64::GPR64sponlyRegClassID:
  case AArch64::GPR64allRegClassID:
  case AArch64::tcGPR64RegClassID:
  case AArch64::WSeqPairsClassRegClassID:
  case AArch64::XSeqPairsClassRegClassID:
    return getRegBank(AArch64::GPRRegBankID);
  case AArch64::CCRRegClassID:
    return getRegBank(AArch64::CCRRegBankID);
  default:
    llvm_unreachable("Register class not supported");
  }
}

RegisterBankInfo::InstructionMappings
AArch64RegisterBankInfo::getInstrAlternativeMappings(
    const MachineInstr &MI) const {
  switch (MI.getOpcode()) {
  case TargetOpcode::G_OR: {
    // 32 and 64-bit or can be mapped on either FPR or
    // GPR for the same cost.
    const MachineFunction &MF = *MI.getParent()->getParent();
    const TargetSubtargetInfo &STI = MF.getSubtarget();
    const TargetRegisterInfo &TRI = *STI.getRegisterInfo();
    const MachineRegisterInfo &MRI = MF.getRegInfo();

    unsigned Size = getSizeInBits(MI.getOperand(0).getReg(), MRI, TRI);
    if (Size != 32 && Size != 64)
      break;

    // If the instruction has any implicit-defs or uses,
    // do not mess with it.
    if (MI.getNumOperands() != 3)
      break;
    InstructionMappings AltMappings;
    InstructionMapping GPRMapping(/*ID*/ 1, /*Cost*/ 1, /*NumOperands*/ 3);
    InstructionMapping FPRMapping(/*ID*/ 2, /*Cost*/ 1, /*NumOperands*/ 3);
    for (unsigned Idx = 0; Idx != 3; ++Idx) {
      GPRMapping.setOperandMapping(
          Idx, AArch64::ValMappings[AArch64::getRegBankBaseIdx(Size) +
                                    AArch64::FirstGPR]);
      FPRMapping.setOperandMapping(
          Idx, AArch64::ValMappings[AArch64::getRegBankBaseIdx(Size) +
                                    AArch64::FirstFPR]);
    }
    AltMappings.emplace_back(std::move(GPRMapping));
    AltMappings.emplace_back(std::move(FPRMapping));
    return AltMappings;
  }
  default:
    break;
  }
  return RegisterBankInfo::getInstrAlternativeMappings(MI);
}

void AArch64RegisterBankInfo::applyMappingImpl(
    const OperandsMapper &OpdMapper) const {
  switch (OpdMapper.getMI().getOpcode()) {
  case TargetOpcode::G_OR: {
    // Those ID must match getInstrAlternativeMappings.
    assert((OpdMapper.getInstrMapping().getID() == 1 ||
            OpdMapper.getInstrMapping().getID() == 2) &&
           "Don't know how to handle that ID");
    return applyDefaultMapping(OpdMapper);
  }
  default:
    llvm_unreachable("Don't know how to handle that operation");
  }
}

/// Returns whether opcode \p Opc is a pre-isel generic floating-point opcode,
/// having only floating-point operands.
static bool isPreISelGenericFloatingPointOpcode(unsigned Opc) {
  switch (Opc) {
  case TargetOpcode::G_FADD:
  case TargetOpcode::G_FSUB:
  case TargetOpcode::G_FMUL:
  case TargetOpcode::G_FDIV:
  case TargetOpcode::G_FCONSTANT:
  case TargetOpcode::G_FPEXT:
  case TargetOpcode::G_FPTRUNC:
    return true;
  }
  return false;
}

RegisterBankInfo::InstructionMapping
AArch64RegisterBankInfo::getInstrMapping(const MachineInstr &MI) const {
  const unsigned Opc = MI.getOpcode();
  const MachineFunction &MF = *MI.getParent()->getParent();
  const MachineRegisterInfo &MRI = MF.getRegInfo();

  // Try the default logic for non-generic instructions that are either copies
  // or already have some operands assigned to banks.
  if (!isPreISelGenericOpcode(Opc)) {
    RegisterBankInfo::InstructionMapping Mapping = getInstrMappingImpl(MI);
    if (Mapping.isValid())
      return Mapping;
  }

  RegisterBankInfo::InstructionMapping Mapping =
      InstructionMapping{DefaultMappingID, 1, MI.getNumOperands()};

  // Track the size and bank of each register.  We don't do partial mappings.
  SmallVector<unsigned, 4> OpBaseIdx(MI.getNumOperands());
  SmallVector<unsigned, 4> OpFinalIdx(MI.getNumOperands());
  for (unsigned Idx = 0; Idx < MI.getNumOperands(); ++Idx) {
    auto &MO = MI.getOperand(Idx);
    if (!MO.isReg())
      continue;

    LLT Ty = MRI.getType(MO.getReg());
    unsigned RBIdx = AArch64::getRegBankBaseIdx(Ty.getSizeInBits());
    OpBaseIdx[Idx] = RBIdx;

    // As a top-level guess, vectors go in FPRs, scalars and pointers in GPRs.
    // For floating-point instructions, scalars go in FPRs.
    if (Ty.isVector() || isPreISelGenericFloatingPointOpcode(Opc)) {
      assert(RBIdx < (AArch64::LastFPR - AArch64::FirstFPR) + 1 &&
             "Index out of bound");
      OpFinalIdx[Idx] = AArch64::FirstFPR + RBIdx;
    } else {
      assert(RBIdx < (AArch64::LastGPR - AArch64::FirstGPR) + 1 &&
             "Index out of bound");
      OpFinalIdx[Idx] = AArch64::FirstGPR + RBIdx;
    }
  }

  // Some of the floating-point instructions have mixed GPR and FPR operands:
  // fine-tune the computed mapping.
  switch (Opc) {
  case TargetOpcode::G_SITOFP:
  case TargetOpcode::G_UITOFP: {
    OpFinalIdx = {OpBaseIdx[0] + AArch64::FirstFPR,
                  OpBaseIdx[1] + AArch64::FirstGPR};
    break;
  }
  case TargetOpcode::G_FPTOSI:
  case TargetOpcode::G_FPTOUI: {
    OpFinalIdx = {OpBaseIdx[0] + AArch64::FirstGPR,
                  OpBaseIdx[1] + AArch64::FirstFPR};
    break;
  }
  case TargetOpcode::G_FCMP: {
    OpFinalIdx = {OpBaseIdx[0] + AArch64::FirstGPR, /* Predicate */ 0,
                  OpBaseIdx[2] + AArch64::FirstFPR,
                  OpBaseIdx[3] + AArch64::FirstFPR};
    break;
  }
  }

  // Finally construct the computed mapping.
  for (unsigned Idx = 0; Idx < MI.getNumOperands(); ++Idx)
    if (MI.getOperand(Idx).isReg())
      Mapping.setOperandMapping(Idx, AArch64::ValMappings[OpFinalIdx[Idx]]);

  return Mapping;
}
