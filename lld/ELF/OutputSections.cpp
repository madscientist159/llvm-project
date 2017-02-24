//===- OutputSections.cpp -------------------------------------------------===//
//
//                             The LLVM Linker
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "OutputSections.h"
#include "Config.h"
#include "LinkerScript.h"
#include "Memory.h"
#include "Strings.h"
#include "SymbolTable.h"
#include "SyntheticSections.h"
#include "Target.h"
#include "Threads.h"
#include "llvm/Support/Dwarf.h"
#include "llvm/Support/MD5.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/SHA1.h"

using namespace llvm;
using namespace llvm::dwarf;
using namespace llvm::object;
using namespace llvm::support::endian;
using namespace llvm::ELF;

using namespace lld;
using namespace lld::elf;

OutputSectionBase::OutputSectionBase(StringRef Name, uint32_t Type,
                                     uint64_t Flags)
    : Name(Name) {
  this->Type = Type;
  this->Flags = Flags;
  this->Addralign = 1;
}

uint32_t OutputSectionBase::getPhdrFlags() const {
  uint32_t Ret = PF_R;
  if (Flags & SHF_WRITE)
    Ret |= PF_W;
  if (Flags & SHF_EXECINSTR)
    Ret |= PF_X;
  return Ret;
}

template <class ELFT>
void OutputSectionBase::writeHeaderTo(typename ELFT::Shdr *Shdr) {
  Shdr->sh_entsize = Entsize;
  Shdr->sh_addralign = Addralign;
  Shdr->sh_type = Type;
  Shdr->sh_offset = Offset;
  Shdr->sh_flags = Flags;
  Shdr->sh_info = Info;
  Shdr->sh_link = Link;
  Shdr->sh_addr = Addr;
  Shdr->sh_size = Size;
  Shdr->sh_name = ShName;
}

template <class ELFT> static uint64_t getEntsize(uint32_t Type) {
  switch (Type) {
  case SHT_RELA:
    return sizeof(typename ELFT::Rela);
  case SHT_REL:
    return sizeof(typename ELFT::Rel);
  case SHT_MIPS_REGINFO:
    return sizeof(Elf_Mips_RegInfo<ELFT>);
  case SHT_MIPS_OPTIONS:
    return sizeof(Elf_Mips_Options<ELFT>) + sizeof(Elf_Mips_RegInfo<ELFT>);
  case SHT_MIPS_ABIFLAGS:
    return sizeof(Elf_Mips_ABIFlags<ELFT>);
  default:
    return 0;
  }
}

template <class ELFT>
OutputSection<ELFT>::OutputSection(StringRef Name, uint32_t Type, uintX_t Flags)
    : OutputSectionBase(Name, Type, Flags) {
  this->Entsize = getEntsize<ELFT>(Type);
}

template <typename ELFT>
static bool compareByFilePosition(InputSection *A, InputSection *B) {
  // Synthetic doesn't have link order dependecy, stable_sort will keep it last
  if (A->kind() == InputSectionBase::Synthetic ||
      B->kind() == InputSectionBase::Synthetic)
    return false;
  auto *LA = cast<InputSection>(A->template getLinkOrderDep<ELFT>());
  auto *LB = cast<InputSection>(B->template getLinkOrderDep<ELFT>());
  OutputSectionBase *AOut = LA->OutSec;
  OutputSectionBase *BOut = LB->OutSec;
  if (AOut != BOut)
    return AOut->SectionIndex < BOut->SectionIndex;
  return LA->OutSecOff < LB->OutSecOff;
}

template <class ELFT> void OutputSection<ELFT>::finalize() {
  if ((this->Flags & SHF_LINK_ORDER) && !this->Sections.empty()) {
    std::sort(Sections.begin(), Sections.end(), compareByFilePosition<ELFT>);
    Size = 0;
    assignOffsets();

    // We must preserve the link order dependency of sections with the
    // SHF_LINK_ORDER flag. The dependency is indicated by the sh_link field. We
    // need to translate the InputSection sh_link to the OutputSection sh_link,
    // all InputSections in the OutputSection have the same dependency.
    if (auto *D = this->Sections.front()->template getLinkOrderDep<ELFT>())
      this->Link = D->OutSec->SectionIndex;
  }

  uint32_t Type = this->Type;
  if (!Config->copyRelocs() || (Type != SHT_RELA && Type != SHT_REL))
    return;

  InputSection *First = Sections[0];
  if (isa<SyntheticSection<ELFT>>(First))
    return;

  this->Link = In<ELFT>::SymTab->OutSec->SectionIndex;
  // sh_info for SHT_REL[A] sections should contain the section header index of
  // the section to which the relocation applies.
  InputSectionBase *S = First->getRelocatedSection<ELFT>();
  this->Info = S->OutSec->SectionIndex;
}

template <class ELFT>
void OutputSection<ELFT>::addSection(InputSectionBase *C) {
  assert(C->Live);
  auto *S = cast<InputSection>(C);
  Sections.push_back(S);
  S->OutSec = this;
  this->updateAlignment(S->Alignment);
  // Keep sh_entsize value of the input section to be able to perform merging
  // later during a final linking using the generated relocatable object.
  if (Config->Relocatable && (S->Flags & SHF_MERGE))
    this->Entsize = S->Entsize;
}

template <class ELFT>
void OutputSection<ELFT>::forEachInputSection(
    std::function<void(InputSectionBase *)> F) {
  for (InputSection *S : Sections)
    F(S);
}

// This function is called after we sort input sections
// and scan relocations to setup sections' offsets.
template <class ELFT> void OutputSection<ELFT>::assignOffsets() {
  uintX_t Off = this->Size;
  for (InputSection *S : Sections) {
    Off = alignTo(Off, S->Alignment);
    S->OutSecOff = Off;
    Off += S->template getSize<ELFT>();
  }
  this->Size = Off;
}

template <class ELFT>
void OutputSection<ELFT>::sort(std::function<int(InputSectionBase *S)> Order) {
  typedef std::pair<unsigned, InputSection *> Pair;
  auto Comp = [](const Pair &A, const Pair &B) { return A.first < B.first; };

  std::vector<Pair> V;
  for (InputSection *S : Sections)
    V.push_back({Order(S), S});
  std::stable_sort(V.begin(), V.end(), Comp);
  Sections.clear();
  for (Pair &P : V)
    Sections.push_back(P.second);
}

// Sorts input sections by section name suffixes, so that .foo.N comes
// before .foo.M if N < M. Used to sort .{init,fini}_array.N sections.
// We want to keep the original order if the priorities are the same
// because the compiler keeps the original initialization order in a
// translation unit and we need to respect that.
// For more detail, read the section of the GCC's manual about init_priority.
template <class ELFT> void OutputSection<ELFT>::sortInitFini() {
  // Sort sections by priority.
  sort([](InputSectionBase *S) { return getPriority(S->Name); });
}

// Returns true if S matches /Filename.?\.o$/.
static bool isCrtBeginEnd(StringRef S, StringRef Filename) {
  if (!S.endswith(".o"))
    return false;
  S = S.drop_back(2);
  if (S.endswith(Filename))
    return true;
  return !S.empty() && S.drop_back().endswith(Filename);
}

static bool isCrtbegin(StringRef S) { return isCrtBeginEnd(S, "crtbegin"); }
static bool isCrtend(StringRef S) { return isCrtBeginEnd(S, "crtend"); }

// .ctors and .dtors are sorted by this priority from highest to lowest.
//
//  1. The section was contained in crtbegin (crtbegin contains
//     some sentinel value in its .ctors and .dtors so that the runtime
//     can find the beginning of the sections.)
//
//  2. The section has an optional priority value in the form of ".ctors.N"
//     or ".dtors.N" where N is a number. Unlike .{init,fini}_array,
//     they are compared as string rather than number.
//
//  3. The section is just ".ctors" or ".dtors".
//
//  4. The section was contained in crtend, which contains an end marker.
//
// In an ideal world, we don't need this function because .init_array and
// .ctors are duplicate features (and .init_array is newer.) However, there
// are too many real-world use cases of .ctors, so we had no choice to
// support that with this rather ad-hoc semantics.
template <class ELFT>
static bool compCtors(const InputSection *A, const InputSection *B) {
  bool BeginA = isCrtbegin(A->template getFile<ELFT>()->getName());
  bool BeginB = isCrtbegin(B->template getFile<ELFT>()->getName());
  if (BeginA != BeginB)
    return BeginA;
  bool EndA = isCrtend(A->template getFile<ELFT>()->getName());
  bool EndB = isCrtend(B->template getFile<ELFT>()->getName());
  if (EndA != EndB)
    return EndB;
  StringRef X = A->Name;
  StringRef Y = B->Name;
  assert(X.startswith(".ctors") || X.startswith(".dtors"));
  assert(Y.startswith(".ctors") || Y.startswith(".dtors"));
  X = X.substr(6);
  Y = Y.substr(6);
  if (X.empty() && Y.empty())
    return false;
  return X < Y;
}

// Sorts input sections by the special rules for .ctors and .dtors.
// Unfortunately, the rules are different from the one for .{init,fini}_array.
// Read the comment above.
template <class ELFT> void OutputSection<ELFT>::sortCtorsDtors() {
  std::stable_sort(Sections.begin(), Sections.end(), compCtors<ELFT>);
}

// Fill [Buf, Buf + Size) with Filler. Filler is written in big
// endian order. This is used for linker script "=fillexp" command.
void fill(uint8_t *Buf, size_t Size, uint32_t Filler) {
  uint8_t V[4];
  write32be(V, Filler);
  size_t I = 0;
  for (; I + 4 < Size; I += 4)
    memcpy(Buf + I, V, 4);
  memcpy(Buf + I, V, Size - I);
}

template <class ELFT> void OutputSection<ELFT>::writeTo(uint8_t *Buf) {
  Loc = Buf;
  if (uint32_t Filler = Script<ELFT>::X->getFiller(this->Name))
    fill(Buf, this->Size, Filler);

  auto Fn = [=](InputSection *IS) { IS->writeTo<ELFT>(Buf); };
  forEach(Sections.begin(), Sections.end(), Fn);

  // Linker scripts may have BYTE()-family commands with which you
  // can write arbitrary bytes to the output. Process them if any.
  Script<ELFT>::X->writeDataBytes(this->Name, Buf);
}

template <class ELFT>
static typename ELFT::uint getOutFlags(InputSectionBase *S) {
  return S->Flags & ~SHF_GROUP & ~SHF_COMPRESSED;
}

template <class ELFT>
static SectionKey createKey(InputSectionBase *C, StringRef OutsecName) {
  //  The ELF spec just says
  // ----------------------------------------------------------------
  // In the first phase, input sections that match in name, type and
  // attribute flags should be concatenated into single sections.
  // ----------------------------------------------------------------
  //
  // However, it is clear that at least some flags have to be ignored for
  // section merging. At the very least SHF_GROUP and SHF_COMPRESSED have to be
  // ignored. We should not have two output .text sections just because one was
  // in a group and another was not for example.
  //
  // It also seems that that wording was a late addition and didn't get the
  // necessary scrutiny.
  //
  // Merging sections with different flags is expected by some users. One
  // reason is that if one file has
  //
  // int *const bar __attribute__((section(".foo"))) = (int *)0;
  //
  // gcc with -fPIC will produce a read only .foo section. But if another
  // file has
  //
  // int zed;
  // int *const bar __attribute__((section(".foo"))) = (int *)&zed;
  //
  // gcc with -fPIC will produce a read write section.
  //
  // Last but not least, when using linker script the merge rules are forced by
  // the script. Unfortunately, linker scripts are name based. This means that
  // expressions like *(.foo*) can refer to multiple input sections with
  // different flags. We cannot put them in different output sections or we
  // would produce wrong results for
  //
  // start = .; *(.foo.*) end = .; *(.bar)
  //
  // and a mapping of .foo1 and .bar1 to one section and .foo2 and .bar2 to
  // another. The problem is that there is no way to layout those output
  // sections such that the .foo sections are the only thing between the start
  // and end symbols.
  //
  // Given the above issues, we instead merge sections by name and error on
  // incompatible types and flags.

  typedef typename ELFT::uint uintX_t;

  uintX_t Alignment = 0;
  uintX_t Flags = 0;
  if (Config->Relocatable && (C->Flags & SHF_MERGE)) {
    Alignment = std::max<uintX_t>(C->Alignment, C->Entsize);
    Flags = C->Flags & (SHF_MERGE | SHF_STRINGS);
  }

  return SectionKey{OutsecName, Flags, Alignment};
}

template <class ELFT>
OutputSectionFactory<ELFT>::OutputSectionFactory(
    std::vector<OutputSectionBase *> &OutputSections)
    : OutputSections(OutputSections) {}

static uint64_t getIncompatibleFlags(uint64_t Flags) {
  return Flags & (SHF_ALLOC | SHF_TLS);
}

// We allow sections of types listed below to merged into a
// single progbits section. This is typically done by linker
// scripts. Merging nobits and progbits will force disk space
// to be allocated for nobits sections. Other ones don't require
// any special treatment on top of progbits, so there doesn't
// seem to be a harm in merging them.
static bool canMergeToProgbits(unsigned Type) {
  return Type == SHT_NOBITS || Type == SHT_PROGBITS || Type == SHT_INIT_ARRAY ||
         Type == SHT_PREINIT_ARRAY || Type == SHT_FINI_ARRAY ||
         Type == SHT_NOTE;
}

template <class ELFT> static void reportDiscarded(InputSectionBase *IS) {
  if (!Config->PrintGcSections)
    return;
  message("removing unused section from '" + IS->Name + "' in file '" +
          IS->getFile<ELFT>()->getName());
}

template <class ELFT>
void OutputSectionFactory<ELFT>::addInputSec(InputSectionBase *IS,
                                             StringRef OutsecName) {
  if (!IS->Live) {
    reportDiscarded<ELFT>(IS);
    return;
  }

  SectionKey Key = createKey<ELFT>(IS, OutsecName);
  uintX_t Flags = getOutFlags<ELFT>(IS);
  OutputSectionBase *&Sec = Map[Key];
  if (Sec) {
    if (getIncompatibleFlags(Sec->Flags) != getIncompatibleFlags(IS->Flags))
      error("Section has flags incompatible with others with the same name " +
            toString(IS));
    if (Sec->Type != IS->Type) {
      if (canMergeToProgbits(Sec->Type) && canMergeToProgbits(IS->Type))
        Sec->Type = SHT_PROGBITS;
      else
        error("Section has different type from others with the same name " +
              toString(IS));
    }
    Sec->Flags |= Flags;
  } else {
    uint32_t Type = IS->Type;
    if (IS->kind() == InputSectionBase::EHFrame) {
      In<ELFT>::EhFrame->addSection(IS);
      return;
    }
    Sec = make<OutputSection<ELFT>>(Key.Name, Type, Flags);
    OutputSections.push_back(Sec);
  }

  Sec->addSection(IS);
}

template <class ELFT> OutputSectionFactory<ELFT>::~OutputSectionFactory() {}

SectionKey DenseMapInfo<SectionKey>::getEmptyKey() {
  return SectionKey{DenseMapInfo<StringRef>::getEmptyKey(), 0, 0};
}

SectionKey DenseMapInfo<SectionKey>::getTombstoneKey() {
  return SectionKey{DenseMapInfo<StringRef>::getTombstoneKey(), 0, 0};
}

unsigned DenseMapInfo<SectionKey>::getHashValue(const SectionKey &Val) {
  return hash_combine(Val.Name, Val.Flags, Val.Alignment);
}

bool DenseMapInfo<SectionKey>::isEqual(const SectionKey &LHS,
                                       const SectionKey &RHS) {
  return DenseMapInfo<StringRef>::isEqual(LHS.Name, RHS.Name) &&
         LHS.Flags == RHS.Flags && LHS.Alignment == RHS.Alignment;
}

namespace lld {
namespace elf {

template void OutputSectionBase::writeHeaderTo<ELF32LE>(ELF32LE::Shdr *Shdr);
template void OutputSectionBase::writeHeaderTo<ELF32BE>(ELF32BE::Shdr *Shdr);
template void OutputSectionBase::writeHeaderTo<ELF64LE>(ELF64LE::Shdr *Shdr);
template void OutputSectionBase::writeHeaderTo<ELF64BE>(ELF64BE::Shdr *Shdr);

template class OutputSection<ELF32LE>;
template class OutputSection<ELF32BE>;
template class OutputSection<ELF64LE>;
template class OutputSection<ELF64BE>;

template class OutputSectionFactory<ELF32LE>;
template class OutputSectionFactory<ELF32BE>;
template class OutputSectionFactory<ELF64LE>;
template class OutputSectionFactory<ELF64BE>;
}
}
