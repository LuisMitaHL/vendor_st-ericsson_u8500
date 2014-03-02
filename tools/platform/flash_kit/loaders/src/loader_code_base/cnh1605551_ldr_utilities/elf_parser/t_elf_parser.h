/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef T_ELF_PARSER_H_
#define T_ELF_PARSER_H_

/**
 * @addtogroup ldr_utilities
 * @{
 *   @addtogroup elf_parser
 *   @{
 */
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_basicdefinitions.h"
#include "error_codes.h"
#include "t_io_subsystem.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/
/** Predefined constant for elf errors */
#define ELF_ERROR NULL
/** Predefined value for the index of section name string table. */
#define SHN_PREDEF 0

typedef enum {
    ELFCLASS32  = 1,
    ELFCLASS64
} El_class_t;

typedef enum {
    ELFDATA2LSB = 1,
    ELFDATA2MSB
} El_data_t;

typedef enum {
    EV_UNDEF   = 0,
    EV_CURRENT
} El_version_t;

#include "c_prepack.h"

typedef PRE_PACKED struct {
    /**< ELF identification */
    uint8       e_ident[16];
    /**< Object file type */
    uint16      e_type;
    /**< Specifies required architecture for the file */
    uint16      e_machine;
    /**< Object file version */
    uint32      e_version;
    /**< Virtual address to which the system first transfers control, thus
         starting the process. If the file has no associated entry point,
         it holds zero. */
    uint32      e_entry;
    /**< Program header table’s file offset in bytes. If the file has no
         program header table, this member holds zero. */
    uint32      e_phoff;
    /**< Section header table’s file offset in bytes. If the file has no section
         header table, this member holds zero. */
    uint32      e_shoff;
    /**< Processor-specific flags associated with the file. */
    uint32      e_flags;
    /**< ELF header’s size in bytes */
    uint16      e_ehsize;
    /**< Size in bytes of one entry in the file’s program header table; all
         entries are the same size. */
    uint16      e_phentsize;
    /**< Number of entries in the program header table.
         If a file has no program header table, this member holds zero. */
    uint16      e_phnum;
    /**< Section header’s size in bytes. All headers are the same size. */
    uint16      e_shentsize;
    /**< The number of entries in the section header table.
         If a file has no section header table, this member holds zero. */
    uint16      e_shnum;
    /**< Section header table index of the entry associated with the section
         name string table. */
    uint16      e_shntrdx;
} ELF32_Ehdr_t POST_PACKED;

typedef PRE_PACKED struct {
    /**< Index into the section header string table section,
         giving the location of a null terminated string. */
    uint32  sh_name;
    /**< Categorizes the section’s contents and semantics. */
    uint32  sh_type;
    /**< Sections support 1-bit flags that describe miscellaneous attributes. */
    uint32  sh_flags;
    /**< If the section will appear in the memory image of a process,
         this member gives the address at which the section’s first byte
         should reside. Otherwise, the member contains 0. */
    uint32  sh_addr;
    /**< Byte offset from the beginning of the file to the first
         byte in the section. */
    uint32  sh_offset;
    /**< Section’s size in bytes. */
    uint32  sh_size;
    /**< Section header table index link, whose interpretation
         depends on the section type. */
    uint32  sh_link;
    /**< Extra information, whose interpretation depends on the section type. */
    uint32  sh_info;
    /**< Address alignment constraints. Currently, only 0 and positive
         integral powers of two are allowed. Values 0 and 1 mean the section
         has no alignment constraints. */
    uint32  sh_addralign;
    /**< Some sections hold a table of fixed-size entries, this member gives
         the size in bytes of each entry. The member contains 0 if the section
         does not hold a table of fixed-size entries. */
    uint32  sh_entsize;
} ELF32_Shdr_t POST_PACKED;

typedef PRE_PACKED struct {
    /**< What kind of segment this array element describes or how to interpret
         the array element’s information. */
    uint32    p_type;
    /**< Offset from the beginning of the file at which the first byte
          of the segment resides. */
    uint32    p_offset;
    /**< Virtual address at which the first byte of the segment resides in memory. */
    uint32    p_vaddr;
    /**< Segment’s physical address. */
    uint32    p_paddr;
    /**< Number of bytes in the file image of the segment; it may be zero. */
    uint32    p_filesz;
    /**< Number of bytes in the memory image of the segment; it may be zero. */
    uint32    p_memsz;
    /**< Flags relevant to the segment. */
    uint32    p_flags;
    /**< Value to which the segments are aligned in memory and in the file. */
    uint32    p_align;
} ELF32_Phdr_t POST_PACKED;

#include "c_postpack.h"

typedef struct {
    IO_Handle_t Handle;             /**< IO Handle. */
    ELF32_Ehdr_t FileHeader;        /**< ELF file header. */
    ELF32_Shdr_t *SectionHeader_p;  /**< Pointer to section header. */
    ELF32_Phdr_t *ProgramHeader_p;
    char *StringSection_p;          /**< String section. */
} ElfParserContext_t;

/** @} */
/** @} */
#endif /*T_ELF_PARSER_H_*/
