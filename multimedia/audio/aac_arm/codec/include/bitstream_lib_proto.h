/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/****************************************************************/
/* 																*/ 
/* 				LIBRAIRIE FOR BIT MANIPULATION					*/
/* 																*/ 
/****************************************************************/

#ifndef _bitstream_lib_proto_h_
#define _bitstream_lib_proto_h_


#define mask16 0xffffu
#define mask24 0xffffffu

// CAUTION: buf_end = buf_add + buf_size !!! 
// ==> buf_end points on the first memory cell after the end of the buffer.

// CAUTION: the pointers are always left on the next available location and not on the last used one.

/**************************************************************/
// Word lenth for ARM is fixed and equals to 32 bit
// The ARMWORDSIZE = 32 is defined to avoid use of word_size
// variable used in MMDSP
#define ARMWORDSIZE 32
// The required shift to calculate  mod(ARMWORDLENGTH)
#define ARMWORDSIZE_BITS 5

// variable used in ARM
#define ARMWORDSIZE_8 8
// The required shift to calculate  mod(ARMWORDLENGTH)
#define ARMWORDSIZE_BITS_8 3
extern const int WORD_SIZE_BYTES[4];
/**************************************************************/

#include "audiolibs_common.h"


/****************************************************************/
/* Typedef used to locate the position of a bit in RAM,			*/
/* - buf_add  : address of the begining of the buffer           */
/* - buf_end  : address of the end of the buffer                */
/* - pos_ptr  : current address (to read or write)              */
/* - buf_size : size of the buffer in WORD                      */
/* - nbit_available : number of bits available in the current   */
/*   word pointed by pos_ptr									*/
/* - word_size : (16/24) DSP running mode 						*/
/****************************************************************/


typedef struct BS_STRUCT_T
{
	unsigned int *buf_add;
	unsigned int *buf_end;
	unsigned int *pos_ptr;
	int buf_size;
	int nbit_available;
	int word_size;
}BS_STRUCT_T;


/****************************************************************/
/* BS_INIT                                                      */
/****************************************************************/
/* initialize structure                                         */
/* IN  : - put_struct: pointer on bs_struct to initialize		*/
/*       - buf_add: pointer on buffer start adress              */
/*       - buf_size: size of buffer in WORD                     */
/*       - word_size: the DSP is 16 or 24 bits wide				*/
/* OUT : no                                                     */
/****************************************************************/
extern CPPCALL AUDIO_API_IMPORT void bs_init(BS_STRUCT_T *bs_struct, 
		unsigned int *buf_add, 
		unsigned int buf_size, 
		unsigned int word_size);  
extern CPPCALL AUDIO_API_IMPORT void bs_init_8b(BS_STRUCT_T *bs_struct, 
		unsigned int *buf_add, 
		unsigned int buf_size, 
		unsigned int word_size);
/****************************************************************/
/* INIT_BUFFER                                                  */
/****************************************************************/
/* initialize structure and reset the buffer content            */
/* IN  : - bs_struct: pointer on bs_struct to initialize		*/
/*       - buf_add: pointer on buffer structure                 */
/*       - buf_size: size of buffer in WORD                     */
/* OUT : no                                                     */
/****************************************************************/
extern CPPCALL AUDIO_API_IMPORT void bs_init_buffer(BS_STRUCT_T *bs_struct, 
		unsigned int *buf_add, 
		unsigned int buf_size, 
		unsigned int word_size);  
extern CPPCALL AUDIO_API_IMPORT void bs_init_buffer_8b(BS_STRUCT_T *bs_struct, 
		unsigned int *buf_add, 
		unsigned int buf_size, 
		unsigned int word_size); 

/****************************************************************/
/* RESET                                                 		*/
/****************************************************************/
/* set pointers at the begining of the buffer					*/
/* IN  : - bs_struct: pointer on struct BS_STRUCT_T to 			*/
/* initialize													*/
/* OUT : no                                                     */
/****************************************************************/
extern CPPCALL AUDIO_API_IMPORT void bs_reset(BS_STRUCT_T *bs_struct);
extern CPPCALL AUDIO_API_IMPORT void bs_reset_8b(BS_STRUCT_T *bs_struct);
/****************************************************************/
/* RESET BUFFER                                            		*/
/****************************************************************/
/* set pointers at the begining of the buffer 					*/
/* and erase the buffer content									*/
/* IN  : - bs_struct: pointer on struct BS_STRUCT_T to 			*/
/* 		 initialize												*/
/*																*/
/* OUT : no                                                     */
/****************************************************************/
extern CPPCALL AUDIO_API_IMPORT void bs_reset_buffer(BS_STRUCT_T *bs_struct); 
extern CPPCALL AUDIO_API_IMPORT void bs_reset_buffer_8b(BS_STRUCT_T *bs_struct); 

/****************************************************************/
/* BS READ WORD 24bits                                          */
/****************************************************************/
/* Read a word from buffer										*/
/* IN  : - bs_struct: pointer on BS_STRUCT_T                   	*/
/* OUT : Return read word (24 first bits only)                  */
/* CAUTION : 	- the buffer has to be word aligned				*/
/* 																*/
/****************************************************************/
extern CPPCALL int bs_read_word_24bits(BS_STRUCT_T *bs_struct);

/****************************************************************/
/* BS READ WORD                                                 */
/****************************************************************/
/* Read a word from buffer										*/
/* IN  : - bs_struct: pointer on BS_STRUCT_T                   	*/
/* OUT : Return read word                                       */
/* CAUTION : 	- the buffer has to be word aligned				*/
/* 				- the returned word is sign-extended			*/
/****************************************************************/
extern CPPCALL int bs_read_word_24b(BS_STRUCT_T *bs_struct);

/****************************************************************/
/* READ_BYTE	                                                */
/****************************************************************/
/* CAUTION : the pos_ptr is supposed to be byte aligned			*/
/* 												                */
/* Read one byte from the structure				                */
/*																*/
/* IN  : bs_struct: pointer on BS_STRUCT		      		    */
/* OUT : return read byte                                       */
/****************************************************************/
extern CPPCALL AUDIO_API_IMPORT unsigned int bs_read_byte_24b(BS_STRUCT_T *bs_struct);
extern CPPCALL AUDIO_API_IMPORT unsigned int bs_read_byte_32b(BS_STRUCT_T *bs_struct);
extern CPPCALL AUDIO_API_IMPORT unsigned int bs_read_byte_8b(BS_STRUCT_T *bs_struct);
/****************************************************************/
/*  BS_READ_BIT                                                */
/****************************************************************/
/* read nbit bits from the structure		                    */
/* max nbits : 24 bits or 32 bits with _arm                                          */
/* IN  : - bs_struct: pointer on struct BS_STRUCT_T             */
/*       - nbit: number of bits read in buffer                  */
/* OUT : return value of nbits of bs_struct read at the  	  	*/
/*       current address										*/                
/* CAUTION : 	when an entire word is read, the pos_ptr is		*/
/* 				incremented and nbit_available is updated 		*/
/* 				automaticaly									*/ 
/* READING ORDER for a word (ABC) composed by 3 bytes A,B,C		*/	
/* 	word1 : xxA													*/ 
/* 	word2 : xxB													*/ 
/* 	word3 : xxC													*/ 
/****************************************************************/
extern CPPCALL AUDIO_API_IMPORT unsigned int bs_read_bit_24b(BS_STRUCT_T *bs_struct, int nbit); 
extern CPPCALL AUDIO_API_IMPORT unsigned int bs_read_bit_32b(BS_STRUCT_T *bs_struct, int nbit);
extern CPPCALL AUDIO_API_IMPORT unsigned int bs_read_bit_8b(BS_STRUCT_T *bs_struct, int nbit);
extern CPPCALL AUDIO_API_IMPORT unsigned int bs_read_bit_rev_32b(BS_STRUCT_T *bs_struct, int nbit);
/****************************************************************/
/*  BS_READ_1BIT                                                */
/****************************************************************/
/* read nbits from the structure		                        */
/* IN  : - bs_struct: pointer on BS_STRUCT_T				*/
/* OUT : return 1 bit											*/
/*																*/                
/* CAUTION : 	when an entire word is read, the pos_ptr is		*/
/* 				incremented and nbit_available is updated 		*/
/* 				automaticaly									*/ 
/* READING ORDER for a word (ABC) composed by 3 bytes A,B,C		*/	
/* 	word1 : xxA													*/ 
/* 	word2 : xxB													*/ 
/* 	word3 : xxC													*/ 
/****************************************************************/
extern CPPCALL unsigned int bs_read_1bit_24b(BS_STRUCT_T *bs_struct); 

/****************************************************************/
/*  BS_READ_BIT_LONG                                            */
/****************************************************************/
/* read nbits from the structure		                        */
/* IN  : - bs_struct: pointer on BS_STRUCT_T					*/
/* OUT : - return nbits bits									*/
/*																*/                
/* CAUTION : function is identical to BS_READ_BIT, but allows 
 * to get up to 24 bits in fifo_mode_16 too						*/ 
/* 																*/ 
/* 																*/ 
/****************************************************************/
extern CPPCALL unsigned int bs_read_bit_long_24b(BS_STRUCT_T *bs_struct, int nbit);

/****************************************************************/
/* BS_WRITE_WORD	                                            */
/****************************************************************/
/* Write 1 word in the structure					  		    */
/* structure must be aligned by word 	                        */
/* IN  : - value: value to put in the structure                 */
/*       - bs_struct: pointer on BS_STRUCT_T        			*/
/* OUT : no                                                     */
/****************************************************************/
extern CPPCALL void bs_write_word_24b(BS_STRUCT_T *bs_struct,unsigned int word);

/****************************************************************/
/* WRITE 1 BYTE ALIGN                                           */
/****************************************************************/
/* Write 1 byte from structure (at pos_ptr location)            */
/* IN  : bs_struct BS_STRUCT_T			                */
/* OUT : 				                                        */
/*																*/
/* CAUTION : structure must be aligned by byte                  */
/****************************************************************/
extern CPPCALL AUDIO_API_IMPORT void bs_write_byte_24b(BS_STRUCT_T *bs_struct,unsigned int byte); 
extern CPPCALL AUDIO_API_IMPORT void bs_write_byte_32b(BS_STRUCT_T *bs_struct, unsigned int byte);
extern CPPCALL AUDIO_API_IMPORT void bs_write_byte_8b(BS_STRUCT_T *bs_struct, unsigned int byte);

/****************************************************************/
/* BS_WRITE_BIT                                                */
/****************************************************************/
/* Write nbit bits inside the buffer		 			 		*/
/* at the adress pointed by pos_ptr                             */
/* The nbit inside value are MSB-first & right-aligned. (Yet it	*/
/* is not a problem if value have more than nbit non zeros bits)*/
/* !! nbit max is 24                                            */
/* !! end of the current word is erase                          */
/* IN  : - bs_struct pointer on BS_STRUCT_T         		*/
/*       - value: data to put in structure                      */
/*       - nbit: number of bits of "value" to consider          */
/*         (aligned on the right)                               */
/* OUT : no 			                                        */
/*			                                                    */
/* FILLING ORDER for packets A, then B, then C: 				*/	
/* 	word1 : Axx													*/ 
/* 	word2 : ABx													*/ 
/* 	word3 : ABC													*/ 
/****************************************************************/
extern CPPCALL void bs_write_less_than_16_bit_24b(BS_STRUCT_T *bs_struct,unsigned int value, int nbit); 
extern CPPCALL AUDIO_API_IMPORT void bs_write_bit_24b(BS_STRUCT_T *bs_struct,unsigned int value, int nbit); 
extern CPPCALL void bs_write_less_than_16_bit_32b(BS_STRUCT_T *bs_struct,unsigned int value, int nbit); 
extern CPPCALL AUDIO_API_IMPORT void bs_write_bit_32b(BS_STRUCT_T *bs_struct,unsigned int value, int nbit); 
extern CPPCALL AUDIO_API_IMPORT void bs_write_bit_8b(BS_STRUCT_T *bs_struct,unsigned int value, int nbit);

/****************************************************************/
/* BS_LOOKUP_WORD_SIZE_BIT                                    	*/
/****************************************************************/
/* Read word_size (either 16 or 24) bits in the structure		*/
/* at the current address without updating the pointers         */
/* 											              		*/
/* IN  : - bs_struct pointer on BS_STRUCT_T         		*/
/* OUT : - word_size bits                                       */
/****************************************************************/
extern CPPCALL unsigned int bs_lookup_word_size_bit_24b(BS_STRUCT_T *bs_struct);
extern CPPCALL unsigned int bs_lookup_word_size_bit_32b(BS_STRUCT_T *bs_struct);
/****************************************************************/
/* BS_READ_SAMPLE_MSB				                            */
/****************************************************************/
/* Read 16 bits MSB from the structure at the current address	*/ 
/* only the 16 MSbits are considered       						*/
/*																*/
/* IN  : - bs_struct pointer on BS_STRUCT_T             	*/
/* OUT : - 16 MSB bits		                                    */
/* EXAMPLE : input = ABC  (A,B,C are bytes) ==> output = 0AB	*/
/****************************************************************/
extern CPPCALL unsigned int bs_read_sample_msb_24b(BS_STRUCT_T *bs_struct); 

/****************************************************************/
/* BS_READ_SAMPLE_LSB				                            */
/****************************************************************/
/* Read 16 bits LSB from the structure at the current address	*/ 
/* only the 16 LS-bits are considered      						*/
/*																*/
/* IN  : - bs_struct pointer on BS_STRUCT_T             	*/
/* OUT : - 16 LSB bits		                                    */
/* EXAMPLE : input = ABC  (A,B,C are bytes) ==> output = 0BC	*/
/****************************************************************/
extern CPPCALL unsigned int bs_read_sample_lsb_24b(BS_STRUCT_T *bs_struct);

/****************************************************************/
/* BS_COPY_BIT					                                */
/****************************************************************/
/* copy nbit bits from the structure1 into structure 2			*/ 
/*																*/
/* IN  : - BS_STRUCT_T *bs_struct1, int nbit             */
/* OUT : - BS_STRUCT_T *bs_strcut2		                */
/****************************************************************/
extern CPPCALL void bs_copy_bit_24b(BS_STRUCT_T *bs_struct1,
		BS_STRUCT_T *bs_struct2,
		int nbit);
extern CPPCALL AUDIO_API_IMPORT void bs_copy_bit_32b(BS_STRUCT_T * bs_struct1,
        BS_STRUCT_T * bs_struct2,
        int nbit) ;
extern CPPCALL AUDIO_API_IMPORT void bs_copy_bit_8b(BS_STRUCT_T * bs_struct1,
        BS_STRUCT_T * bs_struct2,
        int nbit) ;
/****************************************************************/
/* BS_COPY_BYTE					                                */
/****************************************************************/
/* copy nbyte bytes from the structure1 into structure 2		*/ 
/*																*/
/* IN  : - BS_STRUCT_T *bs_struct1, int nbyte             		*/
/* OUT : - BS_STRUCVT_T *bs_struct2		                        */
/* CAUTION : no check performed concerning                      */
/* 			 the lenght of the structures                       */
/****************************************************************/
extern CPPCALL void bs_copy_byte_24b(BS_STRUCT_T *bs_struct1,
		BS_STRUCT_T *bs_struct2,
		int nbyte); 
extern CPPCALL void bs_copy_byte_32b(BS_STRUCT_T *bs_struct1,
		BS_STRUCT_T *bs_struct2,
		int nbyte); 
extern CPPCALL void bs_copy_byte_8b(BS_STRUCT_T *bs_struct1,
		BS_STRUCT_T *bs_struct2,
		int nbyte); 
/****************************************************************/
/* BS_COPY_WORD					                                */
/****************************************************************/
/* copy nword words from the structure1 into structure 2		*/ 
/*																*/
/* IN  : - BS_STRUCT_T *bs_struct1, int nword             		*/
/* OUT : - BS_STRUCT_T *bs_strcut2		                        */
/* CAUTION : - no check performed concerning                    */
/* 			 the lenght of the structures                       */
/* 			 - the structures are not supposed to be word 		*/
/*			aligned         									*/
/* 			 - in the destination structure, the previous 		*/
/*			values are overwritten wordwise without check		*/
/*			 - the number of copied word is based on			*/
/*			the source buffer 									*/
/*			(may be different if word size are different)		*/ 
/****************************************************************/
/****************************************************************/
extern CPPCALL void bs_copy_word_24b(BS_STRUCT_T *bs_struct1,
		BS_STRUCT_T *bs_struct2,
		int nword); 
extern CPPCALL void bs_copy_word_32b(BS_STRUCT_T *bs_struct1,
		BS_STRUCT_T *bs_struct2,
		int nword); 

/****************************************************************/
/* BS_GET_PNTR_POSITION			                                */
/****************************************************************/
/* return the pos_ptr from the structure						*/ 
/*																*/
/* IN  : - BS_STRUCT_T *bs_struct			          	*/
/* OUT : - pointer on bs_Struct->pos_ptr			            */
/****************************************************************/
extern CPPCALL unsigned int * bs_get_pntr_position(BS_STRUCT_T *bs_struct); 

/****************************************************************/
/* BS_FAST_MOVE_WORD			                                */
/****************************************************************/
/* update the pointers for a nword words move					*/ 
/*																*/
/* IN  : - bs_struct pointer on BS_STRUCT_T				*/
/*		 - int nword : number of words to move					*/
/* OUT : - void										            */
/****************************************************************/
extern CPPCALL void bs_fast_move_word_24b(BS_STRUCT_T *bs_struct, int nword); 
extern CPPCALL void bs_fast_move_word_32b(BS_STRUCT_T *bs_struct, int nword); 
/****************************************************************/
/* BS_FAST_MOVE_BYTE			                                */
/****************************************************************/
/* update the pointers for a nbyte bytes move					*/ 
/* CAUTION : the bytes are supposed to be aligned				*/
/*																*/
/* IN  : - bs_struct pointer on BS_STRUCT_T				*/
/*		 - int nbyte : number of bytes to move					*/
/* OUT : - void										            */
/****************************************************************/
extern CPPCALL void bs_fast_move_byte_24b(BS_STRUCT_T *bs_struct, int nbyte); 
extern CPPCALL void bs_fast_move_byte_32b(BS_STRUCT_T *bs_struct, int nbyte); 
extern CPPCALL void bs_fast_move_byte_8b(BS_STRUCT_T *bs_struct, int nbyte);
/****************************************************************/
/* BS_FAST_MOVE_BIT				                                */
/****************************************************************/
/* update the pointers for a nbit bits move						*/ 
/*																*/
/* IN  : - bs_struct pointer on BS_STRUCT_T				*/
/*		 - int nbit : number of bits to move					*/
/* OUT : - void										            */
/****************************************************************/
extern CPPCALL AUDIO_API_IMPORT void bs_fast_move_bit_24b(BS_STRUCT_T *bs_struct, int nbit); 
extern CPPCALL AUDIO_API_IMPORT void bs_fast_move_bit_32b(BS_STRUCT_T *bs_struct,
        int nbit);
extern CPPCALL AUDIO_API_IMPORT void bs_fast_move_bit_8b(BS_STRUCT_T *bs_struct,
        int nbit);

/****************************************************************/
/* BS_WRITE_ARRAY                                               */
/****************************************************************/
/* This function read len (value and nbit of this value) to put */
/* in the structure.                                            */
/* The nbit inside value are MSB-first & right-aligned. (Yet it	*/
/* is not a problem if value have more than nbit non zeros bits)*/
/* !! nbit maxi is 24 bits.                                     */
/* !! end of the current word is erase                          */
/* IN  : - put_struct: pointer on BS_STRUCT_T           	*/
/*       - buffer: pointer on array of data to put in structure */
/*       - codeLen: pointer on array of nbit of data            */
/*       - len: number of the couple(value, nbit) in buffer     */
/* OUT : return number of bits set in put_struct                */
/****************************************************************/
extern CPPCALL unsigned int bs_write_array_24b(BS_STRUCT_T *bs_struct, 
		unsigned int *buffer, 
		unsigned int *bitfield_length, 
		unsigned int len,
		unsigned int offset);

/****************************************************************/
/* BS_CHECK_STRUCT                                              */
/****************************************************************/
/* This fonction checks if the pos_ptr is located in            */
/* the interval [buf_add,buff_end] and eventually corrects this */
/* by adding/subtracting the size of the interval (modulo move) */
/* IN  : - bs_struct: pointer on BS_STRUCT_T            	*/
/* OUT : - no (bs_struct may be modified)						*/
/****************************************************************/
//extern CPPCALL void bs_check_struct(BS_STRUCT_T *bs_struct);

/****************************************************************/
/* BS_DELTA_BIT	                                            	*/
/****************************************************************/
/* This fonction calculates the number of bits between the		*/
/* positions of the pos_ptr of 2 structures			            */
/* Performs difference : structure "1" - structure "2".			*/
/* the interval [buf_add,buff_end] and eventually corrects this */
/* by adding/subtracting the size of the interval (modulo move) */
/* IN  : - bs_struct1, bs_struct2 : pointer on BS_STRUCT_T*/
/* OUT : - nbit : abs of number of bits between the pointers	*/
/****************************************************************/
extern CPPCALL AUDIO_API_IMPORT int bs_delta_bit(BS_STRUCT_T *bs_struct1, 
		BS_STRUCT_T *bs_struct2);
extern CPPCALL AUDIO_API_IMPORT int bs_delta_bit_8b(BS_STRUCT_T *bs_struct1, 
		BS_STRUCT_T *bs_struct2);
/****************************************************************/
/* BS_GET_BUF_ADD 												*/
/****************************************************************/
/* This function returns the address of the memory location		*/ 
/* in use                                                       */ 
/****************************************************************/
extern CPPCALL unsigned int *bs_get_buf_add (BS_STRUCT_T *bs_struct);

/****************************************************************/
/* BS_CHECK_BYTE_ALIGNED_STRUCT									*/
/****************************************************************/
/* This function returns the number of bits to be written 		*/
/* at pos_ptr position to be byte aligned						*/ 
/* 		                                                        */ 
/****************************************************************/
extern CPPCALL int bs_byte_align_struct(BS_STRUCT_T * bs_struct);

/****************************************************************/
/* BS_CHECK_BYTE_ALIGNED 										*/
/****************************************************************/
/* This fonction makes sure that the difference in bits between */
/* two pointers is a multiple of 8 bits. This function          */
/* implements the following calls:                              */
/* 		                                                        */ 
/****************************************************************/
extern CPPCALL void bs_byte_align_24b(BS_STRUCT_T * bs_struct1, BS_STRUCT_T * bs_struct2);
extern CPPCALL void bs_byte_align_32b(BS_STRUCT_T * bs_struct1, BS_STRUCT_T * bs_struct2);

/****************************************************************/
/* BS_CHECK_WORD_ALIGNED 										*/
/****************************************************************/
/* This function returns the number of bits to be written 		*/
/* at pos_ptr position to be word aligned						*/ 
/* 		                                                        */ 
/****************************************************************/
extern CPPCALL unsigned int bs_word_align_24b(BS_STRUCT_T * bs_struct);

/****************************************************************/
/* BS_BIT_POINTER_POSITION										*/
/****************************************************************/
/* IN  : - bs_struct: pointer on BS_STRUCT_T              		*/
/* OUT : return number of bits available at current address		*/
/****************************************************************/
extern CPPCALL unsigned int bs_bit_pointer_position(BS_STRUCT_T *bs_struct);

/****************************************************************/
/* BS_WORD_POINTER_POSITION										*/
/****************************************************************/
/* IN  : - bs_struct: pointer on BS_STRUCT_T              		*/
/* OUT : - return the adress of the memory location in use		*/
/****************************************************************/
extern CPPCALL unsigned int * bs_word_pointer_position(BS_STRUCT_T *bs_struct);


/* these functions are called from 16-bit dcu mode. They act as
 * wrappers, perform the appropriate sign extension and handle the
 * mode change */

extern CPPCALL int bs_delta_bit_16(
	BS_STRUCT_T *bs_struct1,
	BS_STRUCT_T *bs_struct2);

extern CPPCALL unsigned int bs_read_byte_16(BS_STRUCT_T *bs_struct);

extern CPPCALL void bs_write_byte_16(BS_STRUCT_T *bs_struct,unsigned int val);

extern CPPCALL void bs_init_buffer_16(BS_STRUCT_T *bs_struct, 
									  unsigned int *buf_add, 
									  unsigned int buf_size, 
									  unsigned int word_size);


/****************************************************************/
/* BS_COPY_WORD_MODULO_DEST										*/
/****************************************************************/
/* copy nword words from a table into a BS_STRUCT				*/ 
/*																*/
/* IN  : - unsigned int * ptr, unsigned int nword          		*/
/* 		 - BS_STRUCT_T *bs_struct								*/
/* OUT : - none													*/	
/*																*/	
/* CAUTION 	: manipulated struct are supposed to be 24 bits		*/
/* 			: words are supposed to be aligned					*/
/* 			: no check on length of buffer to be copied			*/
/*																*/
/****************************************************************/
extern CPPCALL void bs_copy_word_modulo_dest_24b(unsigned int * ptr, unsigned int nword,BS_STRUCT_T *bs_struct);

/****************************************************************/
/* BS_COPY_WORD_MODULO_SOURCE									*/
/****************************************************************/
/* copy nword words from a table into a BS_STRUCT				*/ 
/*																*/
/* IN  : 	- BS_STRUCT_T *bs_struct							*/
/* 			- unsigned int * ptr, unsigned int nword          	*/
/* OUT : - none													*/	
/*																*/	
/* CAUTION 	: manipulated struct are supposed to be 24 bits		*/
/* 			: words are supposed to be aligned					*/
/* 			: no check on length of buffer to be copied			*/
/*																*/
/****************************************************************/
extern CPPCALL void bs_copy_word_modulo_source_24b(BS_STRUCT_T *bs_struct,unsigned int *ptr, unsigned int nword);

/****************************************************************/
/* BS_COPY_SAMPLES				                                */
/****************************************************************/
/* copy 16bits samples from the structure1 into a structure 2	*/ 
/*																*/
/*																*/
/* IN	: - BS_STRUCT_T *bs_struct1				          		*/
/*		: - int format1 (16 or 24)								*/
/*		: - int format2	(16 or 24)		                        */
/*		: - int nsamples				                        */
/* OUT : - BS_STRUCT_T *bs_struct2		                        */
/*																*/
/* Format1 == 16 and Format2 == 24								*/
/*Word 	0AB		==>  	AB0										*/
/*		0CD				CD0										*/
/*																*/
/* Format1 == 24 and Format1 == 16								*/
/*Word 	ABC		==>  	0A'B'	A'B' = round16(ABC)				*/
/*		DEF				0D'E'	D'E' = round16(DEF)				*/
/*																*/
/****************************************************************/
extern CPPCALL void bs_copy_samples_24b(BS_STRUCT_T *bs_struct1, 
		int format1,
		BS_STRUCT_T *bs_struct2,
		int format2,
		int nsamples); 


/****************************************************************/
/* BS_COPY_FROM_ARRAY			                                */
/****************************************************************/
/* copy 16bits samples from array into a BS_STRUCT				*/ 
/*																*/
/*																*/
/* IN	: - circular array seen internally as BS_STRUCT    		*/
/*			(BS_STRUCT_T *bs_struct1)							*/
/*		: - unsigned int nsamples		                        */
/*		: - int msb_lsb to define the format	                */
/*			(msb_lsb == 0  ==> LSB)								*/
/*			(msb_lsb == 1  ==> MSB)								*/
/*																*/
/* OUT 	: - BS_STRUCT_T *bs_struct2		                        */
/*																*/
/*																*/
/****************************************************************/
extern CPPCALL void bs_copy_from_array_24b(BS_STRUCT_T *bs_struct1, 
		BS_STRUCT_T *bs_struct2,
		unsigned int nsamples,
		unsigned int msb_lsb);

/****************************************************************/
/* BS_COPY_TO_ARRAY			                                	*/
/****************************************************************/
/* copy 16bits samples from a BS_STRUCT	into an array			*/ 
/*																*/
/*																*/
/* IN	: - BS_STRUCT_T *bs_struct1		                        */
/*		: - unsigned int nsamples				                */
/*		: - int msb_lsb to define the format	                */
/*			(msb_lsb == 0  ==> LSB)								*/
/*			(msb_lsb == 1  ==> MSB)								*/
/*																*/
/* OUT	: - circular array seen internally as BS_STRUCT    		*/
/*			(BS_STRUCT_T *bs_struct2)							*/
/*																*/
/*																*/
/****************************************************************/
extern CPPCALL void bs_copy_to_array_24b(BS_STRUCT_T *bs_struct1, 
		BS_STRUCT_T *bs_struct2,
		unsigned int nsamples,
		unsigned int msb_lsb);

#endif
