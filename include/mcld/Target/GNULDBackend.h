//===- GNULDBackend.h -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_TARGET_GNU_LDBACKEND_H
#define MCLD_TARGET_GNU_LDBACKEND_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <llvm/Support/ELF.h>
#include <mcld/ADT/HashTable.h>
#include <mcld/ADT/HashEntry.h>
#include <mcld/LD/ELFDynObjReader.h>
#include <mcld/LD/ELFDynObjWriter.h>
#include <mcld/LD/ELFObjectReader.h>
#include <mcld/LD/ELFObjectWriter.h>
#include <mcld/LD/ELFExecWriter.h>
#include <mcld/LD/ELFDynObjFileFormat.h>
#include <mcld/LD/ELFExecFileFormat.h>
#include <mcld/LD/ELFSegment.h>
#include <mcld/LD/GNUArchiveReader.h>
#include <mcld/Support/GCFactory.h>
#include <mcld/Target/ELFDynamic.h>
#include <mcld/Target/TargetLDBackend.h>
#include <mcld/LD/ELFSegmentFactory.h>

namespace mcld
{

struct SymCompare
{
  bool operator()(const LDSymbol* X, const LDSymbol* Y) const
  { return (X==Y); }
};

struct PtrHash
{
  size_t operator()(const LDSymbol* pKey) const
  {
    return (unsigned((uintptr_t)pKey) >> 4) ^
           (unsigned((uintptr_t)pKey) >> 9);
  }
};

class MCLDInfo;
class Layout;
class SymbolCategory;

/** \class GNULDBackend
 *  \brief GNULDBackend provides a common interface for all GNU Unix-OS
 *  LDBackend.
 */
class GNULDBackend : public TargetLDBackend
{
protected:
  // Based on Kind in LDFileFormat to define basic section orders for ELF, and
  // refer gold linker to add more enumerations to handle Regular and BSS kind
  enum SectionOrder {
    SHO_INTERP = 1,          // .interp
    SHO_RO_NOTE,             // .note.ABI-tag, .note.gnu.build-id
    SHO_NAMEPOOL,            // *.hash, .dynsym, .dynstr
    SHO_RELOCATION,          // .rel.*, .rela.*
    SHO_REL_PLT,             // .rel.plt should come after other .rel.*
    SHO_INIT,                // .init
    SHO_PLT,                 // .plt
    SHO_TEXT,                // .text
    SHO_FINI,                // .fini
    SHO_RO,                  // .rodata
    SHO_EHFRAME,             // .eh_frame_hdr, .eh_frame
    SHO_TLS_DATA,            // .tdata
    SHO_TLS_BSS,             // .tbss
    SHO_RELRO_LOCAL,         // .data.rel.ro.local
    SHO_RELRO,               // .data.rel.ro,
    SHO_RELRO_LAST,          // for x86 to adjust .got if needed
    SHO_NON_RELRO_FIRST,     // for x86 to adjust .got.plt if needed
    SHO_DATA,                // .data
    SHO_LARGE_DATA,          // .ldata
    SHO_RW_NOTE,             //
    SHO_SMALL_DATA,          // .sdata
    SHO_SMALL_BSS,           // .sbss
    SHO_BSS,                 // .bss
    SHO_LARGE_BSS,           // .lbss
    SHO_UNDEFINED = ~(0U)    // default order
  };

protected:
  GNULDBackend();

public:
  virtual ~GNULDBackend();

  bool initArchiveReader(MCLinker& pLinker, MCLDInfo& pInfo);
  bool initObjectReader(MCLinker& pLinker);
  bool initDynObjReader(MCLinker& pLinker);
  bool initObjectWriter(MCLinker& pLinker);
  bool initDynObjWriter(MCLinker& pLinker);
  bool initExecWriter(MCLinker& pLinker);

  bool initExecSections(MCLinker& pMCLinker);
  bool initDynObjSections(MCLinker& pMCLinker);

  bool initStandardSymbols(MCLinker& pLinker);

  GNUArchiveReader *getArchiveReader();
  GNUArchiveReader *getArchiveReader() const;

  ELFObjectReader *getObjectReader();
  ELFObjectReader *getObjectReader() const;

  ELFDynObjReader *getDynObjReader();
  ELFDynObjReader *getDynObjReader() const;

  ELFObjectWriter *getObjectWriter();
  ELFObjectWriter *getObjectWriter() const;

  ELFDynObjWriter *getDynObjWriter();
  ELFDynObjWriter *getDynObjWriter() const;

  ELFExecWriter *getExecWriter();
  ELFExecWriter *getExecWriter() const;

  ELFDynObjFileFormat* getDynObjFileFormat();
  ELFDynObjFileFormat* getDynObjFileFormat() const;

  ELFExecFileFormat* getExecFileFormat();
  ELFExecFileFormat* getExecFileFormat() const;

  size_t sectionStartOffset() const;

  /// The return value of machine() it the same as e_machine in the ELF header*/
  virtual uint32_t machine() const = 0;

  /// ELFVersion - the value of e_ident[EI_VERSION]
  virtual uint8_t ELFVersion() const
  { return llvm::ELF::EV_CURRENT; }

  /// OSABI - the value of e_ident[EI_OSABI]
  virtual uint8_t OSABI() const = 0;

  /// ABIVersion - the value of e_ident[EI_ABIVRESION]
  virtual uint8_t ABIVersion() const = 0;

  /// flags - the value of ElfXX_Ehdr::e_flags
  virtual uint64_t flags() const = 0;

  /// entry - the symbol name of the entry point
  virtual const char* entry() const
  { return "_start"; }

  /// dyld - the name of the default dynamic linker
  /// target may override this function if needed.
  /// @ref gnu ld, bfd/elf32-i386.c:521
  virtual const char* dyld() const
  { return "/usr/lib/libc.so.1"; }

  /// sizeNamePools - compute the size of regular name pools
  /// In ELF executable files, regular name pools are .symtab, .strtab.,
  /// .dynsym, .dynstr, and .hash
  virtual void sizeNamePools(const Output& pOutput,
                             const SymbolCategory& pSymbols,
                             const MCLDInfo& pLDInfo);

  /// emitSectionData - emit target-dependent section data
  virtual uint64_t emitSectionData(const Output& pOutput,
                                   const LDSection& pSection,
                                   const MCLDInfo& pInfo,
                                   MemoryRegion& pRegion) const = 0;

  /// emitRegNamePools - emit regular name pools - .symtab, .strtab
  virtual void emitRegNamePools(Output& pOutput,
                                SymbolCategory& pSymbols,
                                const Layout& pLayout,
                                const MCLDInfo& pLDInfo);

  /// emitNamePools - emit dynamic name pools - .dyntab, .dynstr, .hash
  virtual void emitDynNamePools(Output& pOutput,
                                SymbolCategory& pSymbols,
                                const Layout& pLayout,
                                const MCLDInfo& pLDInfo);

  /// sizeInterp - compute the size of program interpreter's name
  /// In ELF executables, this is the length of dynamic linker's path name
  virtual void sizeInterp(const Output& pOutput, const MCLDInfo& pLDInfo);

  /// emitInterp - emit the .interp
  virtual void emitInterp(Output& pOutput, const MCLDInfo& pLDInfo);

  /// getSectionOrder - compute the layout order of the section
  /// Layout calls this function to get the default order of the pSectHdr.
  /// If the pSectHdr.type() is LDFileFormat::Target, then getSectionOrder()
  /// will call getTargetSectionOrder().
  ///
  /// If targets favors certain order for general sections, please override
  /// this function.
  ///
  /// @see getTargetSectionOrder
  virtual unsigned int getSectionOrder(const Output& pOutput,
                                       const LDSection& pSectHdr,
                                       const MCLDInfo& pInfo) const;

  /// getTargetSectionOrder - compute the layout order of target section
  /// If the target favors certain order for the given gSectHdr, please
  /// override this function.
  ///
  /// By default, this function returns the maximun order, and pSectHdr
  /// will be the last section to be laid out.
  virtual unsigned int
  getTargetSectionOrder(const Output& pOutput,
                        const LDSection& pSectHdr,
                        const MCLDInfo& pInfo) const
  { return (unsigned int)-1; }

  /// numOfSegments - return the number of segments
  /// if the target favors other ways to emit program header, please override
  /// this function
  virtual unsigned int numOfSegments() const
  { return m_ELFSegmentTable.size(); }

  /// commonPageSize - the common page size of the target machine, and we set it
  /// to 4K here. If target favors the different size, please override this
  /// function
  virtual uint64_t commonPageSize(const MCLDInfo& pInfo) const;

  /// abiPageSize - the abi page size of the target machine, and we set it to 4K
  /// here. If target favors the different size, please override this function
  virtual uint64_t abiPageSize(const MCLDInfo& pInfo) const;

  /// getSymbolIdx - get the symbol index of ouput symbol table
  size_t getSymbolIdx(LDSymbol* pSymbol) const;

  /// isDefaultExecStack - target should specify whether the stack is default
  /// executable. If target favors another choice, please override this function
  virtual bool isDefaultExecStack() const
  { return true; }

  /// allocateCommonSymbols - allocate common symbols in the corresponding
  /// sections.
  /// Different concrete target backend may overlap this function.
  virtual bool allocateCommonSymbols(const MCLDInfo& pLDInfo, MCLinker& pLinker) const;
private:
  /// createProgramHdrs - base on output sections to create the program headers
  void createProgramHdrs(Output& pOutput,
                         const MCLDInfo& pInfo);

  /// writeELF32ProgramHdrs - write out the ELF32 program headers
  void writeELF32ProgramHdrs(Output& pOutput);

  /// writeELF64ProgramHdrs - write out the ELF64 program headers
  void writeELF64ProgramHdrs(Output& pOutput);

  /// getSegmentFlag - give a section flag and return the corresponding segment
  /// flag
  inline uint32_t getSegmentFlag(const uint32_t pSectionFlag)
  {
    uint32_t flag = llvm::ELF::PF_R;
    if (0 != (pSectionFlag & llvm::ELF::SHF_WRITE))
      flag |= llvm::ELF::PF_W;
    if (0 != (pSectionFlag & llvm::ELF::SHF_EXECINSTR))
      flag |= llvm::ELF::PF_X;
    return flag;
  }

  /// createGNUStackInfo - create an output GNU stack section or segment if needed
  void createGNUStackInfo(const Output& pOutput,
                          const MCLDInfo& pInfo,
                          MCLinker& pLinker);

  /// preLayout - Backend can do any needed modification before layout
  void preLayout(const Output& pOutput,
                 const MCLDInfo& pInfo,
                 MCLinker& pLinker);

  /// postLayout -Backend can do any needed modification after layout
  void postLayout(const Output& pOutput,
                 const MCLDInfo& pInfo,
                 MCLinker& pLinker);

protected:
  uint64_t getSymbolSize(const LDSymbol& pSymbol) const;

  uint64_t getSymbolInfo(const LDSymbol& pSymbol) const;

  uint64_t getSymbolValue(const LDSymbol& pSymbol) const;

  uint64_t getSymbolShndx(const LDSymbol& pSymbol, const Layout& pLayout) const;

private:
  /// preLayout - Backend can do any needed modification before layout
  virtual void doPreLayout(const Output& pOutput,
                         const MCLDInfo& pInfo,
                         MCLinker& pLinker) = 0;

  /// postLayout -Backend can do any needed modification after layout
  virtual void doPostLayout(const Output& pOutput,
                          const MCLDInfo& pInfo,
                          MCLinker& pLinker) = 0;

  /// dynamic - the dynamic section of the target machine.
  virtual ELFDynamic& dynamic() = 0;

  /// dynamic - the dynamic section of the target machine.
  virtual const ELFDynamic& dynamic() const = 0;

public:
  /// isSymbolPreemtible - whether the symbol can be preemted by other
  /// link unit
  /// @ref Google gold linker, symtab.h:551
  bool isSymbolPreemptible(const ResolveInfo& pSym,
                           const MCLDInfo& pLDInfo,
                           const Output& pOutput) const;

protected:
  /// isOutputPIC - return whether the output is position-independent
  bool isOutputPIC(const Output& pOutput, const MCLDInfo& pInfo) const;

  /// isStaticLink - return whether we're doing static link
  bool isStaticLink(const Output& pOutput, const MCLDInfo& pInfo) const;

  /// symbolNeedsPLT - return whether the symbol needs a PLT entry
  /// @ref Google gold linker, symtab.h:596
  bool symbolNeedsPLT(const ResolveInfo& pSym,
                      const MCLDInfo& pLDInfo,
                      const Output& pOutput) const;

  /// symbolNeedsDynRel - return whether the symbol needs a dynamic relocation
  /// @ref Google gold linker, symtab.h:645
  bool symbolNeedsDynRel(const ResolveInfo& pSym,
                         bool pSymHasPLT,
                         const MCLDInfo& pLDInfo,
                         const Output& pOutput,
                         bool isAbsReloc) const;

  /// symbolNeedsCopyReloc - return whether the symbol needs a copy relocation
  bool symbolNeedsCopyReloc(const Layout& pLayout,
                            const Relocation& pReloc,
                            const ResolveInfo& pSym,
                            const MCLDInfo& pLDInfo,
                            const Output& pOutput) const;

protected:
  // ----- readers and writers ----- //
  GNUArchiveReader* m_pArchiveReader;
  ELFObjectReader* m_pObjectReader;
  ELFDynObjReader* m_pDynObjReader;
  ELFObjectWriter* m_pObjectWriter;
  ELFDynObjWriter* m_pDynObjWriter;
  ELFExecWriter*   m_pExecWriter;

  // -----  file formats  ----- //
  ELFDynObjFileFormat* m_pDynObjFileFormat;
  ELFExecFileFormat* m_pExecFileFormat;

  // -----  ELF segment factory  ----- //
  ELFSegmentFactory m_ELFSegmentTable;

  // -----  ELF special sections  ----- //

protected:
  /// getHashBucketCount - calculate hash bucket count.
  /// @ref Google gold linker, dynobj.cc:791
  static unsigned getHashBucketCount(unsigned pNumOfSymbols, bool pIsGNUStyle);

  /// isDynamicSymbol
  /// @ref Google gold linker: symtab.cc:311
  static bool isDynamicSymbol(const LDSymbol& pSymbol, const Output& pOutput);

protected:
  typedef HashEntry<LDSymbol*, size_t, SymCompare> HashEntryType;
  typedef HashTable<HashEntryType, PtrHash, EntryFactory<HashEntryType> > HashTableType;

  /// m_pSymIndexMap - Map the LDSymbol to its index in the output symbol table
  HashTableType* m_pSymIndexMap;
};

} // namespace of mcld

#endif

