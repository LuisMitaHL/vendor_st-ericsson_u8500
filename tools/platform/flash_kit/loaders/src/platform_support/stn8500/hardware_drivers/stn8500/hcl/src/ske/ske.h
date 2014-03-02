/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
*  Public Header file of SKE
*  AUTHOR :  ST-Ericsson
*/
/*****************************************************************************/

#ifndef __SKE_HEADER
#define __SKE_HEADER

#ifdef __cplusplus
extern "C"
{                                                           /* To allow C++ to use this header */
#endif
#include "hcl_defs.h"
#include "debug.h"

/* -------- SKE HCL VERSION --------- */
#define SKE_HCL_VERSION_ID  1
#define SKE_HCL_MAJOR_ID    0
#define SKE_HCL_MINOR_ID    11

/* --------------------------- ENUMERATIONS & STRUCTURES ----------------------------------------*/
typedef enum
{
    SKE_OK                  = HCL_OK,                   /* No error */
    SKE_UNSUPPORTED_HW      = HCL_UNSUPPORTED_HW,       /* From SKE_Init if the peripheral ids do not match */
    SKE_INVALID_PARAMETER   = HCL_INVALID_PARAMETER,    /* Invalid parameter across all platforms */
    SKE_UNSUPPORTED_FEATURE = HCL_UNSUPPORTED_FEATURE   /* Invalid parameter for specific platforms */
} t_ske_error;

typedef enum
{
    SKE_SCROLL_DEVICE_0     = 0x0,
    SKE_SCROLL_DEVICE_1
} t_ske_scroll_device;

typedef enum
{
    SKE_SCAN_COLUMN_0       = 0x0,
    SKE_SCAN_COLUMN_0_TO_1,
    SKE_SCAN_COLUMN_0_TO_2,
    SKE_SCAN_COLUMN_0_TO_3,
    SKE_SCAN_COLUMN_0_TO_4,
    SKE_SCAN_COLUMN_0_TO_5,
    SKE_SCAN_COLUMN_0_TO_6,
    SKE_SCAN_COLUMN_0_TO_7
} t_ske_scan_column;

typedef enum
{
    SKE_KEYPAD_COLUMN_0         = 0,
    SKE_KEYPAD_COLUMN_1,
    SKE_KEYPAD_COLUMN_2,
    SKE_KEYPAD_COLUMN_3,
    SKE_KEYPAD_COLUMN_4,
    SKE_KEYPAD_COLUMN_5,
    SKE_KEYPAD_COLUMN_6,
    SKE_KEYPAD_COLUMN_7,
    SKE_KEYPAD_COLUMN_MAX,
    SKE_KEYPAD_COLUMN_INVALID   = 0xFF
} t_ske_keypad_column;

typedef enum
{
    SKE_KEYPAD_ROW_0            = 0,
    SKE_KEYPAD_ROW_1,
    SKE_KEYPAD_ROW_2,
    SKE_KEYPAD_ROW_3,
    SKE_KEYPAD_ROW_4,
    SKE_KEYPAD_ROW_5,
    SKE_KEYPAD_ROW_6,
    SKE_KEYPAD_ROW_7,
    SKE_KEYPAD_ROW_MAX,
    SKE_KEYPAD_ROW_INVALID      = 0xFF
} t_ske_keypad_row;

typedef struct
{
    t_bool  keypad_drive_column[SKE_KEYPAD_COLUMN_MAX]; /* index of array to corresponds to the column. E.g. is_keypad_row_set[0] for column0 */
} t_ske_output_drive;

typedef struct
{
    t_bool  keypad_row_set[SKE_KEYPAD_ROW_MAX];         /* index of array to corresponds to the row. E.g. is_keypad_row_set[0] for row0 */
} t_ske_row_result;

/*------------------------------------- SKE HCL APIs ----------------------------------------------*/
/*--------- Initilisation & Supplemenatary APIs ------------*/
PUBLIC t_ske_error  SKE_Init(IN t_logical_address ske_base_address);
PUBLIC t_ske_error  SKE_GetVersion(OUT t_version *p_ske_hcl_version);
PUBLIC t_ske_error  SKE_SetDbgLevel(IN t_dbg_level dbg_level);

/*--------- Set/Get Control & Configuration information APIs ----------*/
/* For Scroll Key */
PUBLIC t_ske_error  SKE_EnableScrollKey(IN t_ske_scroll_device scroll_device);
PUBLIC t_ske_error  SKE_DisableScrollKey(IN t_ske_scroll_device scroll_device);
PUBLIC t_ske_error  SKE_SetScrollKeyCount(IN t_ske_scroll_device scroll_device, IN t_uint8 count_value);
PUBLIC t_ske_error  SKE_GetScrollKeyCount(IN t_ske_scroll_device scroll_device, OUT t_uint8 *p_count_value);
PUBLIC t_ske_error  SKE_ClearScrollKeyOverflowFlag(IN t_ske_scroll_device scroll_device);
PUBLIC t_ske_error  SKE_GetScrollKeyOverflowStatus(IN t_ske_scroll_device scroll_device, OUT t_bool *p_overflow_state);
PUBLIC t_ske_error  SKE_ClearScrollKeyUnderflowFlag(IN t_ske_scroll_device scroll_device);
PUBLIC t_ske_error  SKE_GetScrollKeyUnderflowStatus
                    (
                        IN t_ske_scroll_device  scroll_device,
                        OUT t_bool              *p_underflow_state
                    );
PUBLIC void         SKE_SetScrollKeyDebounce(IN t_uint8 debounce_value);
PUBLIC t_ske_error  SKE_GetScrollKeyDebounce(OUT t_uint8 *p_debounce_value);

/* For Keypad */
PUBLIC void         SKE_SetKeypadDebounce(IN t_uint8 debounce_value);
PUBLIC t_ske_error  SKE_GetKeypadDebounce(OUT t_uint8 *p_debounce_value);
PUBLIC void         SKE_EnableKeypadAutoScan(void);
PUBLIC void         SKE_DisableKeypadAutoScan(void);
PUBLIC t_bool       SKE_IsKeypadAutoScanEnabled(void);
PUBLIC t_bool       SKE_IsAutoScanOngoing(void);
PUBLIC void         SKE_EnableMultiKeyPress(void);
PUBLIC void         SKE_DisableMultiKeyPress(void);
PUBLIC t_bool       SKE_IsMultiKeyPressEnabled(void);
PUBLIC void         SKE_SetAutoScanColumns(IN t_ske_scan_column scan_columns);
PUBLIC t_ske_error  SKE_GetAutoScanColumns(OUT t_ske_scan_column *p_scan_columns);
PUBLIC t_ske_error  SKE_SetKeypadOutputDriveBulk(IN t_ske_output_drive *p_drive_column);
PUBLIC void         SKE_SetKeypadOutputDriveSingle(IN t_ske_keypad_column drive_column, IN t_bool to_drive);
PUBLIC t_ske_error  SKE_GetKeypadOutputDriveBulk(OUT t_ske_output_drive *p_driven_column);
PUBLIC t_ske_error  SKE_GetKeypadOutputDriveSingle(IN t_ske_keypad_column drive_column, OUT t_bool *p_driven);
PUBLIC t_ske_error  SKE_GetAutoScanResult(IN t_ske_keypad_column keypad_column, OUT t_ske_row_result *p_row_value);
PUBLIC t_ske_error  SKE_SetAutoScanResult(IN t_ske_keypad_column keypad_column, IN t_ske_row_result *p_row_value);
PUBLIC t_ske_error  SKE_SetKeypadMatrixSize(IN t_uint8 max_columns, IN t_uint8 max_rows);

PUBLIC t_bool SKE_IsSoftKeyPressed(void);
PUBLIC t_bool SKE_IsAutoKeyPressed(void);
PUBLIC void SKE_SetAllKeypadOutputDrive(IN t_bool to_drive);

/*For Keypad and Scroll Key*/
PUBLIC void         SKE_SaveDeviceContext(void);
PUBLIC void         SKE_RestoreDeviceContext(void);

#ifdef __cplusplus
}   /* To allow C++ to use this header */
#endif /* __cplusplus */
#endif /* __SKE_HEADER */

