/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   misc.h
* \brief   generic utilities
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef MISC_H_
#define MISC_H_

#include "dth_tools.h"

typedef enum {
	LSB_POSS, MSB_POSS
} t_position;

/**
 * Set a byte into a word
 * @param[in] pl_position specifies position in the word where set the byte
 * (LSB or MSB).
 * @param[in] pl_dest specifies word to set.
 * @param[in] pl_srce specifies byte value.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
u16 SetU8InU16(t_position pl_position, u16 pl_dest, u8 pl_srce);

/**
 * Set a word into an integer
 * @param[in] pl_position specifies position in the integer where set the word
 * (LSW or MSW).
 * @param[in] pl_dest specifies integer to set.
 * @param[in] pl_srce specifies word value.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
u32 SetU16InU32(t_position pl_position, u32 pl_dest, u16 pl_srce);

/**
 * Get a byte from a word
 * @param[in] pl_position specifies position in the word where get the byte
 * (LSB or MSB).
 * @param[in] pl_dest specifies word value.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
u8 GetU8InU16(t_position pl_position, u16 pl_dest);

/**
 * Get a word from a integer
 * @param[in] pl_position specifies position in the integer where get the word
 * (LSW or MSW).
 * @param[in] pl_dest specifies integer value.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
u16 GetU16InU32(t_position pl_position, u32 pl_dest);

#define LOWBYTE(w) GetU8InU16(LSB_POSS, w)
#define HIGHBYTE(w) GetU8InU16(MSB_POSS, w)
#define MAKE16(high_byte, low_byte) \
    ((u16)((0x00FF & (low_byte)) | (0xFF00 & ((high_byte) << 8))))

void PrintBinary(void *pp_buf, size_t vp_size);

int SetDthQuot(struct dth_element *elem, void *pp_value, void *pp_var,
	       int vp_quot);

int GetDthQuot(struct dth_element *elem, void *pp_value, void *pp_var,
	       int vp_quot);

/**
 * Raw copy of complete/cell data into a DTH array struct.
 * @param[in] pl_elem       address of a valid DTH array element.
 * @param[in/out] pl_array  address of a DTH array. col and row attributes 
 *                          specify the cells to return.
 * @param[in] pl_array_size size in bytes of an array element.
 * @param[in] pl_data       address of the data
 * @param[in] pl_data_size  size in bytes of a data element
 * @param[in] pl_nb_data    number of data elements
 */
void GetDthArray(struct dth_element *pl_elem, struct dth_array *pl_array,
		 int pl_array_size, void *pl_data, int pl_data_size,
		 int pl_nb_data);

/**
 * Raw copy of complete/cell data from a DTH array struct.
 * @param[in] pl_elem       address of a valid DTH array element.
 * @param[in] pl_array      address of a DTH array. col and row attributes 
 *                          specify the cells to return.
 * @param[in] pl_array_size size in bytes of an array element.
 * @param[in/out] pl_data   address of the data
 * @param[in] pl_data_size  size in bytes of a data element
 * @param[in] pl_nb_data    number of data elements
 */
void SetDthArray(struct dth_element *pl_elem, struct dth_array *pl_array,
		 int pl_array_size, void *pl_data, int pl_data_size,
		 int pl_nb_data);

#define DTH_SET_QUOT(elem, value, var_result, var_val, vp_quot) \
    do {    \
        var_result = SetDthQuot(elem, value, &var_val, vp_quot);    \
    } while (0);

#define DTH_GET_QUOT(elem, value, var_val, vp_quot, var_result) \
    do {    \
        var_result = GetDthQuot(elem, value, &var_val, vp_quot);    \
    } while (0);

#define DTH_SET_L8(elem, value, var_val, var_result)    \
    do {    \
        u16 vl_local;   \
        DTH_SET_UNSIGNED(elem, value, vl_local, var_result);    \
        if (0 == var_result) {    \
            var_val = SetU8InU16(LSB_POSS, var_val, vl_local);  \
        }   \
    } while (0);

#define DTH_SET_H8(elem, value, var_val, var_result)    \
    do {    \
        u16 vl_local;   \
        DTH_SET_UNSIGNED(elem, value, vl_local, var_result);    \
        if (0 == var_result) {    \
            var_val = SetU8InU16(MSB_POSS, var_val, vl_local);  \
        }   \
    } while (0);

#define DTH_GET_L8(var_val, elem, value, var_result)    \
    do {    \
        u8 vl_local = GetU8InU16(LSB_POSS, var_val);    \
        DTH_GET_UNSIGNED(vl_local, elem, value, var_result); \
    } while (0);

#define DTH_GET_H8(var_val, elem, value, var_result)    \
    do {    \
        u8 vl_local = GetU8InU16(MSB_POSS, var_val);    \
        DTH_GET_UNSIGNED(vl_local, elem, value, var_result);    \
    } while (0);


/**
 * Build and Send ISI message needed to read the result of a Start RX 2G5/3G
 * operation
 * @param[in] elem specifies the DTH element.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl3_00GetRfInfo_exec(struct dth_element *elem);

/**
 * Build and Send ISI message needed to stop a Start TX/RX 2G5/3G operation
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl3_03StopRfActivity(void);

int tatl3_02GetRfInfoParam_get(struct dth_element *elem, void *value);

int tatl3_04GetRfInfo_ParamSet(struct dth_element *elem, void *Value);

/**
 * Return INFO_GSM_BAND associated to a RF_BAND
 * @param[in] band_index a RF_BAND but not RF_NO_BAND
 * @retval INFO_GSM_BAND enum corresponding to band_index or INFO_NO_GSM if none
 */
u32 tatl3_04GetGsmRfBand(int band_index);

/**
 * Return RF_BAND associated to a INFO_GSM_BAND
 * @param[in] rf_band a INFO_GSM_BAND
 * @retval one of RF_BAND enum values or RF_NO_BAND if bad INFO_GSM_BAND or not
 * associated
 */
int tatl3_05GetGsmRfBandIndex(u32 rf_band);

/**
 * Return INFO_WCDMA_BAND associated to a RF_BAND
 * @param[in] band_index a RF_BAND but not RF_NO_BAND
 * @retval INFO_WCDMA_BAND enum corresponding to band_index or INFO_NO_WCDMA if
 * none
 */
u32 tatl3_06GetWcdmaRfBand(int band_index);

/**
 * Return RF_BAND associated to a INFO_WCDMA_BAND
 * @param[in] rf_band a INFO_WCDMA_BAND
 * @retval one of RF_BAND enum values or RF_NO_BAND if bad INFO_WCDMA_BAND or
 * not associated
 */
int tatl3_07GetWcdmaRfBandIndex(u32 rf_band);

/** 
 * Build and Send ISI message needed to force stop a Start TX/RX 2G5/3G
 * operation
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl3_08ForceStopRfActivity(void);

/**
 * Return a valid monitoring channel for a GSM band and channel
 * The gap between channel and the monitoring channel is ever more or equal to
 * 10. The monitoring channel is bounded as follows:
 * GSM850: 128 to 251
 * GSM900: 0 to 124 or 975 to 1023
 * GSM1800: 512 to 885
 * GSM1900: 512 to 810
 * @param[in] rf_band a valid INFO_GSM_BAND
 * @param[in] channel a valid channel for the GSM band
 * @retval 0 if no monitoring channel could be defined else a positive
 * monitoring channel between the bounds defined for \a rf_band
 */
u16 tatl3_09GetGsmMonChannel(u32 rf_band, u16 channel);

/** 
 * Return the band number corresponding to a INFO_BAND_GSM value. 850 for
 * INFO_GSM850, 900 for INFO_GSM900 and so on.
 * @param[in] vp_rfBand INFO_GSM_BAND value.
 * @return if vp_rfBand INFO_NO_GSM then 0 is returned, if it is another valid
 * INFO_GSM_BAND the corresponding band number is returned else -1 is return
 * indicating an error.
 */
int tatl3_10GetGsmBandNumber(u32 vp_rfBand);

/** 
 * Return the band number corresponding to a INFO_BAND_WCDMA value. 1 for
 * INFO_WCDMA_FDD_BAND1, 2 for INFO_WCDMA_FDD_BAND2 and so on.
 * @param[in] vp_rfBand INFO_WCDMA_BAND value.
 * @return if vp_rfBand INFO_NO_WCDMA then 0 is returned, if it is another valid
 * INFO_WCDMA_BAND the corresponding band number is returned else -1 is return
 * indicating an error.
 */
int tatl3_11GetWcdmaBandNumber(u32 vp_rfBand);

#endif /* MISC_H_ */
