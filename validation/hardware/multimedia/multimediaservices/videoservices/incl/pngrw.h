/*****************************************************************************/
/**
*  Copyright (C) ST-Ericsson 2009
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Propose png read and write functions
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef __PNGRW_H__
#define __PNGRW_H__

int32_t write_png(char *output_filename, 
		uint8_t *input, 
		uint32_t input_width, uint32_t input_height, 
		uint32_t input_bpp);

int32_t read_png(char *input_filename,  
		uint8_t *output, 
		uint32_t output_width, uint32_t output_height, 
		uint32_t output_bpp);

#endif /* __PNGRW_H__ */
