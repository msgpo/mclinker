/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef MCLD_INPUT_FACTORY_H
#define MCLD_INPUT_FACTORY_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/Support/GCFactory.h>
#include <mcld/MC/MCLDInput.h>

namespace mcld
{

class AttributeFactory;

/** \class InputFactory
 *  \brief InputFactory controls the production and destruction of
 *  MCLDInput.
 *
 *  All MCLDFiles created by MCLDFileFactory are guaranteed to be destructed
 *  while MCLDFileFactory is destructed.
 *
 *  FIXME: the number of the Inputs should be passed in by Target or any
 *  target specific class.
 *
 *  \see llvm::sys::Path
 */
class InputFactory : public GCFactory<Input,0>
{
public:
  typedef GCFactory<Input, 0> Alloc;

public:
  InputFactory(size_t pNum, AttributeFactory& pAttrFactory);
  ~InputFactory();

  // -----  production  ----- //
  Input* produce(llvm::StringRef pName,
                 const sys::fs::Path& pPath,
                 unsigned int pType = Input::Unknown);

private:
  AttributeFactory &m_AttrFactory;
};

} // namespace of mcld

#endif

