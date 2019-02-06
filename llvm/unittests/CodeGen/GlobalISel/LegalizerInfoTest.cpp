//===- llvm/unittest/CodeGen/GlobalISel/LegalizerInfoTest.cpp -------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "llvm/CodeGen/GlobalISel/LegalizerInfo.h"
#include "llvm/CodeGen/TargetOpcodes.h"
#include "GISelMITest.h"
#include "gtest/gtest.h"

using namespace llvm;
using namespace LegalizeActions;

// Define a couple of pretty printers to help debugging when things go wrong.
namespace llvm {
std::ostream &
operator<<(std::ostream &OS, const LegalizeAction Act) {
  switch (Act) {
  case Lower: OS << "Lower"; break;
  case Legal: OS << "Legal"; break;
  case NarrowScalar: OS << "NarrowScalar"; break;
  case WidenScalar:  OS << "WidenScalar"; break;
  case FewerElements:  OS << "FewerElements"; break;
  case MoreElements:  OS << "MoreElements"; break;
  case Libcall: OS << "Libcall"; break;
  case Custom: OS << "Custom"; break;
  case Unsupported: OS << "Unsupported"; break;
  case NotFound: OS << "NotFound"; break;
  case UseLegacyRules: OS << "UseLegacyRules"; break;
  }
  return OS;
}

std::ostream &operator<<(std::ostream &OS, const llvm::LegalizeActionStep Ty) {
  OS << "LegalizeActionStep(" << Ty.Action << ", " << Ty.TypeIdx << ", "
     << Ty.NewType << ')';
  return OS;
}
}

namespace {


TEST(LegalizerInfoTest, ScalarRISC) {
  using namespace TargetOpcode;
  LegalizerInfo L;
  // Typical RISCy set of operations based on AArch64.
  for (unsigned Op : {G_ADD, G_SUB}) {
    for (unsigned Size : {32, 64})
      L.setAction({Op, 0, LLT::scalar(Size)}, Legal);
    L.setLegalizeScalarToDifferentSizeStrategy(
        Op, 0, LegalizerInfo::widenToLargerTypesAndNarrowToLargest);
  }

  L.computeTables();

  for (unsigned opcode : {G_ADD, G_SUB}) {
    // Check we infer the correct types and actually do what we're told.
    EXPECT_EQ(L.getAction({opcode, {LLT::scalar(8)}}),
              LegalizeActionStep(WidenScalar, 0, LLT::scalar(32)));
    EXPECT_EQ(L.getAction({opcode, {LLT::scalar(16)}}),
              LegalizeActionStep(WidenScalar, 0, LLT::scalar(32)));
    EXPECT_EQ(L.getAction({opcode, {LLT::scalar(32)}}),
              LegalizeActionStep(Legal, 0, LLT{}));
    EXPECT_EQ(L.getAction({opcode, {LLT::scalar(64)}}),
              LegalizeActionStep(Legal, 0, LLT{}));

    // Make sure the default for over-sized types applies.
    EXPECT_EQ(L.getAction({opcode, {LLT::scalar(128)}}),
              LegalizeActionStep(NarrowScalar, 0, LLT::scalar(64)));
    // Make sure we also handle unusual sizes
    EXPECT_EQ(L.getAction({opcode, {LLT::scalar(1)}}),
              LegalizeActionStep(WidenScalar, 0, LLT::scalar(32)));
    EXPECT_EQ(L.getAction({opcode, {LLT::scalar(31)}}),
              LegalizeActionStep(WidenScalar, 0, LLT::scalar(32)));
    EXPECT_EQ(L.getAction({opcode, {LLT::scalar(33)}}),
              LegalizeActionStep(WidenScalar, 0, LLT::scalar(64)));
    EXPECT_EQ(L.getAction({opcode, {LLT::scalar(63)}}),
              LegalizeActionStep(WidenScalar, 0, LLT::scalar(64)));
    EXPECT_EQ(L.getAction({opcode, {LLT::scalar(65)}}),
              LegalizeActionStep(NarrowScalar, 0, LLT::scalar(64)));
  }
}

TEST(LegalizerInfoTest, VectorRISC) {
  using namespace TargetOpcode;
  LegalizerInfo L;
  // Typical RISCy set of operations based on ARM.
  L.setAction({G_ADD, LLT::vector(8, 8)}, Legal);
  L.setAction({G_ADD, LLT::vector(16, 8)}, Legal);
  L.setAction({G_ADD, LLT::vector(4, 16)}, Legal);
  L.setAction({G_ADD, LLT::vector(8, 16)}, Legal);
  L.setAction({G_ADD, LLT::vector(2, 32)}, Legal);
  L.setAction({G_ADD, LLT::vector(4, 32)}, Legal);

  L.setLegalizeVectorElementToDifferentSizeStrategy(
      G_ADD, 0, LegalizerInfo::widenToLargerTypesUnsupportedOtherwise);

  L.setAction({G_ADD, 0, LLT::scalar(32)}, Legal);

  L.computeTables();

  // Check we infer the correct types and actually do what we're told for some
  // simple cases.
  EXPECT_EQ(L.getAction({G_ADD, {LLT::vector(8, 8)}}),
            LegalizeActionStep(Legal, 0, LLT{}));
  EXPECT_EQ(L.getAction({G_ADD, {LLT::vector(8, 7)}}),
            LegalizeActionStep(WidenScalar, 0, LLT::vector(8, 8)));
  EXPECT_EQ(L.getAction({G_ADD, {LLT::vector(2, 8)}}),
            LegalizeActionStep(MoreElements, 0, LLT::vector(8, 8)));
  EXPECT_EQ(L.getAction({G_ADD, {LLT::vector(8, 32)}}),
            LegalizeActionStep(FewerElements, 0, LLT::vector(4, 32)));
  // Check a few non-power-of-2 sizes:
  EXPECT_EQ(L.getAction({G_ADD, {LLT::vector(3, 3)}}),
            LegalizeActionStep(WidenScalar, 0, LLT::vector(3, 8)));
  EXPECT_EQ(L.getAction({G_ADD, {LLT::vector(3, 8)}}),
            LegalizeActionStep(MoreElements, 0, LLT::vector(8, 8)));
}

TEST(LegalizerInfoTest, MultipleTypes) {
  using namespace TargetOpcode;
  LegalizerInfo L;
  LLT p0 = LLT::pointer(0, 64);
  LLT s64 = LLT::scalar(64);

  // Typical RISCy set of operations based on AArch64.
  L.setAction({G_PTRTOINT, 0, s64}, Legal);
  L.setAction({G_PTRTOINT, 1, p0}, Legal);

  L.setLegalizeScalarToDifferentSizeStrategy(
      G_PTRTOINT, 0, LegalizerInfo::widenToLargerTypesAndNarrowToLargest);

  L.computeTables();

  // Check we infer the correct types and actually do what we're told.
  EXPECT_EQ(L.getAction({G_PTRTOINT, {s64, p0}}),
            LegalizeActionStep(Legal, 0, LLT{}));

  // Make sure we also handle unusual sizes
  EXPECT_EQ(
      L.getAction({G_PTRTOINT, {LLT::scalar(65), s64}}),
      LegalizeActionStep(NarrowScalar, 0, s64));
  EXPECT_EQ(
      L.getAction({G_PTRTOINT, {s64, LLT::pointer(0, 32)}}),
      LegalizeActionStep(Unsupported, 1, LLT::pointer(0, 32)));
}

TEST(LegalizerInfoTest, MultipleSteps) {
  using namespace TargetOpcode;
  LegalizerInfo L;
  LLT s32 = LLT::scalar(32);
  LLT s64 = LLT::scalar(64);

  L.setLegalizeScalarToDifferentSizeStrategy(
      G_UREM, 0, LegalizerInfo::widenToLargerTypesUnsupportedOtherwise);
  L.setAction({G_UREM, 0, s32}, Lower);
  L.setAction({G_UREM, 0, s64}, Lower);

  L.computeTables();

  EXPECT_EQ(L.getAction({G_UREM, {LLT::scalar(16)}}),
            LegalizeActionStep(WidenScalar, 0, LLT::scalar(32)));
  EXPECT_EQ(L.getAction({G_UREM, {LLT::scalar(32)}}),
            LegalizeActionStep(Lower, 0, LLT::scalar(32)));
}

TEST(LegalizerInfoTest, SizeChangeStrategy) {
  using namespace TargetOpcode;
  LegalizerInfo L;
  for (unsigned Size : {1, 8, 16, 32})
    L.setAction({G_UREM, 0, LLT::scalar(Size)}, Legal);

  L.setLegalizeScalarToDifferentSizeStrategy(
      G_UREM, 0, LegalizerInfo::widenToLargerTypesUnsupportedOtherwise);
  L.computeTables();

  // Check we infer the correct types and actually do what we're told.
  for (unsigned Size : {1, 8, 16, 32}) {
    EXPECT_EQ(L.getAction({G_UREM, {LLT::scalar(Size)}}),
              LegalizeActionStep(Legal, 0, LLT{}));
  }
  EXPECT_EQ(L.getAction({G_UREM, {LLT::scalar(2)}}),
            LegalizeActionStep(WidenScalar, 0, LLT::scalar(8)));
  EXPECT_EQ(L.getAction({G_UREM, {LLT::scalar(7)}}),
            LegalizeActionStep(WidenScalar, 0, LLT::scalar(8)));
  EXPECT_EQ(L.getAction({G_UREM, {LLT::scalar(9)}}),
            LegalizeActionStep(WidenScalar, 0, LLT::scalar(16)));
  EXPECT_EQ(L.getAction({G_UREM, {LLT::scalar(17)}}),
            LegalizeActionStep(WidenScalar, 0, LLT::scalar(32)));
  EXPECT_EQ(L.getAction({G_UREM, {LLT::scalar(31)}}),
            LegalizeActionStep(WidenScalar, 0, LLT::scalar(32)));
  EXPECT_EQ(L.getAction({G_UREM, {LLT::scalar(33)}}),
            LegalizeActionStep(Unsupported, 0, LLT::scalar(33)));
}
}

#define EXPECT_ACTION(Action, Index, Type, Query)                              \
  do {                                                                         \
    auto A = LI.getAction(Query);                                              \
    EXPECT_EQ(LegalizeActionStep(Action, Index, Type), A) << A;                \
  } while (0)

TEST(LegalizerInfoTest, RuleSets) {
  using namespace TargetOpcode;

  const LLT s32 = LLT::scalar(32);

  const LLT v2s32 = LLT::vector(2, 32);
  const LLT v3s32 = LLT::vector(3, 32);
  const LLT v4s32 = LLT::vector(4, 32);

  const LLT p0 = LLT::pointer(0, 32);
  const LLT v3p0 = LLT::vector(3, p0);
  const LLT v4p0 = LLT::vector(4, p0);

  {
    LegalizerInfo LI;

    LI.getActionDefinitionsBuilder(G_IMPLICIT_DEF)
      .legalFor({v4s32, v4p0})
      .moreElementsToNextPow2(0);
    LI.computeTables();

    EXPECT_ACTION(Unsupported, 0, LLT(), LegalityQuery(G_IMPLICIT_DEF, {s32}));
    EXPECT_ACTION(Unsupported, 0, LLT(), LegalityQuery(G_IMPLICIT_DEF, {v2s32}));
    EXPECT_ACTION(MoreElements, 0, v4p0, LegalityQuery(G_IMPLICIT_DEF, {v3p0}));
    EXPECT_ACTION(MoreElements, 0, v4s32, LegalityQuery(G_IMPLICIT_DEF, {v3s32}));
  }
}
