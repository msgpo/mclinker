//===- PLT.h --------------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_PROCEDURE_LINKAGE_TABLE_H
#define MCLD_PROCEDURE_LINKAGE_TABLE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <llvm/ADT/ilist.h>

#include <mcld/LD/LDSection.h>
#include <mcld/LD/SectionData.h>
#include <mcld/LD/TargetFragment.h>

namespace mcld
{

class SectionData;
class ResolveInfo;

/** \class PLTEntry
 */
class PLTEntry : public TargetFragment
{
public:
  PLTEntry(size_t pSize, SectionData* pParent);
  virtual ~PLTEntry();

  size_t getEntrySize() const
  { return m_EntrySize; }

  void setContent(unsigned char* pContent)
  { m_pContent = pContent; }

  const unsigned char* getContent() const
  { return m_pContent; }

  //Used by llvm::cast<>.
  static bool classof(const Fragment *O)
  { return true; }

  size_t size() const
  { return m_EntrySize; }

protected:
  size_t m_EntrySize;
  unsigned char* m_pContent;
};

/** \class PLT
 *  \brief Procedure linkage table
 */
class PLT
{
public:
  typedef SectionData::iterator iterator;
  typedef SectionData::const_iterator const_iterator;

public:
  PLT(LDSection& pSection, SectionData& pSectionData);
  virtual ~PLT();

  /// reserveEntry - reseve the number of pNum of empty entries
  /// The empty entris are reserved for layout to adjust the fragment offset.
  virtual void reserveEntry(size_t pNum = 1) = 0;

  /// getPLTEntry - get an empty entry or an exitsted filled entry with pSymbol.
  /// @param pSymbol - the target symbol
  /// @param pExist - ture if the a filled entry with pSymbol existed,
  /// otherwise false.
  virtual PLTEntry* getOrConsumeEntry(const ResolveInfo& pSymbol,
                                      bool& pExist) = 0;

  // ----- observers ----- //
  const LDSection& getSection() const
  { return m_Section; }

  const SectionData& getSectionData() const
  { return m_SectionData; }

  iterator begin()
  { return m_SectionData.begin(); }

  const_iterator begin() const
  { return m_SectionData.begin(); }

  iterator end()
  { return m_SectionData.end(); }

  const_iterator end() const
  { return m_SectionData.end(); }

protected:
  LDSection& m_Section;
  SectionData& m_SectionData;
};

} // namespace of mcld

#endif

