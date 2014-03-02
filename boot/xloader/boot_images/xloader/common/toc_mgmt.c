/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
 * \file toc_mgmt.c
 * \author STMicroelectronics
 *
 * This module provides functions to analyse boot image
 *
 *---------------------------------------------------------------------------*/
 /** \internal
 * @{
 */
/*------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------*/
#include <stdarg.h>
#include <stddef.h>

#include "toc_mgmt.h"

#include "issw_printf.h"

/*------------------------------------------------------------------------
 * Defines
 *----------------------------------------------------------------------*/
#define FLAGS_MASK            0x0000FFFF
#define FLAGS_SUBTOC          0x00005354

/*------------------------------------------------------------------------
 * Public functions
 *----------------------------------------------------------------------*/

/*------------------------------------------------------------------------
 * Private functions
 *----------------------------------------------------------------------*/
PRIVATE size_t  toc_strlen(const char *s);
PRIVATE int     toc_strcmp(const char *s1, const char *s2);
PRIVATE char   *toc_strcpy(char *dest, const char *src);

/*---------------------------------------------------------------------------*/
/*                    TOC_SearchItemInToc()                                  */
/*---------------------------------------------------------------------------*/
/** \internal
 *
 * This routine will search p_tocItem->FileName in TOC.
 * If found, it will set other fields of p_tocItem
 *
 * \param[in]  tocAddress   Start adress of TOC to analyse
 * \param[in]  subtocAddress  Start adress of SUBTOC to analyse
 * \param[in]  p_stringItem String to search in TOC
 * \param[out]  p_tocItem   if p_stringItem is found in TOC, copy
 *                           infor from TOC in p_tocItem
 * \param[out]  p_in_subtoc Set to true if item was found in SUBTOC
 *
 *
 * \return  error code \n
 *         TRUE: no error\n
 *         FALSE: item not found \n
 *
 */
/*---------------------------------------------------------------------------*/
PUBLIC t_bool TOC_SearchItemInToc (const t_uint32 tocAddress,
                                   const t_uint32 subtocAddress,
                                   const t_uint32 subtocSize,
                                   const char *p_stringItem,
                                   t_boot_toc_item* p_tocItem,
                                   t_bool *p_in_subtoc)
{
    t_bool status  = FALSE;
    t_boot_toc_item *p_item = (t_boot_toc_item *) tocAddress;

    if (toc_strlen(p_stringItem) > ITEM_NAME_SIZE) {
      return FALSE;
    }

    while ((tocAddress + (ITEM_MAX_NB*sizeof(t_boot_toc_item)) >
            (t_uint32) p_item) && ((p_item->Start == 0xFFFFFFFE) ||
            (toc_strcmp(p_item->FileName, p_stringItem) != 0))) {
        p_item++;       /* TOC item not found, let's verify next item */
    }
    if ((t_uint32) p_item >= tocAddress +
        (ITEM_MAX_NB*sizeof(t_boot_toc_item))) {
        /* TOC item not found, search in SUBTOC instead */
        if (subtocAddress)
        {
            p_item = (t_boot_toc_item *) subtocAddress;
            while( (subtocAddress + (subtocSize/sizeof(t_boot_toc_item))
                         > (t_uint32) p_item)
                    && ( (p_item->Start == 0xFFFFFFFE) ||
                         (toc_strcmp(p_item->FileName, p_stringItem) != 0 )))
            {
                p_item++; /* TOC item not found, let's verify next item */
            }

            if((t_uint32) p_item <
                subtocAddress + (subtocSize/sizeof(t_boot_toc_item)))
            {
               /* TOC item found in SUBTOC */
               status = TRUE;
               *p_in_subtoc = TRUE;
            }
        }
    } else {
       /* TOC item found in TOC */
       status = TRUE;
    }

    if (status != FALSE) {
        p_tocItem->Start    = p_item->Start;
        p_tocItem->Size     = p_item->Size;
        p_tocItem->Flags    = p_item->Flags;
        p_tocItem->Align    = p_item->Align;
        p_tocItem->Load     = p_item->Load;
        toc_strcpy(p_tocItem->FileName, p_stringItem);
    }

    return status;
}  /* end of TOC_SearchItemInToc() */

/*---------------------------------------------------------------------------*/
/*                    TOC_GetItemInfo()                                  */
/*---------------------------------------------------------------------------*/
/** \internal
 *
 * This routine will search p_tocItem->FileName in TOC.
 * If found, it will set other fields : size and address
 *
 * \param[in]  tocAddress   Start adress of TOC to analyse
 * \param[in]  subtocAddress  Start adress of SUBTOC to analyse
 * \param[in]  p_stringItem String to search in TOC
 * \param[out]  p_itemSize   filled with the size in bytes of the item
 * \param[out]  p_itemOffset  filled with offset of the item from the TOC.
 * \param[out]  p_subtoc      set to TRUE if item was found in SUBTOC..
 * \param[out]  p_itemFlags   filled with flags of the item from the TOC.
 *
 *
 * \return  error code \n
 *         TRUE: no error\n
 *         FALSE: item not found \n
 *
 */
/*---------------------------------------------------------------------------*/
PUBLIC t_bool TOC_GetItemInfo(t_uint32 tocAddress,
                              t_uint32 subtocAddress,
                              t_uint32 subtocSize,
                              char *p_stringItem,
                              t_size *p_itemSize,
                              t_address *p_itemOffset,
                              t_bool *p_subtoc,
                              t_uint32 *p_itemFlags)
{
    t_bool status = TRUE;
    t_boot_toc_item item ;

    status = TOC_SearchItemInToc (tocAddress, subtocAddress, subtocSize,
                p_stringItem, &item, p_subtoc);

    *p_itemSize     = (t_size) item.Size;
    *p_itemOffset   = (t_address) item.Start;
    *p_itemFlags    = (t_address) item.Flags;

    return status;
}  /* end of TOC_GetItemInfo() */

/*---------------------------------------------------------------------------*/
/*                    toc_find_subtoc()                                       */
/*---------------------------------------------------------------------------*/
/** \internal
 *
 * This routine will search for a subotoc in TOC.
 * If found, it will return true
 *
 * \param[in]  toc_address    Start adress of TOC to analyse
 * \param[out] p_subtoc_size  If subtoc is found this will contain the size,
 *                            otherwise 0.
 *
 * \return TRUE: subtoc found\n
 *         FALSE: subtoc not found \n
 *
 */
/*---------------------------------------------------------------------------*/
t_bool toc_find_subtoc(t_uint32 toc_address, t_uint32 *p_subtoc_size)
{
    t_boot_toc_item *p_item = (t_boot_toc_item *) toc_address;

    if (NULL != p_subtoc_size) {
        *p_subtoc_size = 0;
    } else {
        return FALSE;
    }

    while ((toc_address + (ITEM_MAX_NB*sizeof(t_boot_toc_item))
                   > (t_uint32) p_item)
               && ((p_item->Start == 0xFFFFFFFE) ||
                   (toc_strcmp(p_item->FileName, ASCII_SUBTOC) != 0))
               && ((p_item->Flags & FLAGS_MASK) == FLAGS_SUBTOC)) {
        p_item++; /* TOC item not found, let's verify next item */
    }

    if ((t_uint32) p_item >= toc_address +
      (ITEM_MAX_NB*sizeof(t_boot_toc_item))) {
        /* SUBTOC item not found */
        return FALSE;
    }

    *p_subtoc_size = p_item->Size;

    return TRUE;
}



PRIVATE size_t toc_strlen(const char *s)
{
    size_t  l = 0;

    while (*s != '\0') {
        s++;
        l++;
    }
    return l;
}

PRIVATE int toc_strcmp(const char *s1, const char *s2)
{
    while (*s1 != '\0' && *s1 == *s2) {
        s1++;
        s2++;
    }

    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

PRIVATE char   *toc_strcpy(char *dest, const char *src)
{
    char *ret = dest;

    while (*src != '\0') {
        *dest = *src;
        dest++;
        src++;
    }
    return ret;
}

/* end of toc_mgmt.c */
/** @} */
