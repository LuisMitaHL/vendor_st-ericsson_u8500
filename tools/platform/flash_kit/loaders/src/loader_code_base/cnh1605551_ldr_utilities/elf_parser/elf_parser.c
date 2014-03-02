/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/*
 * @addtogroup ldr_utilities
 * @{
 *   @addtogroup elf_parser
 *   @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <string.h>

#include "r_debug.h"
#include "r_debug_macro.h"
#include "r_memory_utils.h"
#define EMP_Printf printf     /**< @todo remove in final version */
#include "t_elf_parser.h"
#include "r_elf_parser.h"
#include "r_io_subsystem.h"

static const uint8 HEADER_PATERN[] = {0x7f, 'E', 'L', 'F', ELFCLASS32, ELFDATA2LSB, EV_CURRENT};

ElfParserContext_t *Do_ElfParser_Create(const IO_Handle_t Handle)
{
    ELF32_Shdr_t *StringHeader_p;
    uint64 ReadLength;
    uint64 BufferSize;
    int CompareResult;
    ElfParserContext_t *Context_p = NULL;
    uint32 MaxOpenIO = 0;
    uint64 SegmentBufferSize;

    MaxOpenIO = Do_IO_GetNrOfMaxOpenEntry();

    if (Handle >= MaxOpenIO) {
        A_(printf("Error: Handle index out of the range elf64_parser.c (%d)\n\n", __LINE__);)
        return ELF_ERROR;
    }

    Context_p = malloc(sizeof(ElfParserContext_t));
    ASSERT(NULL != Context_p);

    memset(Context_p, 0, sizeof(ElfParserContext_t));
    BufferSize = (uint64) sizeof(ELF32_Ehdr_t);
    Context_p->Handle = Handle;
    ReadLength = Do_IO_PositionedRead(Handle, &Context_p->FileHeader, BufferSize, BufferSize, (uint64) 0);

    if (ReadLength == IO_ERROR) {
        A_(printf("Error: Read failed elf64_parser.c (%d)\n\n", __LINE__);)
        goto ErrorExit;
    }

    CompareResult = memcmp(Context_p->FileHeader.e_ident, HEADER_PATERN, sizeof(HEADER_PATERN));

    if (CompareResult) {
        A_(printf("Error: Elf marker doesen't mach elf64_parser.c (%d)\n\n", __LINE__);)
        goto ErrorExit;
    }

    if (Context_p->FileHeader.e_shentsize != sizeof(ELF32_Shdr_t)) {
        A_(printf("Error: Elf marker doesen't mach elf64_parser.c (%d)\n\n", __LINE__);)
        goto ErrorExit;
    }

    BufferSize = (uint64)Context_p->FileHeader.e_shentsize * Context_p->FileHeader.e_shnum;
    // @todo check if BufferSize is bigger then file size
    Context_p->SectionHeader_p = malloc((size_t)BufferSize);
    ASSERT(NULL != Context_p->SectionHeader_p);

    ReadLength = Do_IO_PositionedRead(Handle, Context_p->SectionHeader_p, BufferSize, BufferSize, Context_p->FileHeader.e_shoff);

    if (ReadLength != BufferSize) {
        A_(printf("Error: Read failed elf64_parser.c (%d)\n\n", __LINE__);)
        goto ErrorExit;
    }

    if (Context_p->FileHeader.e_shntrdx != SHN_PREDEF) {
        StringHeader_p = &Context_p->SectionHeader_p[Context_p->FileHeader.e_shntrdx];
        Context_p->StringSection_p = malloc((size_t)(StringHeader_p->sh_size));
        ASSERT(NULL != Context_p->StringSection_p);

        ReadLength = Do_IO_PositionedRead(Handle, Context_p->StringSection_p, StringHeader_p->sh_size, StringHeader_p->sh_size, StringHeader_p->sh_offset);

        if (ReadLength != StringHeader_p->sh_size) {
            A_(printf("Error: Read failed elf64_parser.c (%d)\n\n", __LINE__);)
            goto ErrorExit;
        }
    } else {
        Context_p->StringSection_p = NULL;
    }

    SegmentBufferSize = (uint64)Context_p->FileHeader.e_phentsize * Context_p->FileHeader.e_phnum;
    Context_p->ProgramHeader_p = malloc((size_t)SegmentBufferSize);
    ASSERT(NULL != Context_p->ProgramHeader_p);

    ReadLength = Do_IO_PositionedRead(Handle, Context_p->ProgramHeader_p, SegmentBufferSize, SegmentBufferSize, Context_p->FileHeader.e_phoff);

    if (ReadLength != SegmentBufferSize) {
        A_(printf("Error: Read failed elf64_parser.c (%d)\n\n", __LINE__);)
        goto ErrorExit;
    }

    return Context_p;

ErrorExit:
    BUFFER_FREE(Context_p);
    return ELF_ERROR;
}

char *Do_ElfParser_GetSectionName(const ElfParserContext_t *const Context_p, const uint32 SectionIndex)
{
    if (Context_p->StringSection_p == NULL) {
        return ELF_ERROR;
    }

    return &Context_p->StringSection_p[Context_p->SectionHeader_p[SectionIndex].sh_name];
}

uint32 Do_ElfParser_GetSectionCount(const ElfParserContext_t *const Context_p)
{
    return Context_p->FileHeader.e_shnum;
}

uint32 Do_ElfParser_GetSegmentCount(const ElfParserContext_t *const Context_p)
{
    return Context_p->FileHeader.e_phnum;
}

void Do_ElfParser_Destroy(ElfParserContext_t *Context_p)
{
    if (Context_p != NULL) {
        BUFFER_FREE(Context_p->SectionHeader_p);
        BUFFER_FREE(Context_p->StringSection_p);
        BUFFER_FREE(Context_p->ProgramHeader_p);
        BUFFER_FREE(Context_p);
    }
}

IO_Handle_t Do_ElfParser_OpenSection(const ElfParserContext_t *const Context_p, const uint32 SectionIndex)
{
    ELF32_Shdr_t *Section_p = &Context_p->SectionHeader_p[SectionIndex];

    return Do_IOW_WrapOpen(Context_p->Handle, Section_p->sh_size, Section_p->sh_offset);
}

uint64 Do_ElfParser_GetSectionLength(const ElfParserContext_t *const Context_p, const uint32 SectionIndex)
{
    ELF32_Shdr_t *Section_p = &Context_p->SectionHeader_p[SectionIndex];

    return Section_p->sh_size;
}

uint32 Do_ElfParser_GetSectionType(const ElfParserContext_t *const Context_p, const uint32 SectionIndex)
{
    ELF32_Shdr_t *Section_p = &Context_p->SectionHeader_p[SectionIndex];

    return Section_p->sh_type;
}

IO_Handle_t Do_ElfParser_OpenSegment(const ElfParserContext_t *const Context_p, const uint32 SegmentIndex)
{
    ELF32_Phdr_t *Segment_p = &Context_p->ProgramHeader_p[SegmentIndex];

    return Do_IOW_WrapOpen(Context_p->Handle, Segment_p->p_filesz, Segment_p->p_offset);
}

uint64 Do_ElfParser_GetSegmentLength(const ElfParserContext_t *const Context_p, const uint32 SegmentIndex)
{
    ELF32_Phdr_t *Segment_p = &Context_p->ProgramHeader_p[SegmentIndex];

    return Segment_p->p_filesz;
}

uint32 Do_ElfParser_GetSegmentType(const ElfParserContext_t *const Context_p, const uint32 SegmentIndex)
{
    ELF32_Phdr_t *Segment_p = &Context_p->ProgramHeader_p[SegmentIndex];

    return Segment_p->p_type;
}

/* @} */
/* @} */
