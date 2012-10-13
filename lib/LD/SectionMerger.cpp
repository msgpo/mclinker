//===- SectionMerger.cpp --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <cassert>
#include <cstring>
#include <mcld/Module.h>
#include <mcld/LinkerConfig.h>
#include <mcld/LD/SectionMerger.h>
#include <mcld/LD/SectionMap.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// SectionMerger
//===----------------------------------------------------------------------===//
SectionMerger::SectionMerger(const LinkerConfig& pConfig, Module& pModule)
: m_SectionNameMap(pConfig.scripts().sectionMap()),
  m_Module(pModule),
  m_LDSectionMap()
{
  initOutputSectMap();
}

SectionMerger::~SectionMerger()
{
}

SectionMerger::iterator SectionMerger::find(const std::string& pName)
{
  iterator it;
  for (it = begin(); it != end(); ++it) {
    if (0 == strncmp(pName.c_str(),
                     (*it).inputSubStr.c_str(),
                     (*it).inputSubStr.length()))
      break;
    // wildcard to a user-defined output section.
    else if(0 == strcmp("*", (*it).inputSubStr.c_str()))
      break;
  }
  return it;
}

SectionMerger::const_iterator SectionMerger::find(const std::string& pName) const
{
  const_iterator it;
  for (it = begin(); it != end(); ++it) {
    if (0 == strncmp(pName.c_str(),
                     (*it).inputSubStr.c_str(),
                     (*it).inputSubStr.length()))
      break;
    // wildcard to a user-defined output section.
    else if(0 == strcmp("*", (*it).inputSubStr.c_str()))
      break;
  }
  return it;
}

LDSection* SectionMerger::getOutputSectHdr(const std::string& pName)
{
  LDSection* section;
  iterator it = find(pName);

  // check if we can find a matched LDSection.
  // If not, we need to find it in output context. But this should be rare.
  if (it != end())
    section = (*it).outputSection;
  else
    section = m_Module.getSection(pName);

  assert(NULL != section);
  return section;
}

SectionData* SectionMerger::getOutputSectData(const std::string& pName)
{
  return getOutputSectHdr(pName)->getSectionData();
}

bool SectionMerger::addMapping(const std::string& pName, LDSection* pSection)
{
  iterator it = find(pName);
  if (it != end()) {
    assert(NULL == (*it).outputSection);
    (*it).outputSection = pSection;
    return true;
  }
  // the mapping rule is not in SectionMap, and this is handled in getOutputSectHdr.
  return false;
}

void SectionMerger::initOutputSectMap()
{
  // Based on SectionMap to initialize the map from a input substr to its 
  // associated output LDSection*
  SectionMap::const_iterator it;
  for (it = m_SectionNameMap.begin(); it != m_SectionNameMap.end(); ++it) {
    NameSectPair mapping;
    mapping.inputSubStr = (*it)->inputSubStr;
    mapping.outputSection =  NULL;
    m_LDSectionMap.push_back(mapping);
  }
  assert(m_SectionNameMap.size() == m_LDSectionMap.size());
}
