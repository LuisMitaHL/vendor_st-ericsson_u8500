/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _R_ELF_PARSER_H_
#define _R_ELF_PARSER_H_

/**
 * @addtogroup ldr_utilities
 * @{
 *   @addtogroup elf_parser
 *   @{
 *   Implementation of ELF files parser.
 *   It implements functionalities: @n
 *   - Initialize Elf parser context (Do_ElfParser_Create), @n
 *   - Get Section count (Do_ElfParser_GetSectionCount), @n
 *   - Open section with SectionIndex (Do_ElfParser_OpenSection), @n
 *   - Read section length (Do_ElfParser_GetSectionLength), @n
 *   - Reads section name (Do_ElfParser_GetSectionName), @n
 *   - Destroy Elf parser context (Do_ElfParser_Destroy) @n
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_elf_parser.h"

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/
/**
 *  @brief Initializes Elf parser context.
 *
 *  Function must be called first to form elf parser context for elf file
 *  referenced by the IO Handle.
 *
 *  @param [in] Handle          IO Handle to an elf file opened with
 *                              Do_IO_FileOpen.
 *  @retval ElfParserContext_t* Pointer to Elf parser context.
 *  @retval ELF_ERROR           If invalid IO handle is passed to function, or
 *                              there is not enough place for elf parser context
 *                              or can not read from elf file, or file is not in
 *                              compliance with elf64 file format.
 */
ElfParserContext_t *Do_ElfParser_Create(const IO_Handle_t Handle);

/**
 *  @brief Reads the count of sections present in the elf file.
 *
 *  @param [in]   Context_p     Pointer to initialized elf parser context.
 *  @return uint32              Total number of sections.
 */
uint32 Do_ElfParser_GetSectionCount(const ElfParserContext_t *const Context_p);

/**
 *  @brief Opens section with SectionIndex and returns IO handle to it.
 *
 *  @param [in] Context_p       Pointer to initialized elf parser context.
 *  @param [in] SectionIndex    Which section in elf file to open.
 *  @retval IO_Handle_t         An IO Handle for use with the I/O subsystem API
 *                              functions.
 *  @retval IO_INVALID_HANDLE   If invalid Handle is passed to function, or
 *                              resource can not be opened, or no space can be
 *                              allocated for wrap context, or there is no free
 *                              IO channel available.
 */
IO_Handle_t Do_ElfParser_OpenSection(const ElfParserContext_t *const Context_p,
                                     const uint32 SectionIndex);

/**
 *  @brief Reads section length.
 *
 *  @param [in] Context_p       Pointer to Elf parser context.
 *  @param [in] SectionIndex    For which section in elf file to get the length.
 *  @retval  Length             Length in bytes of the section.
 */
uint64 Do_ElfParser_GetSectionLength(const ElfParserContext_t *const Context_p,
                                     const uint32 SectionIndex);

/**
 *  @brief Reads section name and returns pointer to string.
 *
 *  @param [in] Context_p       Pointer to Elf parser context.
 *  @param [in] SectionIndex    For which section in elf file to read the name.
 *  @return char*               Pointer to string.
 */
char *Do_ElfParser_GetSectionName(const ElfParserContext_t *const Context_p,
                                  const uint32 SectionIndex);

/**
 *  @brief Destroys Elf parser context.
 *
 *  @param [in] Context_p       Pointer to Elf parser context.
 *  @return                     Doesn’t return value.
 */
void Do_ElfParser_Destroy(ElfParserContext_t *Context_p);

/**
 *  @brief Reads section length.
 *
 *  @param [in] Context_p       Pointer to Elf parser context.
 *  @param [in] SectionIndex    For which section in elf file to get the type.
 *  @retval     Type            Type of the section.
 */
uint32 Do_ElfParser_GetSectionType(const ElfParserContext_t *const Context_p, const uint32 SectionIndex);

uint32 Do_ElfParser_GetSegmentCount(const ElfParserContext_t *const Context_p);
IO_Handle_t Do_ElfParser_OpenSegment(const ElfParserContext_t *const Context_p, const uint32 SegmentIndex);
uint64 Do_ElfParser_GetSegmentLength(const ElfParserContext_t *const Context_p, const uint32 SegmentIndex);
uint32 Do_ElfParser_GetSegmentType(const ElfParserContext_t *const Context_p, const uint32 SegmentIndex);

/** @} */
/** @} */
#endif /*_R_ELF_PARSER_H_*/
