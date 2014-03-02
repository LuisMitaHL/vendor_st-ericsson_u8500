/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef _R_SERIALIZATION_H_
#define _R_SERIALIZATION_H_

/**
 * @addtogroup ldr_utilities
 * @{
 *   @addtogroup serialization
 *   @{
 *   This module contains functions for serialization of data.
 *   @n The reason for having this kind of functions is to ease the
 *   manipulation of data and to simplify the code.
 *   @n The functions are divided in three groups:@n
 *   @li get_* functions:
 *       Used for extracting data from a memory location specified
 *       by the input pointer.
 *   @li put_* functions:
 *       Used for placing data at the memory location specified by the input
 *       pointer.
 *   @li skip_* functions:
 *       Used for skipping the data, without changing it.
 *   @n After call to anyone of this functions the input pointer is
 *      incremented for the number of bytes that are extracted, placed or
 *      skipped.
 *
 *   @remark None of the functions perform a sanity check of the input
 *           parameters. It is the responsibility of the developer to make sure
 *           that the arguments passed to these functions are valid.
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_basicdefinitions.h"
#include "t_serialization.h"
#if defined(CFG_ENABLE_LOADER_SERIALIZATION)
#include "command_ids.h"
#endif

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/

/**
 * @brief Reads 1 byte from @p data_pp.
 *
 * Reads 1 byte from @p *data_pp and increases
 * @p *data_pp for 1 byte.
 *
 * @param [in,out] data_pp  Source.
 * @return         An @p uint8 from @p *data_pp.
 */
uint8 get_uint8(void **data_pp);

/**
 * @brief Reads 2 bytes from @p data_pp.
 *
 * Reads @p 2 bytes from @p *data_pp and increases
 * @p *data_pp by 2 bytes. The bytes are considered to
 * be in Little Endian order.
 *
 * @param [in,out] data_pp  Source.
 * @return         An @p uint16 from @p *data_pp.
 */
uint16 get_uint16_le(void **data_pp);

/**
 * @brief Reads 2 bytes from @p data_pp.
 *
 * Reads 2 bytes from @p *data_pp and increases
 * @p *data_pp by 2 bytes. The bytes are considered to
 * be in Big Endian order.
 *
 * @param [in,out] data_pp  Source.
 * @return         An @p uint16 from @p *data_pp.
 */
uint16 get_uint16_be(void **data_pp);

/**
 * @brief Reads 4 bytes from @p data_pp.
 *
 * Reads 4 bytes from @p *data_pp and increases
 * @p *data_pp by 4 bytes. The bytes are considered to
 * be in Little Endian order.
 *
 * @param [in,out] data_pp  Source.
 * @return         An @p uint32 from @p *data_pp.
 */
uint32 get_uint32_le(void **data_pp);

/**
 * @brief Reads 4 bytes from @p data_pp.
 *
 * Reads 8 bytes from @p *data_pp and increases
 * @p *data_pp by 8 bytes. The bytes are considered to
 * be in Little Endian order.
 *
 * @param [in,out] data_pp  Source.
 * @return         An @p uint64 from @p *data_pp.
 */
uint64 get_uint64_le(void **data_pp);

/**
 * @brief Reads 4 bytes from @p data_pp.
 *
 * Reads 4 bytes from @p *data_pp. The bytes are considered to
 * be in Little Endian order.
 *
 * @param [in,out] data_pp  Source.
 * @return         An @p uint32 from @p *data_pp.
 */
uint32 get_uint32_string_le(void **data_pp);

/**
 * @brief Reads 4 bytes from @p data_pp.
 *
 * Reads 4 bytes from @p *data_pp and increases
 * @p *data_pp by 4 bytes. The bytes are considered to
 * be in Big Endian order.
 *
 * @param [in,out] data_pp  Source.
 * @return         An @p uint32 from @p *data_pp.
 */
uint32 get_uint32_be(void **data_pp);

/**
 * @brief Writes 1 byte to @p data_pp.
 *
 * Writes 1 byte to @p *data_pp and increases
 * @p *data_pp by 1 byte.
 *
 * @param [in,out] data_pp Destination.
 * @param [in]     v       Value.
 * @return         void.
 */
void put_uint8(void **data_pp,
               uint8 v);

/**
 * @brief Writes 2 bytes to @p data_pp.
 *
 * Writes 2 byte to @p *data_pp and increases
 * @p *data_pp by 2 byte. The bytes are written
 * in Little Endian Order.
 *
 * @param [in,out] data_pp Destination.
 * @param [in]     v       Value.
 * @return         void.
 */
void put_uint16_le(void **data_pp,
                   uint16 v);

/**
 * @brief Writes 2 bytes to @p data_pp.
 *
 * Writes 2 byte to @p *data_pp and increases
 * @p *data_pp by 2 byte. The bytes are written
 * in Big Endian Order.
 *
 * @param [in,out] data_pp Destination.
 * @param [in]     v       Value.
 * @return         void.
 */
void put_uint16_be(void **data_pp,
                   uint16 v);

/**
 * @brief Writes 4 bytes to @p data_pp.
 *
 * Writes 4 byte to @p *data_pp and increases
 * @p *data_pp by 4 byte. The bytes are written
 * in Little Endian Order.
 *
 * @param [in,out] data_pp Destination.
 * @param [in]     v       Value.
 * @return         void.
 */
void put_uint32_le(void **data_pp,
                   uint32 v);

/**
 * @brief Writes 4 bytes to @p data_pp.
 *
 * Writes 4 byte to @p *data_pp and increases
 * @p *data_pp by 4 byte. The bytes are written
 * in Big Endian Order.
 *
 * @param [in,out] data_pp Destination.
 * @param [in]     v       Value.
 * @return         void.
 */
void put_uint32_be(void **data_pp,
                   uint32 v);

/**
 * @brief Writes 8 bytes to @p data_pp.
 *
 * Writes 8 byte to @p *data_pp and increases
 * @p *data_pp by 4 byte. The bytes are written
 * in Little Endian Order.
 *
 * @param [in,out] data_pp Destination.
 * @param [in]     v       Value.
 * @return         void.
 */
void put_uint64_le(void **data_pp,
                   uint64 v);

/**
 * @brief Writes 8 bytes to @p data_pp.
 *
 * Writes 8 byte to @p *data_pp and increases
 * @p *data_pp by 8 byte. The bytes are written
 * in Big Endian Order.
 *
 * @param [in,out] data_pp Destination.
 * @param [in]     v       Value.
 * @return         void.
 */
void put_uint64_be(void **data_pp,
                   uint64 v);

/**
 * @brief Skips 1 byte from @p data_pp.
 *
 * Skips 1 byte from data_pp, and increases @p data_pp
 * for 1 byte.
 *
 * @param [in,out] data_pp Destination.
 * @return         void.
 */
void skip_uint8(void **data_pp);

/**
 * @brief Skips 2 byte from @p data_pp.
 *
 * Skips 2 byte from data_pp, and increases @p data_pp
 * for 2 byte.
 *
 * @param [in,out] data_pp Destination.
 * @return         void.
 */
void skip_uint16(void **data_pp);

/**
 * @brief  Skips 4 byte from @p data_pp.
 *
 * Skips 4 byte from data_pp, and increases @p data_pp
 * for 4 byte.
 *
 * @param [in,out] data_pp Destination.
 * @return         void.
 */
void skip_uint32(void **data_pp);

/**
 * @brief Copies bytes from @p data_pp to @p target_p.
 *
 * Copies @p length bytes from @p data_pp to @p target_p,
 * increasing @p data_pp by @p length.
 *
 * @param [in,out] data_pp  Source.
 * @param [out]    target_p Destination.
 * @param [in]     length   Length of block.
 * @return         void.
 */
void get_block(const void **data_pp,
               void *target_p,
               uint32 length);

/**
 * @brief Copies bytes from @p source_p to @p data_pp.
 *
 * Copies @p length bytes from @p source_p to @p data_pp, increasing
 * @p data_pp by @p length.
 *
 * @param [in,out] data_pp  Source.
 * @param [out]    source_p Destination.
 * @param [in]     length   Length of block.
 * @return         void.
 */
void put_block(void **data_pp,
               const void *source_p,
               uint32 length);

/**
 * @brief Copies bytes from @p source_p to @p data_pp.
 *
 * First is copied Length of the buffer (source_p) in the data_pp then.
 * is copied @p length bytes from @p source_p to @p data_pp, increasing
 * @p data_pp by @p length.
 *
 * @param [in,out] data_pp  Source.
 * @param [out]    source_p Destination.
 * @param [in]     length   Length of block.
 * @return         void.
 */
void put_string(void **data_pp,
                const void *source_p,
                uint32 length);

/**
 * @brief Skips a block of length @p length from @p data_pp.
 *
 * Skips a block of length @p length from @p data_pp.
 *
 * @param [in,out] data_pp Source.
 * @param [in]     length  Length of block.
 * @return         void.
 */
void skip_block(void **data_pp,
                uint32 length);

/**
 * @brief Skips a block of length of the string from @p data_pp.
 *
 * @param [in,out] data_pp Source.
 * @return         Pointer to where data_pp was before calling this function.
 */
char *skip_str(void **data_pp);

/**
 * @brief Serialize device entries.
 *
 * @param [out] data_pp            Pointer of serialized data for device entries.
 * @param [in]  source_p           Source.
 * @param [in]  length             length of source string.
 * @return      void.
 */
void insert_string(char **data_pp, const char *source_p, uint32 length);

#if defined(CFG_ENABLE_LOADER_SERIALIZATION)
/**
 * @brief Get directory entries length.
 *
 * @param [out] source_p              Source.
 * @param [in]  DirectoryEntriesCount Entry counter.
 * @return      Entry length.
 */
uint32 get_directory_entries_len(const DirEntry_t *source_p,
                                 uint32 DirectoryEntriesCount);

/**
 * @brief Serialize directory entries.
 *
 * @param [out] data_pp      Pointer of serialized data for directory entries.
 * @param [in]  source_p     Source.
 * @param [in]  DirectoryEntriesCount Entry counter.
 * @return      void.
 */
void serialize_directory_entries(void **data_pp,
                                 const DirEntry_t *source_p,
                                 uint32 DirectoryEntriesCount);

/**
 * @brief Get device entry length.
 *
 * @param [out] source_p           Source.
 * @param [in]  DeviceEntriesCount Entry counter.
 * @return      Entry length.
 */
uint32 get_device_entries_len(const ListDevice_t *source_p,
                              uint32 DeviceEntriesCount);

/**
 * @brief Serialize device entries.
 *
 * @param [out] data_pp            Pointer of serialized data for device entries.
 * @param [in]  source_p           Source.
 * @param [in]  DeviceEntriesCount Entry counter.
 * @return      void.
 */
void serialize_device_entries(void **data_pp,
                              const ListDevice_t *source_p,
                              uint32 DeviceEntriesCount);
#endif

/** @} */
/** @} */
#endif /*_R_SERIALIZATION_H_*/
