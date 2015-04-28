# Conditions:
#   a) Linking a shared library.
#   b) The first relocation modifies a regular .text section.
#   c) The second relocation modifies a .pdr section without SHF_ALLOC flag.
# Check:
#   a) There should be no PLT entries.
#   b) Linker creates a single R_MIPS_REL32 relocation.
#
# RUN: yaml2obj -format=elf %s > %t.o
# RUN: %MCLinker -mtriple=mipsel-linux-gnu -shared -o %t.so %t.o
# RUN: llvm-objdump -s %t.so | FileCheck -check-prefix=RAW %s
# RUN: llvm-readobj -dt -r -s %t.so | FileCheck -check-prefix=SYM %s

# RAW:      Contents of section .text:
# RAW-NEXT:  01a0 00000000 a0010000

# SYM:     Sections [
# SYM:       Section {
# SYM-NOT:     Name: .plt

# SYM:      Relocations [
# SYM-NEXT:   Section (4) .rel.dyn {
# SYM-NEXT:     0x[[ADDR1:[0-9A-Z]+]] R_MIPS_REL32 - 0x0
# SYM-NEXT:     0x[[ADDR2:[0-9A-Z]+]] R_MIPS_REL32 T0 0x0
# SYM-NEXT:   }
# SYM-NEXT: ]

# SYM:      Symbol {
# SYM:        Name: T0@
# SYM-NEXT:   Value: 0x[[ADDR2]]
# SYM-NEXT:   Size: 4
# SYM-NEXT:   Binding: Global
# SYM-NEXT:   Type: Function
# SYM-NEXT:   Other: 0
# SYM-NEXT:   Section: .text
# SYM-NEXT: }
# SYM:      Symbol {
# SYM:        Name: T1@
# SYM-NEXT:   Value: 0x[[ADDR1]]
# SYM-NEXT:   Size: 4
# SYM-NEXT:   Binding: Global
# SYM-NEXT:   Type: Function
# SYM-NEXT:   Other: 0
# SYM-NEXT:   Section: .text
# SYM-NEXT: }

---
FileHeader:
  Class:   ELFCLASS32
  Data:    ELFDATA2LSB
  Type:    ET_REL
  Machine: EM_MIPS
  Flags:   [EF_MIPS_PIC, EF_MIPS_CPIC, EF_MIPS_ABI_O32, EF_MIPS_ARCH_32]

Sections:
- Name:         .text
  Type:         SHT_PROGBITS
  Size:         8
  AddressAlign: 16
  Flags:        [SHF_EXECINSTR, SHF_ALLOC]

- Name:         .rel.text
  Type:         SHT_REL
  Info:         .text
  AddressAlign: 4
  Relocations:
    - Offset: 0
      Symbol: T0
      Type:   R_MIPS_32
    - Offset: 4
      Symbol: .text
      Type:   R_MIPS_32

- Name:         .pdr
  Type:         SHT_PROGBITS
  Size:         8
  AddressAlign: 16
  Flags:        []

- Name:         .rel.pdr
  Type:         SHT_REL
  Info:         .pdr
  AddressAlign: 4
  Relocations:
    - Offset: 0
      Symbol: T1
      Type:   R_MIPS_32

Symbols:
  Local:
    - Name:    .text
      Type:    STT_SECTION
      Section: .text
  Global:
    - Name:    T0
      Section: .text
      Type:    STT_FUNC
      Value:   0
      Size:    4
    - Name:    T1
      Section: .text
      Type:    STT_FUNC
      Value:   4
      Size:    4
    - Name:    D1
      Section: .pdr
      Type:    STT_OBJECT
      Value:   0
      Size:    8
...
