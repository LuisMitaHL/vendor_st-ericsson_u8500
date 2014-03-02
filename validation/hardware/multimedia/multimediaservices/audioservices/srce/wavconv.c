/*
 * =====================================================================================
 * 
 * 	Filename:  wavconv.c
 * 
 *     	Description:  source code of wave file convert service
 * 
 *      © Copyright ST-Ericsson, 2009. All Rights Reserved
 * 
 * =====================================================================================
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "hats_audio.h"
#include "hats_audio.hi"

#define COMPLETION_MODE 0
#define NB_SLOT  2

#define PCM_FORMAT 0x01
#define IEEE_FLOAT_FORMAT 0x03

/* ========================================================================
int GenerateHeader(FILE *p_InFILE,FILE *p_OutFILE)
modify wave header to transform mono/stereo FLOAT sample file S32 samples file
========================================================================*/

e_HatsAudio_PCM_Header_t PCM_Data;
e_HatsAudio_PCM_Header_t PCM_Data_out;

int GenerateHeader(FILE * p_InFILE, FILE * p_OutFILE)
{
	size_t WSize;

	fseek(p_InFILE, 0, SEEK_SET);
	fread(&PCM_Data, sizeof(e_HatsAudio_PCM_Header_t), 1, p_InFILE);

	if (strncmp(PCM_Data.ChunkID, "RIFF", 4) != 0) {
		printf("\nBad File format: input file must be a wav file\n");
		return HATS_AUDIO_FAILED;
	}
	if (PCM_Data.NumChannels > 2) {
		printf
		    ("invalid format for wav file : nb channel detected is %d instead of 1 or 2\n",
		     PCM_Data.NumChannels);
		return HATS_AUDIO_FAILED;
	}
	if (PCM_Data.SampleRate != 48000) {
		printf
		    ("invalid Sample rate for wav file : only 48000 Hz supported, rate detected is %d\n",
		     PCM_Data.SampleRate);
		return HATS_AUDIO_FORMAT_NOT_SUPPORTED;
	}

	/* wavconv: used only to convert IEEE Floating_point WAV files to Linear PCM WAV Files */
	if (PCM_Data.AudioFormat != IEEE_FLOAT_FORMAT) {
		printf
		    ("\nERROR GenerateHeader: bad input format for conversion\n");
		return HATS_AUDIO_FORMAT_NOT_SUPPORTED;
	}
	memcpy(&PCM_Data_out, &PCM_Data, sizeof(e_HatsAudio_PCM_Header_t));

	PCM_Data_out.AudioFormat = PCM_FORMAT;	/* uncompressed PCM */
	/* data size =>change samplesize and nb of channel* */
	PCM_Data_out.SubChunk2Size =
	    (PCM_Data_out.SubChunk2Size * NB_SLOT / PCM_Data_out.NumChannels *
	     32) / PCM_Data_out.BitsPerSample;
	/* chunksize */
	PCM_Data_out.ChunkSize = PCM_Data_out.SubChunk2Size + 36;
	/* byterate=>increase samplesize and nb of channel */
	PCM_Data_out.ByteRate =
	    (PCM_Data_out.ByteRate * NB_SLOT / PCM_Data_out.NumChannels * 32) /
	    PCM_Data_out.BitsPerSample;
	PCM_Data_out.BlockAlign = 4 * NB_SLOT;

	/* bits per Samples */
	PCM_Data_out.BitsPerSample = 32;

	fseek(p_OutFILE, 0, SEEK_SET);
	if ((WSize =
	     fwrite(&PCM_Data_out, 1, sizeof(e_HatsAudio_PCM_Header_t),
		    p_OutFILE)) != 44) {
		goto FWRITE_ERROR;
	}

	return HATS_AUDIO_NO_ERROR;

	/* error management */

 FWRITE_ERROR:
	printf("\nERROR GenerateHeader: no space left on device\n");
	return HATS_AUDIO_NO_SPACE_LEFT;

}

/* ========================================================================
int Generatedata(e_HatsAudioDevice_t Device,FILE *p_InFILE,FILE *p_OutFILE)
		1) modify sample format from 16bits size to 20bits size stored on 32bits size 
          0xFFFF => 0x000FFFF0
		2) copy  channel several type to have sample on 8 channels
    			mono  :  LLLL     =>  000LLLL0 000LLLL0 000LLLL0 000LLLL0 000LLLL0 000LLLL0 000LLLL0 000LLLL0 
    			stereo:  LLLL RRRR=>  000LLLL0 000RRRR0 000LLLL0 000RRRR0 000LLLL0 000RRRR0 000LLLL0 000RRRR0 
   ========================================================================*/
int Generatedata(FILE * p_InFILE, FILE * p_OutFILE)
{
	unsigned char data[44];
	unsigned int out_data[8];
	unsigned int i;
	unsigned int silence = 0;
	unsigned int in_data;
	/*float real_val; */
	int shift;

/*  printf ("\ninput NbOfChannels: %d\n", PCM_Data.NumChannels);*/

/*  printf ("input BlockAlign: %d\n", PCM_Data.BlockAlign);*/
	if (PCM_Data.BlockAlign > 16) {
		printf
		    ("ERROR: invalid format for wav file : invalid block size \n");
		return HATS_AUDIO_FAILED;
	}

/*  printf ("input BitsperSample: %d\n", PCM_Data.BitsPerSample);*/

	fseek(p_InFILE, 44, SEEK_SET);
	fseek(p_OutFILE, 44, SEEK_SET);

	if (PCM_Data.BitsPerSample != 32) {
		printf
		    ("ERROR: invalid format for wav file : sample must be in 32bits format not %dbits\n",
		     PCM_Data.BitsPerSample);
		return HATS_AUDIO_FAILED;
	}
	if (PCM_Data.NumChannels > 2) {
		printf
		    ("ERROR: invalid format for wav file : number of channel must be 1 or 2 not %d\n",
		     PCM_Data.NumChannels);
		return HATS_AUDIO_FAILED;
	}
	while (feof(p_InFILE) == 0) {
		fread(data, 1, PCM_Data.BlockAlign, p_InFILE);
		/*first step : change sample format */

		for (i = 0; i < PCM_Data.NumChannels; i++) {
			if (PCM_Data.BitsPerSample == 32) {
				/* need to convert compressed PCM to uncompressed PCM */
				/* IEEE 754 single precision binary floating-point format => 0x00FFFFF */

				in_data = data[(4 * i)]
				    + (data[(4 * i) + 1] << 8)
				    + (data[(4 * i) + 2] << 16) +
				    (data[(4 * i) + 3] << 24);
				/* fraction part */
				out_data[i] = ((in_data & 0x7FFFFF));
				out_data[i] += 0x800000;
				shift = 7;	/* convert 22 to 31 bits */
				shift += (((in_data >> 23) & 0xFF) - 126);	/* apply exposant */

				if (shift > 0) {
					out_data[i] =
					    out_data[i] << (unsigned int)shift;
				} else {
					if (shift > (-31)) {
						shift = -shift;
						out_data[i] =
						    out_data[i] >> (unsigned
								    int)shift;
					} else {
						out_data[i] = 0;
					}
				}

				out_data[i] *= (1 - 2 * (in_data >> 31));	/*sign */
			} else {
				printf("sample size format %d unsupported\n",
				       PCM_Data.BitsPerSample);
			}

		}
		if (feof(p_InFILE) == 0) {
#if (COMPLETION_MODE==1)
			/* duplicate channel */
			for (i = 0;
			     i < ((unsigned int)NB_SLOT / PCM_Data.NumChannels);
			     i++) {
				if (fwrite
				    (out_data, 4, PCM_Data.NumChannels,
				     p_OutFILE) != PCM_Data.NumChannels) {
					goto FWRITE_ERROR;
				}
			}
#else
			if (fwrite(out_data, 4, PCM_Data.NumChannels, p_OutFILE)
			    != PCM_Data.NumChannels) {
				goto FWRITE_ERROR;
			}
			/* printf( "output sample: %x\n",out_data[0]); */
			/* add silence */
			for (i = 0;
			     i < ((unsigned int)NB_SLOT - PCM_Data.NumChannels);
			     i++) {
				if (fwrite(&silence, 4, 1, p_OutFILE) != 1) {
					goto FWRITE_ERROR;
				}
			}
#endif
		}
	}
	return HATS_AUDIO_NO_ERROR;
	/* error management */

 FWRITE_ERROR:
	printf("\nERROR Generatedata: no space left on device\n");
	return HATS_AUDIO_NO_SPACE_LEFT;
}

/*--------------------------------------------------------------------------------------*/
/*! \ingroup audio_services
	\fn uint32_t ConvWavFile(const char * InFile, const char * OutFile);
	\brief old Service allowing the convertion from 16 or 32 bits wav file to 20 bit 8ch wav file
	\param [in] InFile pointer to the name of the file to convert
	\param [in] OutFile pointer to the name of the converted file
	\return 	
		0 if no error
*/
/*--------------------------------------------------------------------------------------*/
uint32_t ConvWavFile(const char *InFile, const char *OutFile)
{
	FILE *p_InFILE, *p_OutFILE;
	int result = HATS_AUDIO_NO_ERROR;
/*    e_HatsAudio_PCM_Header_t PCM_Data;*/

	/* check that input and output are different */
	if (strncmp(InFile, OutFile, 200) == 0) {
		printf("\nERROR : Input and output file must be different\n");
		return HATS_AUDIO_FAILED;
	}
	/* open  input */
	p_InFILE = fopen(InFile, "r");
	if ((p_InFILE == NULL)) {
		printf("\nERROR :  %s not found\n", InFile);
		return HATS_AUDIO_FAILED;
	}
	/* open output file */
	p_OutFILE = fopen(OutFile, "w");
	if ((p_InFILE == NULL)) {
		printf("\nERROR :  %s not found\n", OutFile);
		return HATS_AUDIO_FAILED;
	}

	result = GenerateHeader(p_InFILE, p_OutFILE);
	if (result == HATS_AUDIO_NO_ERROR)
		result = Generatedata(p_InFILE, p_OutFILE);

	fclose(p_InFILE);
	fclose(p_OutFILE);
	return result;
}

/*--------------------------------------------------------------------------------------*/
/*! \ingroup audio_services
	\fn uint32_t ConvWavFile(e_HatsAudioDevice_t Device,const char * InFile, const char * OutFile);
	\brief Service alow the convertion from 16 or 32 bits wav file to 20 bit 8ch wav file
	\param [in] InFile pointer to the name of the file to convert
	\param [in] OutFile pointer to the name of the converted file
	\return 	
		0 if no error
*/
/*--------------------------------------------------------------------------------------*/
uint32_t
ConvWaveFile(e_HatsAudioDevice_t Device, const char *InFile,
	     const char *OutFile)
{
	FILE *p_InFILE, *p_OutFILE;
	int result = HATS_AUDIO_NO_ERROR;

	/*check device */
	if ((Device != AUDIO_DEVICE_0) && (Device != AUDIO_DEVICE_HDMI)) {
		printf(" unknown device : DEVICE = %d\n", Device);
		return HATS_AUDIO_INVALID_PARAMETER;
	}

	/* check that input and output are different */
	if (strncmp(InFile, OutFile, 200) == 0) {
		printf("\nERROR : Input and output file must be different\n");
		return HATS_AUDIO_FAILED;
	}
	/* open  input */
	p_InFILE = fopen(InFile, "r");
	if ((p_InFILE == NULL)) {
		printf("\nERROR :  %s not found\n", InFile);
		return HATS_AUDIO_FAILED;
	}
	/* open output file */
	p_OutFILE = fopen(OutFile, "w");
	if ((p_OutFILE == NULL)) {
		printf("\nERROR :  %s not found\n", OutFile);
		return HATS_AUDIO_FAILED;
	}

	result = GenerateHeader(p_InFILE, p_OutFILE);

	if (result == HATS_AUDIO_NO_ERROR)
		result = Generatedata(p_InFILE, p_OutFILE);

	fclose(p_InFILE);
	fclose(p_OutFILE);
	return result;
}
