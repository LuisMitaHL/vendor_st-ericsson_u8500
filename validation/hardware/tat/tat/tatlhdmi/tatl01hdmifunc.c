/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides services for power services module
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include "tatlhdmi.h"
#include <stdlib.h>

#define SIZE_FILE_KSV		  8
#define SIZE_FILE_KEYS		280
#define SIZE_FILE_ENC_PAD	  8
#define SIZE_FILE_CRC32		  4
#define SIZE_FILE_PAD		  4

uint8_t v_tatlhdmi_hdmicmdbuf[BUFFERMAX];
char *p_tatlhdmi_IO[HDMI_ELEMENT_COUNT] = {NULL};
uint8_t v_tatlhdmi_IO[HDMI_ELEMENT_COUNT] = {0};

/*----------------------------------------------------------------------------*
* Procedure    : tatlx1_09NextCrc
*-----------------------------------------------------------------------------*
* Description  : Calcul Cyclic Redundancy Check
*-----------------------------------------------------------------------------*
* Parameter(s) :
*-----------------------------------------------------------------------------*
* Return Value : uint8_t, CRC
*----------------------------------------------------------------------------*/
static uint8_t tatlx1_09_NextCrc(uint8_t din, uint8_t crc)
{
	uint8_t newCRC = 0x00;
	if (din > 0)
		newCRC |= (0x80 ^ (crc & 0x80)) >> 7;	/*(0) xor */
	else
		newCRC |= (0x00 ^ (crc & 0x80)) >> 7;	/*(0) xor */

	newCRC &= ~0x02;
	newCRC |= (crc & 0x01) << 1;	/*(1) */
	newCRC &= ~0x04;
	newCRC |= ((crc & 0x02) << 1) ^ ((crc & 0x80) >> 5);	/*(2) xor */
	newCRC &= ~0x08;
	newCRC |= ((crc & 0x04) << 1) ^ ((crc & 0x80) >> 4);	/*(3) xor */
	newCRC &= ~0x10;
	newCRC |= ((crc & 0x08) << 1) ^ ((crc & 0x80) >> 3);	/*(4) xor */
	newCRC &= ~0x20;
	newCRC |= ((crc & 0x10) << 1);	/*(4) */
	newCRC &= ~0x40;
	newCRC |= ((crc & 0x20) << 1);	/*(5) */
	newCRC &= ~0x80;
	newCRC |= ((crc & 0x40) << 1);	/*(6) */
	return newCRC;
}

/*----------------------------------------------------------------------------*
* Procedure    : tatlx1_04hdmiCommandReg
*-----------------------------------------------------------------------------*
* Description  : Send Command Data
*-----------------------------------------------------------------------------*
* Parameter(s) : unsigned int mode, READ or SEND configuration
*		  hdmi_command_register *pp_hdmi_cmdreg, Structure of HDMI_CMD
*-----------------------------------------------------------------------------*
* Return Value : u32, TAT error
*----------------------------------------------------------------------------*/
static int tatlx1_04hdmiCommandReg(unsigned int mode,
			    struct hdmi_command_register *pp_hdmi_cmdreg)
{
	int vl_hdmifd = 0;
	int vl_Error = 0;

	/* open hdmi device Access  */
	vl_hdmifd = open(TATHDMI_HDMI_DEVICE_NAME, O_RDWR);
	if (vl_hdmifd < 0) {
		printf("TATHDMI ERROR : tatlx1_04hdmiCommandReg fail to open /dev/hdmi\n");
		vl_Error = ENOENT;
	} else {
		/* Send the command */
		vl_Error = ioctl(vl_hdmifd, mode, pp_hdmi_cmdreg);

		/* close hdmi device  Access */
		close(vl_hdmifd);
	}
	return vl_Error;
}

/*----------------------------------------------------------------------------*
* Procedure    : tatlx1_05_check_aes_key_otp
*-----------------------------------------------------------------------------*
* Description  : Check OTP type
*-----------------------------------------------------------------------------*
* Parameter(s) :
*-----------------------------------------------------------------------------*
* Return Value : u32, TAT error
*----------------------------------------------------------------------------*/
static int tatlx1_05_check_aes_key_otp()
{
	int vl_Error = TAT_ERROR_OFF;
	int vl_hdmifd = 0;
	uint8_t check_result = 0;

	vl_hdmifd = open(TATHDMI_HDMI_DEVICE_NAME, O_RDWR);
	if (vl_hdmifd < 0) {
		printf("TATHDMI ERROR : tatlx1_04hdmiCommandReg failed to open /dev/hdmi\n");
		vl_Error = ENOENT;
	} else {
		/* Checking if key has already been programmed. */
		if (ioctl(vl_hdmifd, IOC_HDCP_CHKAESOTP, &check_result) < 0) {
			vl_Error = -2;
			printf("TATHDMI ERROR : IOC_HDCP_CHKAESOTP failed.\n");
		} else {
			if (check_result == 0) {
				v_tatlhdmi_IO[OUT_HDCPOTPSTAT] = VIRGIN;
				printf("TATHDMI : OTP is virgin.\n");
			} else {
				v_tatlhdmi_IO[OUT_HDCPOTPSTAT] = PROGRAMMED;
				printf("TATHDMI : OTP is already programmed.\n");
			}
		}
	}
	close(vl_hdmifd);
	return vl_Error;
}

/*----------------------------------------------------------------------------*
* Procedure    : tatlx1_06_fuse_aes_otp_key
*-----------------------------------------------------------------------------*
* Description  : Write OTP
*-----------------------------------------------------------------------------*
* Parameter(s) :
*-----------------------------------------------------------------------------*
* Return Value : u32, TAT error
*----------------------------------------------------------------------------*/
static int tatlx1_06_fuse_aes_otp_key()
{
	int vl_Error = TAT_ERROR_OFF, start = -1;
	char pl_otpdata[35] = { 0 };
	char pl_parseotp[9] = { 0 };
	uint32_t temp;
	int8_t i, k = 0, j = 0;
	uint8_t din0 = 0, din7 = 0, len = 0;
	uint8_t otp_key_crc = 0;	/* CRC initialized to 0 */
	uint32_t fused_aes_128b[4];
	uint8_t otp_key[HDMI_FUSE_KEYSIZE];

	struct hdcp_fuseaes hdmi_myfuseaes;
	int vl_hdmifd;

	v_tatlhdmi_IO[OUT_HDCPOTPPROSTAT] = BADINPUTCMD;

	strncpy(pl_otpdata, p_tatlhdmi_IO[IN_HDCPOTPPRO], strlen(p_tatlhdmi_IO[IN_HDCPOTPPRO]));
	len = strlen(pl_otpdata);
	/*check if data is 128 bits */
	if ((((pl_otpdata[1] == 'x') || (pl_otpdata[1] == 'X')) && pl_otpdata[0] == '0') && len == 34)
		start = 2;
	else if (len == 32)	/*in case 0x is missing */
		start = 0;

	if (start >= 0) {
		/*save and swapp OTP */
		for (i = 0; i < 4; i++) {
			strncpy(pl_parseotp, pl_otpdata + start + j, 8);
			temp = (uint32_t) strtoul(pl_parseotp, NULL, 16);

			fused_aes_128b[i] =
				(((temp & 0xFF000000) >> 24) |
			     ((temp & 0x00FF0000) >> 8) |
				 ((temp & 0x0000FF00) << 8) |
				 ((temp & 0x000000FF) << 24));
			j = j + 8;

			memcpy((otp_key) + k, &fused_aes_128b[i], 4);
			k = k + 4;
		}

		printf("TATHDMI : key_data sent to hdmi driver: ");
		for (i = 0; i < HDMI_FUSE_KEYSIZE; i++)
			printf("%x", otp_key[i]);

		printf("\n");

		/* Calculation of expected CRC value ([ndlr] 128-bit antifusekey) */
		for (i = 15; i > -1; i--) {
			if (i < 4)
				din0 = fused_aes_128b[0] >> (i * 8);
			if (i > 3 && i < 8)
				din0 = fused_aes_128b[1] >> ((i - 4) * 8);
			if (i > 7 && i < 12)
				din0 = fused_aes_128b[2] >> ((i - 8) * 8);
			if (i > 11 && i < 16)
				din0 = fused_aes_128b[3] >> ((i - 12) * 8);

			for (k = 0; k < 8; k++) {
				din7 = din0 & (0x01 << k);
				otp_key_crc = tatlx1_09_NextCrc(din7, otp_key_crc);
			}
		}
		printf("TATHDMI : key CRC8 sent to hdmi driver:0X%x\n", otp_key_crc);

		vl_hdmifd = open(TATHDMI_HDMI_DEVICE_NAME, O_RDWR);
		if (vl_hdmifd < 0) {
			printf("TATHDMI ERROR : tatlx1_04hdmiCommandReg failed to open /dev/hdmi\n");
			vl_Error = -1;
		} else {
			/* OTP fuse the AES key */
			for (i = 0; i < HDMI_FUSE_KEYSIZE; i++)
				hdmi_myfuseaes.key[i] = otp_key[i];

			hdmi_myfuseaes.crc = otp_key_crc;
			if (ioctl(vl_hdmifd, IOC_HDCP_FUSEAES, &hdmi_myfuseaes) < 0) {
				vl_Error = -2;
				printf("TATHDMI ERROR : IOC_HDCP_FUSEAES failed.\n");
			} else {
				switch (hdmi_myfuseaes.result) {
				case 0:
					v_tatlhdmi_IO[OUT_HDCPOTPPROSTAT] = OK;
					printf("TATHDMI : fuse operation OK.\n");
					vl_Error = 0;
					break;
				case 1:
					v_tatlhdmi_IO[OUT_HDCPOTPPROSTAT] = FAILED;
					printf("TATHDMI ERROR : fuse operation failed.\n");
					vl_Error = -3;

					break;
				case 3:
					v_tatlhdmi_IO[OUT_HDCPOTPPROSTAT] = BADINPUTCMD;
					printf("TATHDMI ERROR : CRC8 mismatch\n");
					vl_Error = -4;

					break;
				default:
					printf("TATHDMI ERROR : wrong result for IOC_HDCP_FUSEAES.\n");
					vl_Error = -5;
					break;
				}
			}
		}
		close(vl_hdmifd);
	} else {
		printf("TATHDMI ERROR : key must have 128 bits\n");
		vl_Error = -6;
	}
	return vl_Error;
}

/*----------------------------------------------------------------------------*
* Procedure    : tatlx1_08_load_and_verify_aes_keys
*-----------------------------------------------------------------------------*
* Description  : Manage HDCP key
*-----------------------------------------------------------------------------*
* Parameter(s) :
*-----------------------------------------------------------------------------*
* Return Value : u32, TAT error
*----------------------------------------------------------------------------*/
static int tatlx1_08_load_and_verify_aes_keys()
{
	int vl_Error = 0;
	int vl_result = 0;
	FILE *pl_HDCPfd;
	struct hdcp_loadaesall pl_keyset;
	int vl_hdmifd = 0;
	u8 vl_dataread[HDMI_AES_KEYSIZE];
	u8 vl_datareadksv[HDMI_AES_KSVSIZE];	/* 5 bytes */
	u8 vl_datareadcrc[HDMI_CRC32_SIZE];	/* 4 bytes */

	v_tatlhdmi_IO[OUT_HDCPAESSTAT] = FAILED;

	int i;

	printf("TATHDMI : Let's open .bin file!\n");

	/* open HDCP_key.bin File  */
	pl_HDCPfd = fopen(TATHDMI_FILE_HDCP_KEY, "r");
	if (pl_HDCPfd != NULL) {
		printf("TATHDMI : .bin file is opened!\n");
		/* Read KSV */
		vl_result = fread(vl_datareadksv, 1, SIZE_FILE_KSV, pl_HDCPfd);
		if (vl_result == SIZE_FILE_KSV) {
			memcpy(pl_keyset.ksv, vl_datareadksv, HDMI_AES_KSVSIZE);
			printf("TATHDMI : ksv = 0x");

			for (i = 0; i < HDMI_AES_KSVSIZE; i++)
				printf("%x", pl_keyset.ksv[i]);

			printf("\n");

			/* Read AES keys */
			vl_result = fread(vl_dataread, 1, SIZE_FILE_KEYS + SIZE_FILE_ENC_PAD, pl_HDCPfd);	/* HDMI_AES_KEYSIZE */
			if (vl_result == HDMI_AES_KEYSIZE) {
				memcpy(pl_keyset.key, vl_dataread, HDMI_AES_KEYSIZE);

				vl_result = fread(vl_datareadcrc, 1, HDMI_CRC32_SIZE, pl_HDCPfd);
				if (vl_result == HDMI_CRC32_SIZE) {
					memcpy(pl_keyset.crc32, vl_datareadcrc, HDMI_CRC32_SIZE);
					printf("TATHDMI : crc32 = 0x");
					for (i = 0; i < HDMI_CRC32_SIZE; i++)
						printf("%02x", pl_keyset.crc32[i]);
					printf(" (LSB first)\n");

					vl_hdmifd = open(TATHDMI_HDMI_DEVICE_NAME, O_RDWR);
					if (vl_hdmifd < 0) {
						printf("TATHDMI ERROR : tatlx1_08_load_and_verify_aes_keys failed to open /dev/hdmi\n");
						vl_Error = -5;
					} else {
						printf("TATHDMI : /dev/hdmi is opened\n");
						if (ioctl(vl_hdmifd, IOC_HDCP_LOADAES, &pl_keyset) < 0) {
							printf("TATHDMI ERROR: IOC_HDCP_LOADAES failed\n");
							vl_Error = -6;
						} else {	/* ioctl */

							switch (pl_keyset.result) {
							case HDMI_RESULT_OK:
								v_tatlhdmi_IO[OUT_HDCPAESSTAT] = OK;
								vl_Error = 0;
								printf("TATHDMI : keys are loaded, CRC32 is OK\n");
								break;
							case HDMI_AES_NOT_FUSED:
								v_tatlhdmi_IO[OUT_HDCPAESSTAT] = NOKEY;
								vl_Error = -7;
								printf("TATHDMI ERROR : OTP not fused.\n");
								break;
							case HDMI_RESULT_CRC_MISMATCH:
								v_tatlhdmi_IO[OUT_HDCPAESSTAT] = CRCMISMATCH;
								vl_Error = -8;
								printf("TATHDMI ERROR : CRC32 mismatch\n");
								break;
							case HDMI_RESULT_NOT_OK:
								v_tatlhdmi_IO[OUT_HDCPAESSTAT] = FAILED;
								vl_Error = -9;
								printf("TATHDMI ERROR : check AES failed (driver)\n");
								break;
							default:
								printf("TATHDMI ERROR : wrong result for IOC_HDCP_LOADAES : %d\n", pl_keyset.result);
								vl_Error = -10;
							}
						}
						close(vl_hdmifd);
					}	/* else from open(TATHDMI_HDMI_DEVICE_NAME, O_RDWR)) < 0 */
				} else {	/* from fread = CRC32 SIZE */

					printf("TATHDMI ERROR : Not enought data for CRC32\n");
					vl_Error = -4;
				}
			} else {	/* from fread = AES SIZE */

				printf
				    ("TATHDMI ERROR : Not enought data for AES KEYS\n");
				vl_Error = -3;
			}
		} else {	/* from fread = KSV SIZE */

			printf("TATHDMI ERROR : Not enought data for KSV\n");
			vl_Error = -2;
		}
	} else {		/* from fopen .bin */

		printf("TATHDMI ERROR : failed to read in the file .bin\n");
		vl_Error = -1;
	}

	return vl_Error;
}

/*----------------------------------------------------------------------------*
* Procedure    : tatlx1_10_send_n_register_command
*-----------------------------------------------------------------------------*
* Description  : Action to send multiple register command
*-----------------------------------------------------------------------------*
* Parameter(s) :
*-----------------------------------------------------------------------------*
* Return Value : u32, TAT error
*----------------------------------------------------------------------------*/
static int tatlx1_10_send_n_register_command()
{
	int vl_Error = TAT_ERROR_OFF;
	struct hdmi_command_register hdmi_cmdreg;
	char vl_temp[3] = { 0 };
	char *pl_outstr = NULL;
	char *pl_token = NULL;
	char *pl_temp = NULL;
	uint8_t pl_indata[BUFFERMAX] = { 0 };
	uint8_t pl_outdata[BUFFERMAX] = { 0 };
	int len = 0;
	int vl_size = 0, vl_sizebackup = 0;
	int8_t i, j = 0, k = 0, start = 0;

	/* free previous output data */
	if (p_tatlhdmi_IO[OUT_FULLCOMMAND] != NULL) {
		free(p_tatlhdmi_IO[OUT_FULLCOMMAND]);
		p_tatlhdmi_IO[OUT_FULLCOMMAND] = NULL;
	}

	/* copy p_tatlhdmi_IO[IN_FULLCOMMAND]to pl_temp to not modified p_tatlhdmi_IO[IN_FULLCOMMAND] */
	pl_temp = calloc(strlen(p_tatlhdmi_IO[IN_FULLCOMMAND]), sizeof(char));
	if (pl_temp != NULL) {

		memcpy(pl_temp, p_tatlhdmi_IO[IN_FULLCOMMAND],
		       strlen(p_tatlhdmi_IO[IN_FULLCOMMAND]));

		pl_token = strtok(pl_temp, "\n");
		while ((pl_token != NULL) && (vl_Error == TAT_ERROR_OFF)) {
			/*init */
			memset(pl_indata, 0, BUFFERMAX);
			memset(pl_outdata, 0, BUFFERMAX);
			j = 0;
			k = 0;
			i = 0;
			start = 2;	/*"O" "X" character */

			/*check if "0x" string is present in input data in order tp calculate the number of parameters */
			if ((((pl_token[1] == 'x') || (pl_token[1] == 'X')) && pl_token[0] == '0')) {
				len = (strlen(pl_token) / 2) - 1;

				/*convert string to uint8_t to fill struct AV8100_cmdreg */
				for (i = 0; i < len; i++) {
					strncpy(vl_temp, pl_token + start + j, 2);
					pl_indata[i] = (uint8_t) strtoul(vl_temp, NULL, 16);
					j = j + 2;
				}

				/*Send Command */
				hdmi_cmdreg.cmd_id = pl_indata[0];
				hdmi_cmdreg.buf_len = pl_indata[1];
				memset(hdmi_cmdreg.buf, 0, BUFFERMAX);
				memcpy(hdmi_cmdreg.buf, &pl_indata[2], hdmi_cmdreg.buf_len);
				if (0 == tatlx1_04hdmiCommandReg(IOC_HDMI_CONFIGURATION_WRITE, &hdmi_cmdreg)) {
					pl_outdata[0] = pl_indata[0];
					pl_outdata[1] = hdmi_cmdreg.return_status;
					if (hdmi_cmdreg.return_status == HDMI_RESULT_OK) {
						pl_outdata[2] = hdmi_cmdreg.buf_len;
						memcpy(&pl_outdata[3],
						       hdmi_cmdreg.buf,
						       hdmi_cmdreg.buf_len);

						/*calcul size needed of output string with \0:   sizemax=0X comID len param*len \n \0 */
						vl_sizebackup = vl_size + vl_sizebackup;
						vl_size = (pl_outdata[2] + 5) * 2;	/* buf_len*2 + 0x + cmd_id + return_status + buf_len + \n */

						p_tatlhdmi_IO[OUT_FULLCOMMAND] =
						    realloc(p_tatlhdmi_IO[OUT_FULLCOMMAND], (vl_size + vl_sizebackup) * sizeof(char));
						if (p_tatlhdmi_IO[OUT_FULLCOMMAND] != NULL) {
							if (vl_sizebackup == 0)
								memset(p_tatlhdmi_IO[OUT_FULLCOMMAND], 0, vl_size);

							pl_outstr = calloc(vl_size, sizeof(char));
							pl_outstr[0] = '0';
							pl_outstr[1] = 'x';
							k = 2;
							for (i = 0 ; i < hdmi_cmdreg.buf_len + 3 ; i++) {
								snprintf(pl_outstr + k, vl_size - k, "%02x", pl_outdata[i]);
								k = k + 2;
							}
							pl_outstr[k] = '\n';

							strncat(p_tatlhdmi_IO[OUT_FULLCOMMAND], pl_outstr, strlen(pl_outstr));
						}	/*realloc if (p_tatlhdmi_IO[OUT_FULLCOMMAND]!=NULL) */
					} /* if (if (hdmi_cmdreg.return_status)) */
					else {
						printf("Return Status : FAIL\n");
						vl_size = 8;	/* 0x1100\n = 8 symbols */
						p_tatlhdmi_IO[OUT_FULLCOMMAND] =
						    realloc(p_tatlhdmi_IO[OUT_FULLCOMMAND], (vl_size) * sizeof(char));
						if (p_tatlhdmi_IO[OUT_FULLCOMMAND] != NULL) {
							memset(p_tatlhdmi_IO[OUT_FULLCOMMAND], 0, vl_size);
							pl_outstr = calloc(vl_size, sizeof(char));
							*(pl_outstr) = '0';
							*(pl_outstr + 1) = 'x';
							snprintf(pl_outstr + 2, vl_size - 2, "%02x", pl_outdata[0]);
							snprintf(pl_outstr + 4, vl_size - 4, "%02x", pl_outdata[1]);
							*(pl_outstr + 6) = '\n';
							strncat(p_tatlhdmi_IO[OUT_FULLCOMMAND], pl_outstr, strlen(pl_outstr));
						} else {
							vl_Error = -4;
							printf("ERROR realloc FAIL vl_error: %d\n", vl_Error);
						}
					}
				} else {
					vl_Error = -3;
				}
			} else {
				printf("ERROR: commands should be like [0x][commande ID][lenght of param][param[0]][param[1]] ...\n");
				vl_Error = -2;
			}
			pl_token = strtok(NULL, "\n");
		}		/*while */
	} /* if calloc */
	else {
		vl_Error = -1;
	}

	if (vl_Error != 0)
		printf("tatlx1_10_send_n_register_command  ERROR:%d\n", vl_Error);
	else
		*(p_tatlhdmi_IO[OUT_FULLCOMMAND] + vl_sizebackup + vl_size - 1) = '\0';

	if (pl_temp != NULL) {
		free(pl_temp);
		pl_temp = NULL;
	}
	if (pl_outstr != NULL) {
		free(pl_outstr);
		pl_outstr = NULL;
	}

	return vl_Error;
}

/*|+----------------------------------------------------------------------------**/
/** Procedure    : tatlx1_11_Crc32calculation*/
/**-----------------------------------------------------------------------------**/
/** Description  : Calcul 32 Cyclic Redundancy Check*/
/**-----------------------------------------------------------------------------**/
/** Parameter(s) :*/
/**-----------------------------------------------------------------------------**/
/** Return Value : uint32_t, crc32bit*/
/**----------------------------------------------------------------------------+|*/
/*static uint32_t tatlx1_11_Crc32calculation(uint8_t din[288], uint8_t **outcrc)*/
/*{*/

/*    uint8_t newcrc[32], data[8];*/
/*    uint32_t crc32bit = 0;*/

/*    int byte, bit, k;*/

/*    uint8_t *crc = (uint8_t *) malloc(32 * sizeof(uint8_t));*/

/*    for (bit = 0; bit < 32; bit++)*/
/*        crc[bit] = 0;*/

/*    for (byte = 0; byte < 288; byte++) {*/
/*        k = 1;*/
/*        for (bit = 0; bit < 8; bit++) {*/
/*            data[bit] = (din[byte] & k) >> bit;*/
/*            k = k * 2;*/
/*        }*/

/*        newcrc[0] = data[6] ^ data[0] ^ crc[24] ^ crc[30];*/
/*        newcrc[1] = data[7] ^ data[6] ^ data[1] ^ data[0] ^ crc[24] ^ crc[25] ^ crc[30] ^ crc[31];*/
/*        newcrc[2] = data[7] ^ data[6] ^ data[2] ^ data[1] ^ data[0] ^ crc[24] ^ crc[25] ^ crc[26] ^ crc[30] ^ crc[31];*/
/*        newcrc[3] = data[7] ^ data[3] ^ data[2] ^ data[1] ^ crc[25] ^ crc[26] ^ crc[27] ^ crc[31];*/
/*        newcrc[4] = data[6] ^ data[4] ^ data[3] ^ data[2] ^ data[0] ^ crc[24] ^ crc[26] ^ crc[27] ^ crc[28] ^ crc[30];*/
/*        newcrc[5] = data[7] ^ data[6] ^ data[5] ^ data[4] ^ data[3] ^ data[1] ^ data[0] ^ crc[24] ^ crc[25] ^ crc[27] ^ crc[28] ^ crc[29] ^ crc[30] ^ crc[31];*/
/*        newcrc[6] = data[7] ^ data[6] ^ data[5] ^ data[4] ^ data[2] ^ data[1] ^ crc[25] ^ crc[26] ^ crc[28] ^ crc[29] ^ crc[30] ^ crc[31];*/
/*        newcrc[7] = data[7] ^ data[5] ^ data[3] ^ data[2] ^ data[0] ^ crc[24] ^ crc[26] ^ crc[27] ^ crc[29] ^ crc[31];*/
/*        newcrc[8] = data[4] ^ data[3] ^ data[1] ^ data[0] ^ crc[0] ^ crc[24] ^ crc[25] ^ crc[27] ^ crc[28];*/
/*        newcrc[9] = data[5] ^ data[4] ^ data[2] ^ data[1] ^ crc[1] ^ crc[25] ^ crc[26] ^ crc[28] ^ crc[29];*/
/*        newcrc[10] = data[5] ^ data[3] ^ data[2] ^ data[0] ^ crc[2] ^ crc[24] ^ crc[26] ^ crc[27] ^ crc[29];*/
/*        newcrc[11] = data[4] ^ data[3] ^ data[1] ^ data[0] ^ crc[3] ^ crc[24] ^ crc[25] ^ crc[27] ^ crc[28];*/
/*        newcrc[12] = data[6] ^ data[5] ^ data[4] ^ data[2] ^ data[1] ^ data[0] ^ crc[4] ^ crc[24] ^ crc[25] ^ crc[26] ^ crc[28] ^ crc[29] ^ crc[30];*/
/*        newcrc[13] = data[7] ^ data[6] ^ data[5] ^ data[3] ^ data[2] ^ data[1] ^ crc[5] ^ crc[25] ^ crc[26] ^ crc[27] ^ crc[29] ^ crc[30] ^ crc[31];*/
/*        newcrc[14] = data[7] ^ data[6] ^ data[4] ^ data[3] ^ data[2] ^ crc[6] ^ crc[26] ^ crc[27] ^ crc[28] ^ crc[30] ^ crc[31];*/
/*        newcrc[15] = data[7] ^ data[5] ^ data[4] ^ data[3] ^ crc[7] ^ crc[27] ^ crc[28] ^ crc[29] ^ crc[31];*/
/*        newcrc[16] = data[5] ^ data[4] ^ data[0] ^ crc[8] ^ crc[24] ^ crc[28] ^ crc[29];*/
/*        newcrc[17] = data[6] ^ data[5] ^ data[1] ^ crc[9] ^ crc[25] ^ crc[29] ^ crc[30];*/
/*        newcrc[18] = data[7] ^ data[6] ^ data[2] ^ crc[10] ^ crc[26] ^ crc[30] ^ crc[31];*/
/*        newcrc[19] = data[7] ^ data[3] ^ crc[11] ^ crc[27] ^ crc[31];*/
/*        newcrc[20] = data[4] ^ crc[12] ^ crc[28];*/
/*        newcrc[21] = data[5] ^ crc[13] ^ crc[29];*/
/*        newcrc[22] = data[0] ^ crc[14] ^ crc[24];*/
/*        newcrc[23] = data[6] ^ data[1] ^ data[0] ^ crc[15] ^ crc[24] ^ crc[25] ^ crc[30];*/
/*        newcrc[24] = data[7] ^ data[2] ^ data[1] ^ crc[16] ^ crc[25] ^ crc[26] ^ crc[31];*/
/*        newcrc[25] = data[3] ^ data[2] ^ crc[17] ^ crc[26] ^ crc[27];*/
/*        newcrc[26] = data[6] ^ data[4] ^ data[3] ^ data[0] ^ crc[18] ^ crc[24] ^ crc[27] ^ crc[28] ^ crc[30];*/
/*        newcrc[27] = data[7] ^ data[5] ^ data[4] ^ data[1] ^ crc[19] ^ crc[25] ^ crc[28] ^ crc[29] ^ crc[31];*/
/*        newcrc[28] = data[6] ^ data[5] ^ data[2] ^ crc[20] ^ crc[26] ^ crc[29] ^ crc[30];*/
/*        newcrc[29] = data[7] ^ data[6] ^ data[3] ^ crc[21] ^ crc[27] ^ crc[30] ^ crc[31];*/
/*        newcrc[30] = data[7] ^ data[4] ^ crc[22] ^ crc[28] ^ crc[31];*/
/*        newcrc[31] = data[5] ^ crc[23] ^ crc[29];*/

/*        for (k = 31; k >= 0; k--)*/
/*            crc[k] = newcrc[k];*/
/*    }*/
/*    printf("\n");*/

/*    for (k = 31; k >= 0; k = k - 4) {*/
/*        crc32bit =*/
/*            crc32bit |*/
/*            ((crc[k] * 8 + crc[k - 1] * 4 + crc[k - 2] * 2 +*/
/*              crc[k - 3]) << (k - 3));*/
/*    }*/

/*    *outcrc = crc;*/

/*    return crc32bit;*/
/*}*/

/*----------------------------------------------------------------------------*
* Procedure    : tatlx1_00ActExec
*-----------------------------------------------------------------------------*
* Description  : Execute the Cmd Action sent by DTH
*-----------------------------------------------------------------------------*
* Parameter(s) : dth_element *, the  DTH element
*-----------------------------------------------------------------------------*
* Return Value : u32, TAT error
*----------------------------------------------------------------------------*/
int tatlx1_00ActExec(struct dth_element *elem)
{
	int vl_Error = TAT_ERROR_OFF;
	struct hdmi_register hdmi_reg;
	struct hdmi_command_register hdmi_cmdreg;

	/* Actions to read/write register */
	if (elem->user_data <= ACT_RWREG15) {
		if (v_tatlhdmi_IO[elem->user_data] == HDMIREG_READ) {
			/* set address */
			hdmi_reg.offset = (char)elem->user_data;
			hdmi_reg.value = 0;
			vl_Error = tatlx1_03hdmiReg(IOC_HDMI_REGISTER_READ, &hdmi_reg);
			/* get value */
			v_tatlhdmi_IO[INOUT_REGVALUE] = hdmi_reg.value;
		} else {
			hdmi_reg.offset = elem->user_data;
			hdmi_reg.value = v_tatlhdmi_IO[INOUT_REGVALUE];
			vl_Error = tatlx1_03hdmiReg(IOC_HDMI_REGISTER_WRITE, &hdmi_reg);
		}

	}
	/* Actions to send Command */
	else if (elem->user_data == ACT_SENDCONFIG) {
		memset(hdmi_cmdreg.buf, 0, BUFFERMAX);
		hdmi_cmdreg.return_status = 0;
		hdmi_cmdreg.cmd_id = v_tatlhdmi_IO[IN_CMDID];
		hdmi_cmdreg.buf_len = v_tatlhdmi_IO[IN_DATALEN];
		memcpy(hdmi_cmdreg.buf, v_tatlhdmi_hdmicmdbuf, hdmi_cmdreg.buf_len);

		if (0 == tatlx1_04hdmiCommandReg(IOC_HDMI_CONFIGURATION_WRITE, &hdmi_cmdreg)) {
			v_tatlhdmi_IO[OUT_CMDID] = v_tatlhdmi_IO[IN_CMDID];
			v_tatlhdmi_IO[OUT_RETURNSTATUS] = hdmi_cmdreg.return_status;
			if (hdmi_cmdreg.return_status == HDMI_RESULT_OK) {
				v_tatlhdmi_IO[OUT_DATALEN] = hdmi_cmdreg.buf_len;
				memcpy(v_tatlhdmi_hdmicmdbuf, hdmi_cmdreg.buf, hdmi_cmdreg.buf_len);
			} else {
				printf("TATHDMI ERROR : tatlx1_04hdmiCommandReg return status FAIL\n");
				v_tatlhdmi_IO[OUT_DATALEN] = 0;
				memset(v_tatlhdmi_hdmicmdbuf, 0, BUFFERMAX);
			}
		} else {
			vl_Error = -2;
			printf("TATHDMI ERROR : tatlx1_04hdmiCommandReg vl_error: %d\n", vl_Error);
		}
	} else if (elem->user_data == ACT_SENDXCOMMAND) {
		vl_Error = tatlx1_10_send_n_register_command();
	}

	/* Actions for HDCP Management */
	else if (elem->user_data == ACT_HDCPOTPSTAT) {
		vl_Error = tatlx1_05_check_aes_key_otp();
	} else if (elem->user_data == ACT_HDCPOTPPRO) {
		vl_Error = tatlx1_06_fuse_aes_otp_key();
	} else if (elem->user_data == ACT_HDCPAESSTAT) {
		vl_Error = tatlx1_08_load_and_verify_aes_keys();
	}

	/* No Actions matched */
	else {
		vl_Error = ENOMSG;
	}

	if (vl_Error != 0) {
		printf("TATHDMI ERROR : tatlx1_00ActExec elem->user_data:%d   Error:%d\n",
		     elem->user_data, vl_Error);
	}

	return vl_Error;
}

/*----------------------------------------------------------------------------*
* Procedure    : tatlx1_01ActSet
*-----------------------------------------------------------------------------*
* Description  : Set the DTH Element Value Input Parameters
*-----------------------------------------------------------------------------*
* Parameter(s) : dth_element *, Structure of DTH element
*		  void *, Value of Dth Element
*-----------------------------------------------------------------------------*
* Return Value : u32, TAT error
*----------------------------------------------------------------------------*/
int tatlx1_01ActSet(struct dth_element *elem, void *Value)
{
	int vl_Error = TAT_ERROR_OFF;
	struct dth_array *ValTab = NULL;

	/*special case for array */
	if (elem->user_data == IN_DATA) {
		ValTab = (struct dth_array *)Value;
		memset(v_tatlhdmi_hdmicmdbuf, 0, BUFFERMAX);
		memcpy(v_tatlhdmi_hdmicmdbuf, (u8 *) ValTab->array, v_tatlhdmi_IO[IN_DATALEN]);
		return vl_Error;
	} else {
		switch (elem->type) {
		case DTH_TYPE_U8:
			v_tatlhdmi_IO[elem->user_data] = *(u8 *) Value;
			break;
		case DTH_TYPE_S8:
			v_tatlhdmi_IO[elem->user_data] = *(s8 *) Value;
			break;
		case DTH_TYPE_U16:
			v_tatlhdmi_IO[elem->user_data] = *(u16 *) Value;
			break;
		case DTH_TYPE_S16:
			v_tatlhdmi_IO[elem->user_data] = *(s16 *) Value;
			break;
		case DTH_TYPE_U32:
			v_tatlhdmi_IO[elem->user_data] = *(u32 *) Value;
			break;
		case DTH_TYPE_S32:
			v_tatlhdmi_IO[elem->user_data] = *(s32 *) Value;
			break;
		case DTH_TYPE_U64:
			v_tatlhdmi_IO[elem->user_data] = *(u64 *) Value;
			break;
		case DTH_TYPE_S64:
			v_tatlhdmi_IO[elem->user_data] = *(s64 *) Value;
			break;
		case DTH_TYPE_STRING:
			if ((char *)Value != NULL) {
				int len = strlen((char *)Value);
				if (p_tatlhdmi_IO[elem->user_data] != NULL)
					free(p_tatlhdmi_IO[elem->user_data]);

				p_tatlhdmi_IO[elem->user_data] = (char *)calloc(len + 1, sizeof(char));
				strncpy(p_tatlhdmi_IO[elem->user_data], (char *)Value, len);
			} else {
				printf("TATHDMI ERROR : tatlx1_01ActSet (char*)Value) == NULL\n");
			}
			break;
		default:
			printf("TATHDMI ERROR : tatlx1_01ActSet TYPE not AVAILABLE\n");
			vl_Error = ENOMSG;
			break;
		}
	}
	return vl_Error;

}

/*----------------------------------------------------------------------------*
* Procedure    : tatlx1_02ActGet
*-----------------------------------------------------------------------------*
* Description  : Get the DTH ELement Value Output Parameters
*-----------------------------------------------------------------------------*
* Parameter(s) : dth_element *, Structure of DTH element
*		  void *, Value of Dth Element
*-----------------------------------------------------------------------------*
* Return Value : u32, TAT error
*----------------------------------------------------------------------------*/
int tatlx1_02ActGet(struct dth_element *elem, void *Value)
{

	int vl_Error = TAT_ERROR_OFF;
	struct dth_array *ValTab = NULL;

	/*special case for array */
	if (elem->user_data == OUT_DATA) {

		ValTab = (struct dth_array *)Value;

		if (v_tatlhdmi_IO[OUT_DATALEN] != 0)
			memcpy((u8 *) ValTab->array, v_tatlhdmi_hdmicmdbuf, v_tatlhdmi_IO[OUT_DATALEN]);

		return vl_Error;
	}

	switch (elem->type) {

	case DTH_TYPE_U8:
		*((u8 *) Value) = v_tatlhdmi_IO[elem->user_data];
		break;
	case DTH_TYPE_S8:
		*((s8 *) Value) = v_tatlhdmi_IO[elem->user_data];
		break;
	case DTH_TYPE_U16:
		*((u16 *) Value) = v_tatlhdmi_IO[elem->user_data];
		break;
	case DTH_TYPE_S16:
		*((s16 *) Value) = v_tatlhdmi_IO[elem->user_data];
		break;
	case DTH_TYPE_U32:
		*((u32 *) Value) = v_tatlhdmi_IO[elem->user_data];
		break;
	case DTH_TYPE_S32:
		*((s32 *) Value) = v_tatlhdmi_IO[elem->user_data];
		break;
	case DTH_TYPE_U64:
		*((u64 *) Value) = v_tatlhdmi_IO[elem->user_data];
		break;
	case DTH_TYPE_S64:
		*((s64 *) Value) = v_tatlhdmi_IO[elem->user_data];
		break;
	case DTH_TYPE_STRING:

		if (p_tatlhdmi_IO[elem->user_data] != NULL)
			strncpy((char *)Value, p_tatlhdmi_IO[elem->user_data], strlen(p_tatlhdmi_IO[elem->user_data]));
		else
			strncpy((char *)Value, "NULL", 5);

		break;
	default:
		printf("TATHDMI ERROR : tatl1_03ActHDMIParam_Get TYPE not AVAILABLE\n");
		vl_Error = ENOMSG;
		break;
	}

	return vl_Error;

}

/*----------------------------------------------------------------------------*
* Procedure    : tatlx1_03hdmiReg
*-----------------------------------------------------------------------------*
* Description  : Read/write hardware HDMI registers
*-----------------------------------------------------------------------------*
* Parameter(s) : unsigned int mode, READ or SEND configuration
*		  hdmi_register *pp_hdmi_reg, Structure of HDMIREG
*-----------------------------------------------------------------------------*
* Return Value : u32, TAT error
*----------------------------------------------------------------------------*/
int tatlx1_03hdmiReg(unsigned int mode, struct hdmi_register *pp_hdmi_reg)
{
	int vl_hdmifd = 0;
	int vl_Error = 0;

	/* open HDMI File device Access  */
	vl_hdmifd = open(TATHDMI_HDMI_DEVICE_NAME, O_RDWR);
	if (vl_hdmifd < 0) {
		printf("TATHDMI ERROR : tatlx1_03hdmiReg fail to open /dev/hdmi\n");
		vl_Error = ENOENT;
	} else {
		/* Send the command */
		printf("TATHDMI : tatlx1_03hdmiReg before ioctl cmd, hdmi_reg->offset 0x%x   hdmi_reg->value 0x%x\n",
		     pp_hdmi_reg->offset, pp_hdmi_reg->value);
		vl_Error = ioctl(vl_hdmifd, mode, pp_hdmi_reg);
		printf("TATHDMI : tatlx1_03hdmiReg after ioctl cmd, hdmi_reg->offset 0x%x   hdmi_reg->value 0x%x\n",
		     pp_hdmi_reg->offset, pp_hdmi_reg->value);

		/* close hdmi device  Access */
		close(vl_hdmifd);
	}
	return vl_Error;
}

