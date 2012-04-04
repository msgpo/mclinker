//===- StaticResolver.h ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_STATIC_SYMBOL_RESOLVER_H
#define MCLD_STATIC_SYMBOL_RESOLVER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <string>
#include <mcld/LD/Resolver.h>
#include <mcld/LD/ResolveInfo.h>

namespace mcld
{

class NamePool;

/** \class StaticResolver
 */
class StaticResolver : public Resolver
{
public:
  /** \enum LinkAction
   *  LinkAction follows BFD:linker.c (binary file descriptor).
   *  List all actions to take in the state table
   */
  enum LinkAction
  {
    FAIL,         /* abort.  */
    NOACT,        /* no action.  */
    UND,          /* override by symbol undefined symbol.  */
    WEAK,         /* override by symbol weak undefined.  */
    DEF,          /* override by symbol defined.  */
    DEFW,         /* override by symbol weak defined.  */
    DEFD,         /* override by symbol dynamic defined.  */
    DEFWD,        /* override by symbol dynamic weak defined.  */
    MDEFD,        /* mark symbol dynamic defined.  */
    MDEFWD,       /* mark symbol dynamic weak defined.  */
    DUND,         /* override dynamic defined symbol by undefined one.  */
    DUNDW,        /* oevrride dynamic defined symbol by weak undefined one.  */
    COM,          /* override by symbol common.  */
    CREF,         /* Possibly warn about common reference to defined symbol.  */
    CDEF,         /* redefine existing common symbol.  */
    BIG,          /* override by symbol common using largest size.  */
    MBIG,         /* mark common symbol by larger size.  */
    IND,          /* override by indirect symbol.  */
    CIND,         /* mark indirect symbol from existing common symbol.  */
    MDEF,         /* multiple definition error.  */
    MIND,         /* multiple indirect symbols.  */
    REFC          /* Mark indirect symbol referenced and then CYCLE.  */
  };

private:
  // These are the values generated by the bit codes.
  /** Encoding:
   *  D -> define
   *  U -> undefine
   *  d -> dynamic
   *  w -> weak
   *  C -> common
   *  I -> indirect
   */
  enum
  {
    U    = ResolveInfo::global_flag | ResolveInfo::regular_flag | ResolveInfo::undefine_flag,
    w_U  = ResolveInfo::weak_flag   | ResolveInfo::regular_flag | ResolveInfo::undefine_flag,
    d_U  = ResolveInfo::global_flag | ResolveInfo::dynamic_flag | ResolveInfo::undefine_flag,
    wd_U = ResolveInfo::weak_flag   | ResolveInfo::dynamic_flag | ResolveInfo::undefine_flag,
    D    = ResolveInfo::global_flag | ResolveInfo::regular_flag | ResolveInfo::define_flag,
    w_D  = ResolveInfo::weak_flag   | ResolveInfo::regular_flag | ResolveInfo::define_flag,
    d_D  = ResolveInfo::global_flag | ResolveInfo::dynamic_flag | ResolveInfo::define_flag,
    wd_D = ResolveInfo::weak_flag   | ResolveInfo::dynamic_flag | ResolveInfo::define_flag,
    C    = ResolveInfo::global_flag | ResolveInfo::regular_flag | ResolveInfo::common_flag, 
    w_C  = ResolveInfo::weak_flag   | ResolveInfo::regular_flag | ResolveInfo::common_flag,
    d_C  = ResolveInfo::global_flag | ResolveInfo::dynamic_flag | ResolveInfo::common_flag,
    wd_C = ResolveInfo::weak_flag   | ResolveInfo::dynamic_flag | ResolveInfo::common_flag,
    I    = ResolveInfo::global_flag | ResolveInfo::regular_flag | ResolveInfo::indirect_flag,
    w_I  = ResolveInfo::weak_flag   | ResolveInfo::regular_flag | ResolveInfo::indirect_flag,
    d_I  = ResolveInfo::global_flag | ResolveInfo::dynamic_flag | ResolveInfo::indirect_flag,
    wd_I = ResolveInfo::weak_flag   | ResolveInfo::dynamic_flag | ResolveInfo::indirect_flag
  };

  enum ORDINATE
  {
    U_ORD,
    w_U_ORD,
    d_U_ORD,
    wd_U_ORD,
    D_ORD,
    w_D_ORD,
    d_D_ORD,
    wd_D_ORD,
    C_ORD,
    w_C_ORD,
    Cs_ORD,
    Is_ORD,
    LAST_ORD
  };

public:
  StaticResolver();

  StaticResolver(const StaticResolver& pCopy);

  virtual ~StaticResolver();

  /// shouldOverride - Can resolver override the symbol pOld by the symbol pNew?
  /// @return the action should be taken.
  /// @param pOld the symbol which may be overridden.
  /// @param pNew the symbol which is used to replace pOld
  virtual unsigned int resolve(ResolveInfo & __restrict__ pOld,
                               const ResolveInfo & __restrict__ pNew,
                               bool &pOverride);

  StaticResolver* doClone() const {
    return new StaticResolver(*this);
  }

private:
  inline unsigned int getOrdinate(const ResolveInfo& pInfo) const {
    if (pInfo.isAbsolute() && pInfo.isDyn())
      return d_D_ORD;
    if (pInfo.isAbsolute())
      return D_ORD;
    if (pInfo.isCommon() && pInfo.isDyn())
      return Cs_ORD;
    if (pInfo.isCommon() && pInfo.isDefine())
      return C_ORD;
    if (pInfo.isCommon() && pInfo.isWeak())
      return w_C_ORD;
    if (pInfo.isIndirect())
      return Is_ORD;
    return pInfo.info();
  }
};

} // namespace of mcld

#endif

