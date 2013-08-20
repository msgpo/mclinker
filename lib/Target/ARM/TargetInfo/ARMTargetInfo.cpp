//===- ARMTargetInfo.cpp --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Support/TargetRegistry.h>
#include <mcld/Support/Target.h>

namespace mcld {

mcld::Target TheARMTarget;
mcld::Target TheThumbTarget;

extern "C" void MCLDInitializeARMLDTargetInfo() {
  // register into mcld::TargetRegistry
  mcld::RegisterTarget X(TheARMTarget, "arm" );
  mcld::RegisterTarget Y(TheThumbTarget, "thumb" );
}

} // namespace of mcld

