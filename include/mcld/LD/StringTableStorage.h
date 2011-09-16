/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Nowar Gu <nowar100@gmail.com>                                           *
 ****************************************************************************/
#ifndef LDSTRINGTABLESTORAGE_H
#define LDSTRINGTABLESTORAGE_H
#include <mcld/Support/GCFactory.h>
#include <llvm/ADT/StringRef.h>
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

class StringTableStorage : public GCFactory<char, 4*1024>
{
  typedef GCFactory<char, 4*1024> Factory;

public:
  StringTableStorage()
  : m_Size(0) {}
  ~StringTableStorage() {}

  const char* add(llvm::StringRef pSR)
  {
    char* p = Factory::allocate(pSR.size()+1);
    strcpy(p, pSR.data());
    m_Size += 1;
    return const_cast<const char*>(p);
  }

  size_t size() const { return m_Size; }

private:
  size_t m_Size;
};

}

#endif
