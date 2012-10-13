//===- SectionMap.h -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_SECTION_MAP_H
#define MCLD_SECTION_MAP_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <llvm/Support/DataTypes.h>
#include <vector>
#include <string>

namespace mcld
{

/** \class SectionMap
 *  \brief descirbe the mappings of input section's name (or prefix) to
 *         its associated output section's name and offset
 */
class SectionMap
{
private:
  // a mapping in SectionMap is the triple of
  // {input substr, output section's name, output section's offset}
  struct Mapping
  {
  public:
    Mapping(const std::string& pIn, const std::string& pOut)
      : inputSubStr(pIn), outputStr(pOut) { }

  public:
    std::string inputSubStr;
    std::string outputStr;
  };

public:
  typedef std::vector<Mapping*> SectionMappingTy;

  typedef SectionMappingTy::iterator iterator;
  typedef SectionMappingTy::const_iterator const_iterator;

public:
  SectionMap();
  ~SectionMap();

  // get the possible output section name based on the mapping table
  // return NULL if not found
  const std::string& getOutputSectName(const std::string& pInput) const;

  // add a mapping from input substr to output name and offset.
  bool push_back(const std::string& pInput,
                 const std::string& pOutput);

  // find - return the iterator to the mapping
  iterator find(const std::string& pInput);

  // -----  observers  ----- //
  bool empty() const
  { return m_SectMap.empty(); }

  size_t size() const
  { return m_SectMap.size(); }

  size_t capacity () const
  { return m_SectMap.capacity(); }

  // -----  iterators  ----- //
  iterator begin()
  { return m_SectMap.begin(); }

  iterator end()
  { return m_SectMap.end(); }

  const_iterator begin() const
  { return m_SectMap.begin(); }

  const_iterator end() const
  { return m_SectMap.end(); }

  // initStdSectionMap - add common mappings of ELF and other formats
  // to SectionMap
  bool initStdSectionMap();

private:
  SectionMappingTy m_SectMap;
};

} // namespace of mcld

#endif

