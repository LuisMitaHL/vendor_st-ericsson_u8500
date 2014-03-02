/**
* @file avutils_avs.c
* @brief AVS related functions. A single call to avs_configure() will make sure
*        AVS is up and running.
* @author Jérôme Soulard (jerome.soulard@stericsson.com)
* @version 2.0.2
* @date 02 November 2011
*/

#include "avutils_avs.h"
#include "numerics.h"
#include "xp70_memmap.h"
#include <uart.h>


/* Homebrew CEIL macro */
#define AVS_CEIL(VAR) ((VAR - (int)VAR) == 0 ? (int)VAR : (int)VAR + 1)
#define BIT_MASK9 0x1FF
#define BIT_MASK8 0xFF
#define MAX_AVS_ELEMENTS 3


/**
 * @brief Configure and enable AVS.
 * @param int avs_fuseword0: value of fuseword0 (FVR 46)
 * @param int avs_fuseword1: value of fuseword1 (FVR 47)
 * @param const int avs_inptr: pointer to a copy of the AVS struct used by PRCMU.
 * @return AVS_EXIT_SUCCESS: successful, AVS applied
 * @return AVS_EXIT_UNFUSEDDEVICE: error, device is not fused
 * @return AVS_EXIT_UNKNOWNREV: error, unknown AVS revision
 * @return AVS_EXIT_OUTOFLIMITS: error, one requested voltage is too high
 */
int avs_configure(unsigned int avs_fuseword0, unsigned int avs_fuseword1,
						const int* avs_inptr)
{

	int avs_revision;

	/* FIXME Rework the way those arrays are indexed (use an enum) */
	/* First element in avs_varm arrays is 1000MHz */
	int avs_varm_enabled[MAX_AVS_ELEMENTS]; /* First element is 1000MHz */
	int avs_varm_offset[MAX_AVS_ELEMENTS]; /* First element is 1000MHz */
	float avs_varm_stack[MAX_AVS_ELEMENTS]; /* First element is 1000MHz */
	unsigned char avs_varm_petra[MAX_AVS_ELEMENTS];/* First element is 1000MHz */

	/* First element in avs_vape arrays is 333MHz */
	int avs_vape_enabled[MAX_AVS_ELEMENTS]; /* First element is 333MHz */
	int avs_vape_offset[MAX_AVS_ELEMENTS]; /* First element is 333MHz */
	float avs_vape_stack[MAX_AVS_ELEMENTS]; /* First element is 333MHz */
	unsigned char avs_vape_petra[MAX_AVS_ELEMENTS];

	int retval;
	int i;

	/* Retrieve which AVS revision has been used */
	avs_revision = (avs_fuseword0 & AVS_REVISION) >> AVS_REVISION_SHIFT;
	logmsg("avs - revision %x \r\n", avs_revision);

	/* Proceed according to AVS revision */
	switch(avs_revision) {
		case 0:
			/* Unfused devices + rev0 devices (prototypes only) */
			return AVS_EXIT_UNFUSEDDEVICE;
			break;

		case 1: /* Rev 1*/
			/* Reading fuses accoring to Rev 1 mapping */
			avs_r1_read_fuses(avs_fuseword0, avs_fuseword1,
				avs_varm_enabled, avs_vape_enabled,
					avs_varm_offset, avs_vape_offset);

			/* Get 2's complement for offsets */
			avs_varm_offset[1] = avs_twos_compl(avs_varm_offset[1],
								8, BIT_MASK9);
			avs_varm_offset[2] = avs_twos_compl(avs_varm_offset[2],
								8, BIT_MASK9);

			/* Converting offset into absolute minimum voltage */
			avs_varm_offset[1] =
				avs_varm_offset[1] * AVS_R1_ARM_OFFSETCONV
							+ AVS_ARM800_VTX;
			avs_varm_offset[2] =
				avs_varm_offset[2] * AVS_R1_ARM_OFFSETCONV
							+ AVS_ARM400_VTX;
			/* Make sure we are above minimum voltage */
			avs_checklowerlimits(avs_varm_offset, avs_vape_offset);

			/* Computing stack */
			avs_computestack(avs_varm_stack, avs_vape_stack);

			/* Compensate for characterisation errors */
			/*
			 * FIXME should be used with the base 
			 * voltage (clamp relation)
			 */
			avs_varm_stack[1] += AVS_R1_ARM800_TESTOFFSET;
			avs_varm_stack[2] += AVS_R1_ARM400_TESTOFFSET;

			/* Compute value to be sent to Petra (10mV rounding) */
			avs_computepetra(avs_varm_petra, avs_vape_petra,
				avs_varm_offset, avs_vape_offset,
				avs_varm_stack, avs_vape_stack);

			/* Make sure we are below maximum voltage */
			retval = avs_checkupperlimits(avs_varm_enabled,
							avs_vape_enabled,
							avs_varm_petra,
							avs_vape_petra);
			if(retval)
				/* Error, Voltage out of bounds */
				return retval;

			/* Enable and configure GSS */
			/* GSS not enabled yet */

			/* Write new AVS voltages */
			avs_fill_avsstruct(avs_varm_enabled, avs_vape_enabled,
					avs_varm_petra, avs_vape_petra, avs_inptr);
			break;

		case 2: /* Rev 2*/
			/* Reading fuses accoring to Rev 2 mapping */
			avs_r2_read_fuses(avs_fuseword0, avs_fuseword1,
					avs_varm_enabled, avs_vape_enabled,
					avs_varm_offset, avs_vape_offset);

			/* Get 2's complement for offsets */
			for (i = 0; i < MAX_AVS_ELEMENTS; i++) {
				avs_varm_offset[i] = avs_twos_compl
					(avs_varm_offset[i], 7, BIT_MASK8);
				avs_vape_offset[i] = avs_twos_compl
					(avs_vape_offset[i], 7, BIT_MASK8);
			}

			/* Converting offset into absolute minimum voltage */
			avs_varm_offset[0] =
				avs_varm_offset[0] * AVS_R2_ARM_OFFSETCONV 
							+ AVS_ARM1000_VTX;
			avs_varm_offset[1] =
				avs_varm_offset[1] * AVS_R2_ARM_OFFSETCONV 
							+ AVS_ARM800_VTX;
			avs_varm_offset[2] = avs_varm_offset[2] * AVS_R2_ARM_OFFSETCONV
							+ AVS_ARM400_VTX;
			avs_vape_offset[0] = avs_vape_offset[0] * AVS_R2_APE_OFFSETCONV
							+ AVS_APE333_VTX;
			avs_vape_offset[1] = avs_vape_offset[1] * AVS_R2_APE_OFFSETCONV
							+ AVS_APE266_VTX;
			avs_vape_offset[2] = avs_vape_offset[2] * AVS_R2_APE_OFFSETCONV
							+ AVS_APE200_VTX;

			/* Make sure we are above minimum voltage */
			avs_checklowerlimits(avs_varm_offset, avs_vape_offset);

			/* Computing stack */
			avs_computestack(avs_varm_stack, avs_vape_stack);

			/* Compensate for characterisation errors */
			/*
			 * FIXME should be used with the base 
			 * voltage (clamp relation)
			 */
			avs_varm_stack[0] += AVS_R2_ARM1000_TESTOFFSET;
			avs_varm_stack[1] += AVS_R2_ARM800_TESTOFFSET;
			avs_varm_stack[2] += AVS_R2_ARM400_TESTOFFSET;
			avs_vape_stack[0] += AVS_R2_APE333_TESTOFFSET;
			avs_vape_stack[1] += AVS_R2_APE266_TESTOFFSET;
			avs_vape_stack[2] += AVS_R2_APE200_TESTOFFSET;

			/* Compute value to be sent to Petra (10mV rounding) */ 
			avs_computepetra(avs_varm_petra, avs_vape_petra, avs_varm_offset, 
                              avs_vape_offset, avs_varm_stack, avs_vape_stack);

			/* Make sure we are below maximum voltage */
			retval = avs_checkupperlimits(avs_varm_enabled, avs_vape_enabled,
                                           avs_varm_petra, avs_vape_petra);
			if (retval) {
				logerror("AVS check upper limits failed ret=0x%x\r\n", retval);
				return retval; /* Various error scenarios */
			}

			/* Enable and configure GSS */
			/* GSS not enabled yet */

			/* Write new AVS voltages */
			avs_fill_avsstruct(avs_varm_enabled, avs_vape_enabled,
                                avs_varm_petra, avs_vape_petra, avs_inptr);
		break;
		default:
			/* Error, Unknown AVS revision */
			return AVS_EXIT_UNKNOWNREV;
			break;

	}

	/* AVS up and running */
	return AVS_EXIT_SUCCESS;

}

/**
 * @brief Returns 2's complement of a value.
 * @param int value: signed fused value.
 * @param int nb_bits: number of bits minus sign bit.
 * @param int mask: masking all the bits including sign bit.
 * @return 2's complement of input value
 */
int avs_twos_compl(int value, unsigned int nb_bits, unsigned int mask)
{
	int compl_value = value;

	if(value >> nb_bits) { /* if sign bit is 1 */
		compl_value = (~value + 1) & mask;
		compl_value = -compl_value;
	}

	return compl_value;
}

/**
 * @brief Read fuses according to AVS revision 1.
 * @param int avs_fuseword0: value of fuseword0 (FVR 46)
 * @param int avs_fuseword1: value of fuseword1 (FVR 47)
 * @param int* avs_varm_enabled: bit stating if AVS on VARM is enabled.
 * @param int* avs_vape_enabled: bit stating if AVS on VAPE is enabled.
 * @param int* avs_varm_offset: voltage offset to be applied on VARM(NOT in mV)
 * @param int* avs_vape_offset: voltage offset to be applied on VAPE(NOT in mV)
 * @return none.
 */
void avs_r1_read_fuses(unsigned int avs_fuseword0, unsigned int avs_fuseword1,
                        int* avs_varm_enabled, int* avs_vape_enabled,
                         int* avs_varm_offset, int* avs_vape_offset)
{

	avs_varm_enabled[0] = 0;/* Rev 1 doesn't handle 1000MHz */
	avs_varm_enabled[1] = (avs_fuseword1 & AVS_R1_ARM_ENABLE)
                                                    >> AVS_R1_ARM_ENABLE_SHIFT;
	/* AVS on 800+400 or nothing */
	avs_varm_enabled[2] = avs_varm_enabled[1];

	avs_varm_offset[0] = 0;/* Rev 1 doesn't handle 1000MHz */
	avs_varm_offset[1] = (avs_fuseword0 & AVS_R1_ARM800_OFFSET)
                                                 >> AVS_R1_ARM800_OFFSET_SHIFT;
	avs_varm_offset[2] = (avs_fuseword0 & AVS_R1_ARM400_OFFSET)
                                                 >> AVS_R1_ARM400_OFFSET_SHIFT;

	/* Rev 1 doesn't handle AVS on Vape */
	avs_vape_enabled[0] = 0; 
	avs_vape_enabled[1] = 0;                
	avs_vape_enabled[2] = 0;
	avs_vape_offset[0] = 0; /* Set the offsets at 0 to be safe*/
	avs_vape_offset[1] = 0;                
	avs_vape_offset[2] = 0;

	return;
}

/**
 * @brief Read fuses according to AVS revision 2.
 * @param unsigned int avs_fuseword0: value of fuseword0 (FVR 46)
 * @param unsigned int avs_fuseword1: value of fuseword1 (FVR 47)
 * @param int* avs_varm_enabled: bit stating if AVS on VARM is enabled.
 * @param int* avs_vape_enabled: bit stating if AVS on VAPE is enabled.
 * @param int* avs_varm_offset:voltage offset to be applied on VARM. NOT in mV.
 * @param int* avs_vape_offset:voltage offset to be applied on VAPE. NOT in mV.
 * @return none.
 */
void avs_r2_read_fuses(unsigned int avs_fuseword0, unsigned int avs_fuseword1,
                        int* avs_varm_enabled, int* avs_vape_enabled,
                         int* avs_varm_offset, int* avs_vape_offset)
{

	int i;

	avs_varm_offset[0] = (avs_fuseword0 & AVS_R2_ARM1000_OFFSET)
					>> AVS_R2_ARM1000_OFFSET_SHIFT;
	avs_varm_offset[1] = (avs_fuseword0 & AVS_R2_ARM800_OFFSET)
					>> AVS_R2_ARM800_OFFSET_SHIFT;
	avs_varm_offset[2] = (avs_fuseword0 & AVS_R2_ARM400_OFFSET)
					>> AVS_R2_ARM400_OFFSET_SHIFT;
	avs_vape_offset[0] = (avs_fuseword1 & AVS_R2_APE333_OFFSET) 
					>> AVS_R2_APE333_OFFSET_SHIFT;
	avs_vape_offset[1] = (avs_fuseword1 & AVS_R2_APE266_OFFSET) 
					>> AVS_R2_APE266_OFFSET_SHIFT;
	avs_vape_offset[2] = (avs_fuseword1 & AVS_R2_APE200_OFFSET) 
					>> AVS_R2_APE200_OFFSET_SHIFT;

	/* AVS on given OPP is disabled if 0x80 is encoded as an offset */
	for (i = 0; i < MAX_AVS_ELEMENTS; i++) {
		avs_varm_enabled[i] = (avs_varm_offset[i] != 0x80);
		avs_vape_enabled[i] = (avs_vape_offset[i] != 0x80);
	}

	/* FIXME for the moment ARM1000 and OPP333 are always disabled */
	avs_varm_enabled[0] = 0;
	avs_vape_enabled[0] = 0;


	return;

}


/**
 * @brief Compute the voltage stack of each voltage domains and OPPs.
 *        Doesn't include offsets to compensate for test vectors tweaking.
 *        Those will be added acording to AVS revision
 * @param int* avs_varm_stack: stack in mV to be added on top of VARM.
 * @param int* avs_vape_stack: stack in mV to be added on top of VAPE.
 * @return none.
 */
void avs_computestack(float* avs_varm_stack, float* avs_vape_stack)
{

	avs_varm_stack[0] = AVS_ARM1000_UNDERSHOOT + AVS_ARM1000_PCBOFFSET
                       + AVS_ARM_TEMPAGING + AVS_ARM_SMPSACCURACY
                        + AVS_ARM1000_INTOFFSET;
	avs_varm_stack[1] = AVS_ARM800_UNDERSHOOT + AVS_ARM800_PCBOFFSET
                       + AVS_ARM_TEMPAGING + AVS_ARM_SMPSACCURACY
                        + AVS_ARM800_INTOFFSET; 
	avs_varm_stack[2] = AVS_ARM400_UNDERSHOOT + AVS_ARM400_PCBOFFSET
                       + AVS_ARM_TEMPAGING + AVS_ARM_SMPSACCURACY
                        + AVS_ARM400_INTOFFSET;

	/* Gradual Start/Stop benefit */
	if(AVS_ARM_GSS_ENABLE) {
		avs_varm_stack[0] -= AVS_ARM1000_GSSOFFSET;
		avs_varm_stack[1] -= AVS_ARM800_GSSOFFSET;
	}

	/*
	 * ATE offset is taken into accound here for
	 * Vape only(when fusing for CPU)
	 * FIXME ATEOFFSET should not be part of stack but of base voltage
	 * Will matter when minimum clamp voltage is reached.
	 * ATEOFFSET will always be 0 and TESTOFFSET will be used?
	 */
	avs_vape_stack[0] = AVS_APE333_UNDERSHOOT + AVS_APE333_PCBOFFSET
                       + AVS_APE_TEMPAGING + AVS_APE_SMPSACCURACY
                        + AVS_APE333_ATEOFFSET + AVS_APE333_INTOFFSET; 
	avs_vape_stack[1] = AVS_APE266_UNDERSHOOT + AVS_APE266_PCBOFFSET
                       + AVS_APE_TEMPAGING + AVS_APE_SMPSACCURACY
                        + AVS_APE266_ATEOFFSET + AVS_APE266_INTOFFSET; 
	avs_vape_stack[2] = AVS_APE200_UNDERSHOOT + AVS_APE200_PCBOFFSET
                       + AVS_APE_TEMPAGING + AVS_APE_SMPSACCURACY
                        + AVS_APE200_ATEOFFSET + AVS_APE200_INTOFFSET; 

	return;

}


/**
 * @brief Compute the hex voltage value to be sent to Petra through PRCMU.
 *        Note: AVS_PETRA_STEP mV rounding approximation, round to upper step.
 * @param int* avs_varm_petra: hex values corresponding to the voltages on VARM.
 * @param int* avs_vape_petra: hex values corresponding to the voltages on VAPE.
 * @param int* avs_varm_offset: Minimum voltage in mV to be supplied on VARM.
 * @param int* avs_vape_offset: Minimum voltage in mV to be supplied on VAPE.
 * @param int* avs_varm_stack: stack in mV to be added on top of VARM.
 * @param int* avs_vape_stack: stack in mV to be added on top of VAPE.
 * @return none.
 */
void avs_computepetra(unsigned char* avs_varm_petra,
			unsigned char* avs_vape_petra, int* avs_varm_offset,
			int* avs_vape_offset, float* avs_varm_stack,
			float* avs_vape_stack)
{
	float tempfloat_val;

	/*
	 * FIXME if(petra < 0x5) petra = 0x5;
	 * safe by design ATM
	 * TODO implement for Petra v3.0
	 */

	tempfloat_val = avs_varm_offset[0] +
				avs_varm_stack[0] - AVS_PETRA_BASEV;
	tempfloat_val /= AVS_PETRA_STEP;
	avs_varm_petra[0] = AVS_CEIL(tempfloat_val);

	tempfloat_val = avs_varm_offset[1] +
				avs_varm_stack[1] - AVS_PETRA_BASEV;
	tempfloat_val /= AVS_PETRA_STEP;
	avs_varm_petra[1] = AVS_CEIL(tempfloat_val);

	tempfloat_val = avs_varm_offset[2] +
				avs_varm_stack[2] - AVS_PETRA_BASEV;
	tempfloat_val /= AVS_PETRA_STEP;
	avs_varm_petra[2] = AVS_CEIL(tempfloat_val);

	tempfloat_val = avs_vape_offset[0] +
				avs_vape_stack[0] - AVS_PETRA_BASEV;
	tempfloat_val /= AVS_PETRA_STEP;
	avs_vape_petra[0] = AVS_CEIL(tempfloat_val);

	tempfloat_val = avs_vape_offset[1] +
				avs_vape_stack[1] - AVS_PETRA_BASEV;
	tempfloat_val /= AVS_PETRA_STEP;
	avs_vape_petra[1] = AVS_CEIL(tempfloat_val);

	tempfloat_val = avs_vape_offset[2] +
				avs_vape_stack[2] - AVS_PETRA_BASEV;
	tempfloat_val /= AVS_PETRA_STEP;
	avs_vape_petra[2] = AVS_CEIL(tempfloat_val);

	return;
}

/**
 * @brief Fill the copy of the AVS structure with the new values.
 *        Do not change values you don't want as they will be copied back.
 * @param int avs_varm_enabled: bit stating if AVS on VARM is enabled.
 * @param int avs_vape_enabled: bit stating if AVS on VAPE is enabled.
 * @param unsigned char* avs_varm_petra: hex values for the voltages on VARM.
 * @param unsigned char* avs_vape_petra: hex values for the voltages on VAPE.
 * @param const int avs_inptr: pointer to a copy of the AVS struct used by PRCMU.
 * @return none.
 */
void avs_fill_avsstruct(int* avs_varm_enabled, int* avs_vape_enabled,
				unsigned char* avs_varm_petra,
				unsigned char* avs_vape_petra,
				const int* avs_inptr)
{
	struct avs* avs_data = (struct avs*)avs_inptr;

	/* Write new Varm voltages */
	if (avs_varm_enabled[0]) {
		/* 1000MHz not implemented yet */
		/*
		 * FIXME AVS_VARM_OPP also used,
		 * will have to make 800/1000 exclusive?
		 */
	}
	if (avs_varm_enabled[1])
		avs_data->varm.abb_reg_setting[AVS_VARM_OPP_100] =
					avs_varm_petra[1];
	if (avs_varm_enabled[2])
		avs_data->varm.abb_reg_setting[AVS_VARM_OPP_50] =
					avs_varm_petra[2];

	/* Write new Vape voltages */
	if (avs_vape_enabled[0]) {
		/* 333MHz not implemented yet */
		/* avs_data->vape.abb_reg_setting[AVS_VAPE_OPP_MAX] =
						avs_vape_petra[0];*/
	}
	if (avs_vape_enabled[1])
		avs_data->vape.abb_reg_setting[AVS_VAPE_OPP_133] =
					avs_vape_petra[1];
	if (avs_vape_enabled[2])
		avs_data->vape.abb_reg_setting[AVS_VAPE_OPP_100] =
					avs_vape_petra[2];

	return;
}

/**
 * @brief Make sure the voltage we are going to apply remains within limits.
 *	Note: AVS won't be applied and an error will be returned if failing.
 * @param int avs_varm_enabled: bit stating if AVS on VARM is enabled.
 * @param int avs_vape_enabled: bit stating if AVS on VAPE is enabled.
 * @param int* avs_varm_petra: hex values corresponds to the voltages on VARM.
 * @param int* avs_vape_petra: hex values corresponds to the voltages on VAPE.
 * @return EXIT_SUCCESS: successful
 * @return EXIT_FAILURE: error
 */
int avs_checkupperlimits(int* avs_varm_enabled, int* avs_vape_enabled,
				unsigned char* avs_varm_petra,
				unsigned char* avs_vape_petra)
{

	int mv_voltage; /* Temp var to store converted Petra value */
	int i;

	for (i = 0; i < MAX_AVS_ELEMENTS; i++) {
		/*
	 	 * Check enabled flag to avoid bad surprises,
	 	 * since failure means aborting
	 	 */
		if (avs_varm_enabled[i]) {
		 	/* Make sure we are below maximum voltage Petra */
			if (avs_varm_petra[i] > AVS_PETRA_MAXV)
				return AVS_EXIT_OUTOFLIMITS;

			/*
		 	 * Make sure we are below maximum sustained
		 	 * and transient voltage
		 	 */
			mv_voltage = avs_varm_petra[i] * AVS_PETRA_STEP
						+ AVS_PETRA_BASEV;
			if (mv_voltage > AVS_ARM_MAXVOLTAGE)
				return AVS_EXIT_OUTOFLIMITS;

			/*
			 * FIXME check against
			 * AVS_ARM_MAXTRVOLTAGE (mv_Voltage + upp. stack)
			 * Safe by design ATM
			 */
		}

		/*
	 	 * Check enabled flag to avoid bad surprises,
	 	 * since failure means aborting
	 	 */
		if (avs_vape_enabled[i]) {
			/* Make sure we are below maximum voltage Petra */
			if (avs_vape_petra[i] > AVS_PETRA_MAXV)
				return AVS_EXIT_OUTOFLIMITS;

			/*
		 	 * Make sure we are below maximum sustained
		 	 * and transient voltage
		 	 */
			mv_voltage = avs_vape_petra[i] * AVS_PETRA_STEP
						+ AVS_PETRA_BASEV;
			if (mv_voltage > AVS_APE_MAXVOLTAGE)
				return AVS_EXIT_OUTOFLIMITS;

			/*
			 * FIXME check against
			 * AVS_ARM_MAXTRVOLTAGE (mv_Voltage + upp. stack)
			 * Safe by design ATM
			 */
			/*
			 * FIXME Make sure Varray > Vperiphery – 150mV
			 * Safe by design ATM
			 */
		}
	}

	return AVS_EXIT_SUCCESS;
}

/**
 * @brief Make sure the voltage we are going to apply remains within limits.
 *        Note: AVS WILL BE applied, voltages will be set to minimum allowed.
 * @param int* avs_varm_offset: Minimum voltage in mV to be supplied on VARM.
 * @param int* avs_vape_offset: Minimum voltage in mV to be supplied on VAPE.
 * @return None.
 */
void avs_checklowerlimits(int* avs_varm_offset, int* avs_vape_offset)
{

	int i;

	for (i = 0; i < MAX_AVS_ELEMENTS; i++) {
		if(avs_varm_offset[i] < AVS_ARM_MINVOLTAGE)
			avs_varm_offset[i] = AVS_ARM_MINVOLTAGE;
		if(avs_vape_offset[i] < AVS_APE_MINVOLTAGE)
			avs_vape_offset[i] = AVS_APE_MINVOLTAGE;
	}

	return;
}

