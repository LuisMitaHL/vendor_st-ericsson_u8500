/*******************************************************************************
 * $Copyright ST-Ericsson 2012 $
 ******************************************************************************/
#ifndef _PREFLASH_TOOL_H_
#define _PREFLASH_TOOL_H_

#include "t_io_subsystem.h"
#include "r_manifest_parser.h"
#include "r_flashlayout_parser.h"
#include "cspsa_handler.h"
#include "cli_parser.h"
#include "toc_handler.h"

#define MAX_DATA_LENGTH_IN_SRECORD 0x10
#define CHECKSUM_LENGTH_IN_SRECORD 0x01
#define BOOT_IMAGE_SIZE 0x20000
#define BOOT_BLOCK_SIZE 0x80000

// Max size of buffer that is used for adding pad in output image
#define PAD_BUFFER_SIZE 2048
// Start the output image from the second copy of the ToC
#define FIRST_TOC_NUMBER 2

/**
 * Enumerated preflash type.
 */
typedef enum {
    PREFLASH_IMAGE = 0,
    TOC_IMAGE,
} PreflashType_t;

typedef struct {
    IO_Handle_t ZipIOHandle;
    IO_Handle_t ZipInZipIOHandle;
    void *ZipHandle_p;
    void *ZipInZipHandle_p;
    ManifestParser_t *ManifestParser_p;
    uint32 ImagesInManifest;
    FlashLayoutParser_t *FlashLayoutParser_p;
    uint32 ImagesInFlashLayout;

    char SubTOCID[TOC_ID_LENGTH];
    uint32 SubTOCStartAddress;
    uint32 SubTOCSize;

    CSPSA_Handle_t CSPSAHandle;

    uint8 BAMFlash;
    uint8 SubTOCFlash;
    boolean GenerateBinary;
    boolean SkipFirstBlock;
    boolean Flashlayout_3_0;
    boolean SplitOutputImage;

    PreflashType_t PreflashType;
} LocalParameters_t;

/**
 * @brief Command that generates preflash image
 *
 * @param [in] Command_p - Structure that contains command line parameters
 *
 * @return     0 after successful execution, negative value otherwise
 *
 */
int CommandPreFlash(Command_t *Command_p);

/**
 * @brief Command that generates boot area image
 *
 * @param [in] Command_p - Structure that contains command line parameters
 *
 * @return     0 after successful execution, negative value otherwise
 *
 */
int CommandTocGen(Command_t *Command_p);

/**
 * @brief Help command that lists all available options
 *
 * @param [in] Command_p - Structure that contains command line parameters
 *
 * @return     0 after successful execution, negative value otherwise
 *
 */
int CommandHelp(Command_t *Command_p);

#endif
