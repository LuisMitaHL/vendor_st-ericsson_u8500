/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Author: Jens Wiklander <jens.wiklander@stericsson.com>
 *  for ST-Ericsson.
 */

#include <avs_calc.h>
#include <prcmu.h>
#include <ab8500.h>
#include <log.h>
#include <io.h>
#include <isswapi_types.h>
#include <soc_settings.h>
#include <config.h>

/*
 * The coding style in this file is not following what's used in the
 * rest of SoC settings. This is because the contents of this file
 * is imported from SoC settings released by SoC verification and
 * we'd like to make furthing imports as clean as possible.
 */

/* Mask for AVS fuse field */
/* FVR21 [31:0] */
#define thermal_sensor_mask    0x1F
#define vbbn_mask              0x7
#define vbbn_shift             5
#define vbbp_mask              0x7
#define vbbp_shift             8
#define arm_avs125_mask        0x1F
#define arm_avs125_shift       11
#define arm_avs100_mask        0x1F
#define arm_avs100_shift       16
#define arm_avs50_mask         0x1F
#define arm_avs50_shift        21
#define avs_revision_mask      0x7
#define avs_revision_shift     26
#define opp0_enable_mask       1
#define opp2_no_bias_mask      1
#define ret_no_bias_mask       1
#define ret_no_bias_shift      29
#define opp2_no_bias_shift     30
#define opp0_enable_shift      31

/* FVR22 [63:32] */
#define ape_avs100_mask        0xF
#define ape_avs100_shift       0
#define ape_avs50_mask         0xF
#define ape_avs50_shift        4
#define mod_avs100_mask        0xF
#define mod_avs100_shift       8
#define mod_avs50_mask         0xF
#define mod_avs50_shift        12
#define safe_avs100_mask       0xF
#define safe_avs100_shift      16
#define avs_enhancement_mask   0x3
#define avs_enhancement_shift  20
#define opp0_mask_mask         1
#define opp0_mask_shift        31

/* Constant input parameters for AVS data computation */
#define ab8505_step                6.25F
#define ab8500_step                12.5F
#define ab8500_ape_mod_step        25.0F
#define vtx_arm_opp1               1.15F
#define vtx_arm_ret                0.75F
#define vtx_ape_opp1               1.025F
#define vtx_mod_opp1               1.025F
#define vtx_safe_opp1              1.025F
#define delta_opp                  100
#define vdrop_arm_opp1             24
#define vdrop_ape_opp1             27
#define vdrop_mod_opp1             14
#define vdrop_safe_opp1            22
#define smps_accuracy              7.5F
#define smps_transient_arm         60
#define smps_transient_ape         30
#define smps_transient_mod         25
#define smps_transient_safe        25
#define temp_aging                 25
/* Max forward biasing voltage in active mode */
#define vbbf_max                   300
/* Max reverse biasing voltage in active mode */
#define vbbr_max                   100
#define vsmps_arm_max              1.35F
#define vsmps_arm_min_reg          0x42   /* 1.0125V */
#define vsmps_ape_min_reg          0x12   /* 0.925V */
#define vsmps_ape_min              0.925F
#define vsmps_mod_min_reg          0x12   /* 0.925V */
#define vsmps_mod_min              0.925F
#define pcb_drop                   5      /* 5 mV */
#define smps_safe_ret		   0.95F  /* 0.95V */
#define vsmps_ape_mod_max_reg	   0x2A	  /* 1.225V */
#define vsafe_trans_overshoot_opp1 25
#define vsafe_trans_overshoot_opp2 12.5F
#define vsafe_smps_array	   1.2F
#define vsafe_stack_correction	   25	/* 25mV */

#define VBB_MAX_OPP 0xDB

/*
 * Settings in this sequence
 * VBBN/P ret%, VBBN/P 125%, VBBN/P 100%, VBBN/P 50%,
 * En/SR/VARM 125%, VARM 100%, VARM 50%, VARM retention,
 * VAPE 100%, VAPE 50%, VMOD 100%, VMOD 50%,
 * VSAFE NOT DSLEEP, VSAFE DSLEEP; HJ, Unused,
 * HJ/AE, VTX-ARM100, VTX-APE100, VTX-MOD100,
 * VTX-SAVE100, DELTA-OPP, VDrop-ARM100, Vdrop-APE100,
 * Vdrop-MOD100, Vdrop-SAFE100, SMPS-Accuracy, SMPS-Transcient,
 * TEMP-Offset, VSMPS-ARM-RETENTION, VSMPS-SAFE-DSLEEP, Unused,
 */

static u8 avs_payload_v1_overclock[] = {
	/* Generic default values */
	0x00, 0x6D, 0x6D, 0x6D, 0xF8, 0x68, 0x44, 0x18,
	0x22, 0x16, 0x22, 0x18, 0x28, 0x14, 0x00, 0xFF,
	/* Non fused parameters needed to compute AVS regulators */
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};
static u8 avs_payload_overclock[] = {
	/* Generic default values */
	0x00, 0xDD, 0xDD, 0xDD, 0xF8, 0x68, 0x44, 0x18,
	0x22, 0x16, 0x22, 0x18, 0x28, 0x14, 0x00, 0xFF,
	/* Non fused parameters needed to compute AVS regulators */
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};
static u8 avs_payload_v1[] = {
	/* Generic default values */
	0x00, 0x6D, 0x6D, 0x6D, 0x78, 0x68, 0x44, 0x18,
	0x22, 0x16, 0x22, 0x18, 0x28, 0x14, 0x00, 0xFF,
	/* Non fused parameters needed to compute AVS regulators */
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};
static u8 avs_payload[] = {
	/* Generic default values */
	0x00, 0xDD, 0xDD, 0xDD, 0x78, 0x68, 0x44, 0x18,
	0x22, 0x16, 0x22, 0x18, 0x28, 0x14, 0x00, 0xFF,
	/* Non fused parameters needed to compute AVS regulators */
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};

/* Function that returns the two-complement of a number */
static int get_two_complement(int value, unsigned nb_bit, unsigned mask)
{
	if ((value >> (nb_bit-1)) & 0x1) {     /* 2-comp negative number */
		value = (((~(value & mask)) & mask) + 1);
		value = -value;
		return value;
	} else
		return value;
}

static unsigned char get_cut_id(void)
{
	unsigned char cutID;
	hwi2c_read(AB8500_REV, &cutID);
	return cutID;
}

__attribute__((section(".AVS")))
void avs8420_calculate_settings(u32 avs_fuses[2], void *avs_data)
{
	u32 FVR21_data;
	u32 FVR22_data;
	t_AVSData *p_AVSData = avs_data;
	/* Output parameters */
	float volt_stack_arm_opp0;
	float volt_stack_arm_opp1;
	float volt_stack_arm_opp2;
	float volt_stack_ape_opp1;
	float volt_stack_ape_opp2;
	float volt_stack_mod_opp1;
	float volt_stack_mod_opp2;
	float volt_stack_safe_opp1;

	/* Output Parameters */
	unsigned vbbn;
	unsigned vbbp;
	int arm_avs125offset;
	int arm_avs100offset;
	int arm_avs50offset;
	unsigned ret_no_bias;
	unsigned opp0_enable;
	int ape_avs100offset;
	int ape_avs50offset;
	int mod_avs100offset;
	int mod_avs50offset;
	int safe_avs100offset;
	int avs_enhance;

	float arm_smps_opp0;
	float arm_smps_opp1;
	float arm_smps_opp2;
	float ape_smps_opp1;
	float ape_smps_opp2;
	float mod_smps_opp1;
	float mod_smps_opp2;
	float safe_smps_opp1;
	float arm_smps_opp0_r;
	float arm_smps_opp2_r;
	float ape_smps_opp1_r;
	float ape_smps_opp2_r;
	float mod_smps_opp2_r;
	float mod_smps_opp1_r;
	char arm_smps_opp0_reg;
	char arm_smps_opp1_reg;
	char arm_smps_opp2_reg;
	char arm_smps_ret_reg;
	char ape_smps_opp1_reg;
	char ape_smps_opp2_reg;
	char mod_smps_opp1_reg;
	char mod_smps_opp2_reg;
	char safe_smps_opp1_reg;
	char safe_smps_ret_reg;

	float v_array;
	float v_periph;

	unsigned char vbbn_reg = 0;
	unsigned char vbbp_reg = 0;

	/* Read back AVS fuse value (FVR21 and FVR22) */
	FVR21_data = avs_fuses[0];
	FVR22_data = avs_fuses[1];

	/* Extract field from fuse data	 */
	vbbn = (FVR21_data >> vbbn_shift) & vbbn_mask;
	vbbp = (FVR21_data >> vbbp_shift) & vbbp_mask;

	arm_avs125offset = (FVR21_data >> arm_avs125_shift) & arm_avs125_mask;
	arm_avs125offset = get_two_complement(arm_avs125offset, 5, 0xF);

	arm_avs100offset = (FVR21_data >> arm_avs100_shift) & arm_avs100_mask;
	arm_avs100offset = get_two_complement(arm_avs100offset, 5, 0xF);

	arm_avs50offset = (FVR21_data >> arm_avs50_shift) & arm_avs50_mask;
	arm_avs50offset = get_two_complement(arm_avs50offset, 5, 0xF);

	ret_no_bias = (FVR21_data >> ret_no_bias_shift) & ret_no_bias_mask;

	opp0_enable = (FVR21_data >> opp0_enable_shift) & opp0_enable_mask;

	ape_avs100offset = (FVR22_data >> ape_avs100_shift) & ape_avs100_mask;
	ape_avs100offset = get_two_complement(ape_avs100offset, 4, 0x7);

	ape_avs50offset = (FVR22_data >> ape_avs50_shift) & ape_avs50_mask;
	ape_avs50offset = get_two_complement(ape_avs50offset, 4, 0x7);

	mod_avs100offset = (FVR22_data >> mod_avs100_shift) & mod_avs100_mask;
	mod_avs100offset = get_two_complement(mod_avs100offset, 4, 0x7);

	mod_avs50offset = (FVR22_data >> mod_avs50_shift) & mod_avs50_mask;
	mod_avs50offset = get_two_complement(mod_avs50offset, 4, 0x7);

	safe_avs100offset = (FVR22_data >> safe_avs100_shift) & \
		safe_avs100_mask;
	safe_avs100offset = get_two_complement(safe_avs100offset, 4, 0x7);

	avs_enhance = (FVR22_data >> avs_enhancement_shift) & avs_enhancement_mask;

	/* Fill AVS Data field for PRCMU service */
	/* Varm biasing values	=> AVSData->bias */
	/* Convert fuse data to AB8505 register value */
	/* VBBN values */

	if ((vbbn == 0x4) || (vbbn == 0x5))
		/* -400mV / -300mV => CLAMP :  VBBN = -100mV (RBB) */
		vbbn_reg = 0x2;
	else if (vbbn == 0x6)
		/* -200mV => CLAMP : VBBN = -100mV (RBB) */
		vbbn_reg = 0x2;
	else if (vbbn == 0x7)
		/* -100mV => VBBN = -100mV (RBB) */
		vbbn_reg = 0x2;
	else if (vbbn == 0x0)
		/* 0mV => VBBN = 0mV (no biasing) */
		vbbn_reg = 0x0;
	else if (vbbn == 0x1)
		/* +100mV => VBBN = 100mV (FBB) */
		vbbn_reg = 0x9;
	else if (vbbn == 0x2)
		/* +200mV => VBBN = 200mV (FBB) */
		vbbn_reg = 0xB;
	else if (vbbn == 0x3)
		/*  +300mV => VBBN = 300mV (FBB) */
		vbbn_reg = 0xD;

	/* VBBP values */

	if (get_cut_id() == CUT_ID_AB8500_V1) {
		/* If CutID = 0x10 */
		loginfo("AB8505 V1 detected");
		/* Check Vbbp_400 bit */
		if ((vbbp == 0x4) || (vbbp == 0x5))
			/* -400mV / -300mV => CLAMP : VBBP = VDDCPU + 100mV (RBB) */
			vbbp_reg = 0x9;
		else if (vbbp == 0x6)
			/* -200mV => CLAMP : VBBP = VDDCPU + 100mV (RBB) */
			vbbp_reg = 0x9;
		else if (vbbp == 0x7)
			/* -100mV => VBBP = VDDCPU + 100mV (RBB) */
			vbbp_reg = 0x9;
		else if (vbbp == 0x0)
			/* 0mV => VBBP = VDDCPU (no biasing) */
			vbbp_reg = 0x0;
		else if (vbbp == 0x1)
			/* +100mV => VBBP = VDDCPU - 100mV (FBB) */
			vbbp_reg = 0x2;
		else if (vbbp == 0x2)
			/* +200mV => VBBP = VDDCPU - 200mV (FBB) */
			vbbp_reg = 0x4;
		else if (vbbp == 0x3)
			/* +300mV => VBBP = VDDCPU - 300mV (FBB) */
			vbbp_reg = 0x6;
	}
	else {
		/* If CutID = 0x20 */
		loginfo("AB8505 V2 detected");
		/* Check Vbbp_400 bit */
		if ((vbbp == 0x4) || (vbbp == 0x5))
			/* -400mV / -300mV => CLAMP : VBBP = VDDCPU + 100mV (RBB) */
			vbbp_reg = 0x2;
		else if (vbbp == 0x6)
			/* -200mV => CLAMP : VBBP = VDDCPU + 100mV (RBB) */
			vbbp_reg = 0x2;
		else if (vbbp == 0x7)
			/* -100mV => VBBP = VDDCPU + 100mV (RBB) */
			vbbp_reg = 0x2;
		else if (vbbp == 0x0)
			/* 0mV => VBBP = VDDCPU (no biasing) */
			vbbp_reg = 0x0;
		else if (vbbp == 0x1)
			/* +100mV => VBBP = VDDCPU - 100mV (FBB) */
			vbbp_reg = 0x9;
		else if (vbbp == 0x2)
			/* +200mV => VBBP = VDDCPU - 200mV (FBB) */
			vbbp_reg = 0xB;
		else if (vbbp == 0x3)
			/* +300mV => VBBP = VDDCPU - 300mV (FBB) */
			vbbp_reg = 0xD;
	}

	/* Biasing during retention mode */
	/* Vbbp in retention always 0 */
	if ((vbbn_reg <= 3) && (ret_no_bias == 0))
		/* Vbbn same in active and retention mode */
		p_AVSData->Voltage[AVS_VBB_RET] =  (0 << 4) | vbbn_reg;
	else
		p_AVSData->Voltage[AVS_VBB_RET] =  0; /* No biasing */
	p_AVSData->Voltage[AVS_VBB_MAX_OPP] =  (vbbp_reg << 4) | vbbn_reg;
	p_AVSData->Voltage[AVS_VBB_100_OPP] =  (vbbp_reg << 4) | vbbn_reg;
	p_AVSData->Voltage[AVS_VBB_50_OPP] =  (vbbp_reg << 4) | vbbn_reg;


	/* Voltage stack per OPP */
	/* Add 2 step on Varm OPP0/OPP1 if VBBP fused at FBB400mV */
	volt_stack_arm_opp0 = (125.0/100.0)*(vdrop_arm_opp1 + smps_transient_arm) + \
		smps_accuracy + temp_aging;
	volt_stack_arm_opp1 = vdrop_arm_opp1 + \
		smps_transient_arm + smps_accuracy + temp_aging;
	volt_stack_arm_opp2 = (50.0/100.0)*(vdrop_arm_opp1 + \
		smps_transient_arm) + smps_accuracy + temp_aging;
	volt_stack_ape_opp1 = vdrop_ape_opp1 + \
		smps_transient_ape + smps_accuracy + temp_aging + pcb_drop;
	volt_stack_ape_opp2 = (50.0/100.0)*(vdrop_ape_opp1 + \
		smps_transient_ape + pcb_drop) + smps_accuracy + temp_aging;
	volt_stack_mod_opp1 = vdrop_mod_opp1 + \
		smps_transient_mod + smps_accuracy + temp_aging + pcb_drop;
	volt_stack_mod_opp2 = (50.0/100.0)*(vdrop_mod_opp1 + \
		smps_transient_mod + pcb_drop) + smps_accuracy + temp_aging;
	volt_stack_safe_opp1 = vdrop_safe_opp1 + \
		smps_transient_safe + smps_accuracy + temp_aging + pcb_drop;

	/* Voltage table for all power domain (max, 100 and 50 OPP) */

	/*
	 * VarmMaxOPP for ARM
	 */
	/*
	 * Check if Marketing bit is enabled (OPP0 BOOST MASK)
	 * if OPP0 BOOST MASK = 1 => 1.15GHz capable masked
	 * if OPP0 BOOST MASK = 0 => 1.15GHz capable allowed
	 */
	arm_smps_opp0 = ((arm_avs125offset*ab8500_step) + vtx_arm_opp1*1000)
		 + volt_stack_arm_opp0;

	/* Round to 12.5mV */
	if ((int)(arm_smps_opp0/ab8500_step) < (arm_smps_opp0/ab8500_step)) {
		arm_smps_opp0_r = (((int)(arm_smps_opp0/ab8500_step) \
			+ 1)*ab8500_step)/1000;
		arm_smps_opp0_reg = (char)(((((int)(arm_smps_opp0/ \
			ab8500_step) + 1)*ab8500_step) - 600.0)/ab8505_step);
	} else {
		arm_smps_opp0_r = (((int)(arm_smps_opp0/ \
			ab8500_step))*ab8500_step)/1000;
		arm_smps_opp0_reg = (char)(((((int)(arm_smps_opp0/ \
			ab8500_step))*ab8500_step) - 600.0)/ab8505_step);
	}

	/*
	 * Process limitation: 1.40V is the absolute maximum voltage at SMPS level
	 * If (VSMPS_arm_opp0 > VSMPS_arm_max_boost) then OPP0 operating point shall be disabled
	 */
	if (arm_smps_opp0_r > vsmps_arm_max)
		opp0_enable = 0;

	/*
	 * Varm100OPP for ARM
	 */
	arm_smps_opp1 = ((arm_avs100offset*ab8500_step) + \
		vtx_arm_opp1*1000) + volt_stack_arm_opp1;
	/* Round to 12.5mV */
	if ((int)(arm_smps_opp1/ab8500_step) < \
		(arm_smps_opp1/ab8500_step))
		arm_smps_opp1_reg = (char)(((((int)(arm_smps_opp1/ab8500_step) \
			+ 1)*ab8500_step) - 600.0)/ab8505_step);
	else
		arm_smps_opp1_reg = (char)(((((int)(arm_smps_opp1/ \
			ab8500_step))*ab8500_step) - 600.0)/ab8505_step);

	/*
	 * Varm50OPP for ARM
	 */
	arm_smps_opp2 = ((arm_avs50offset*ab8500_step) + \
		(vtx_arm_opp1*1000 - delta_opp)) + volt_stack_arm_opp2;
	/* Round to 12.5mV */
	if ((int)(arm_smps_opp2/ab8500_step) < \
		(arm_smps_opp2/ab8500_step)) {
		arm_smps_opp2_r = (((int)(arm_smps_opp2/ab8500_step)  \
			+ 1)*ab8500_step)/1000;
		arm_smps_opp2_reg = (char)(((((int)(arm_smps_opp2/ab8500_step) \
			+ 1)*ab8500_step) - 600.0)/ab8505_step);
	} else {
		arm_smps_opp2_r = (((int)(arm_smps_opp2/ab8500_step)) \
			*ab8500_step)/1000;
		arm_smps_opp2_reg = (char)(((((int)(arm_smps_opp2/ \
			ab8500_step))*ab8500_step) - 600.0)/ab8505_step);
	}

	/*
	 * Process limitation:	0.95V is the absolute minimum voltage
	 * at transistor level during active mode for ARM power domain
	 */
	if (((arm_smps_opp2_r*1000) - volt_stack_arm_opp2) <= 950)
		arm_smps_opp2_reg = vsmps_arm_min_reg;

	/*
	 * VarmRet for ARM
	 */
	/* Round to 12.5mV */
	if ((int)((vtx_arm_ret*1000)/ab8500_step) < \
		((vtx_arm_ret*1000)/ab8500_step))
		arm_smps_ret_reg = (char)(((((int)((vtx_arm_ret*1000)/ \
			ab8500_step) + 1)*ab8500_step) - 600.0)/ab8505_step);
	else
		arm_smps_ret_reg = (char)(((((int)((vtx_arm_ret*1000)/ \
			ab8500_step))*ab8500_step) - 600.0)/ab8505_step);


	/*
	 * Vape100OPP for APE
	 */
	ape_smps_opp1 = ((ape_avs100offset*ab8500_ape_mod_step) \
		+ vtx_ape_opp1*1000) + volt_stack_ape_opp1;

	/* Round to 25mV */
	if ((int)(ape_smps_opp1/ab8500_ape_mod_step) < \
	    (ape_smps_opp1/ab8500_ape_mod_step)) {
		ape_smps_opp1_r = (((int)(ape_smps_opp1/
			ab8500_ape_mod_step) + 1)*ab8500_ape_mod_step)/1000;
		ape_smps_opp1_reg = (char)(((((int)(ape_smps_opp1/ \
			ab8500_ape_mod_step) + 1)*ab8500_ape_mod_step) - \
			700.0)/ab8500_step);
	} else {
		ape_smps_opp1_r = (((int)(ape_smps_opp1/
			ab8500_ape_mod_step))*ab8500_ape_mod_step)/1000;
		ape_smps_opp1_reg = (char)(((((int)(ape_smps_opp1/ \
			ab8500_ape_mod_step))*ab8500_ape_mod_step) - \
			700.0)/ab8500_step);
	}

	/* Process limitation: Vape SMPS max */
	if (ape_smps_opp1_reg > vsmps_ape_mod_max_reg)
		ape_smps_opp1_reg = vsmps_ape_mod_max_reg;

	/*
	 * Vape50OPP for APE
	 */
	ape_smps_opp2 = ((ape_avs50offset*ab8500_ape_mod_step) + \
		(vtx_ape_opp1*1000 - delta_opp)) + volt_stack_ape_opp2;

	/* Round to 25mV */
	if ((int)(ape_smps_opp2/ab8500_ape_mod_step) \
			< (ape_smps_opp2/ab8500_ape_mod_step)) {
		ape_smps_opp2_r = (((int)(ape_smps_opp2/ \
			ab8500_ape_mod_step) + 1)*ab8500_ape_mod_step)/1000;
		ape_smps_opp2_reg = (char)(((((int)(ape_smps_opp2/ \
			ab8500_ape_mod_step) + 1)*ab8500_ape_mod_step) - \
			700.0)/ab8500_step);
	} else {
		ape_smps_opp2_r = (((int)(ape_smps_opp2/ \
			ab8500_ape_mod_step))*ab8500_ape_mod_step)/1000;
		ape_smps_opp2_reg = (char)(((((int)(ape_smps_opp2/ \
			ab8500_ape_mod_step))*ab8500_ape_mod_step) - \
			700.0)/ab8500_step);
	}

	/* Check AVS enhancement (fused value not aligned with AVS signoff for enhancement 00 and 01) */
	if ((avs_enhance == 0)||(avs_enhance == 1))
		ape_smps_opp2_reg = ape_smps_opp2_reg + 0x2;  /* add 1 step APE/MOD (25mV) */

	/*
	 * Process limitation:	0.85V is the absolute minimum voltage
	 * at transistor level during active mode for APE power domain
	 */
	if (((ape_smps_opp2_r*1000) - volt_stack_ape_opp2) <= 850) {
		ape_smps_opp2_reg = vsmps_ape_min_reg;
		ape_smps_opp2_r = vsmps_ape_min;
	}

	/*
	 * Vmod100OPP for MODEM
	 */
	mod_smps_opp1 = ((mod_avs100offset*ab8500_ape_mod_step) \
		+ vtx_mod_opp1*1000) + volt_stack_mod_opp1;
	/* Round to 25mV */
	if ((int)(mod_smps_opp1/ab8500_ape_mod_step) \
	    < (mod_smps_opp1/ab8500_ape_mod_step)) {
		mod_smps_opp1_r = (((int)(mod_smps_opp1/ab8500_ape_mod_step) + 1)*ab8500_ape_mod_step)/1000;
		mod_smps_opp1_reg = (char)(((((int)(mod_smps_opp1/ \
		ab8500_ape_mod_step) + 1)*ab8500_ape_mod_step) - \
		700.0)/ab8500_step);
	} else {
		mod_smps_opp1_r = (((int)(mod_smps_opp1/ab8500_ape_mod_step))*ab8500_ape_mod_step)/1000;
		mod_smps_opp1_reg = (char)(((((int)(mod_smps_opp1/ \
			ab8500_ape_mod_step))*ab8500_ape_mod_step) - \
			700.0)/ab8500_step);
	}
	/* Process limitation: Vmodem SMPS max */
	if (mod_smps_opp1_reg > vsmps_ape_mod_max_reg)
		mod_smps_opp1_reg = vsmps_ape_mod_max_reg;

	/*
	 * Vmod50OPP for MODEM
	 */
	mod_smps_opp2 = ((mod_avs50offset*ab8500_ape_mod_step) + \
		(vtx_mod_opp1*1000 - delta_opp)) + volt_stack_mod_opp2;
	/* Round to 25mV */
	if ((int)(mod_smps_opp2/ab8500_ape_mod_step) < \
			(mod_smps_opp2/ab8500_ape_mod_step)) {
		mod_smps_opp2_r = (((int)(mod_smps_opp2/ \
			ab8500_ape_mod_step) + 1)*ab8500_ape_mod_step)/1000;
		mod_smps_opp2_reg = (char)(((((int)(mod_smps_opp2/ \
			ab8500_ape_mod_step) + 1)*ab8500_ape_mod_step) - \
			700.0)/ab8500_step);
	} else {
		mod_smps_opp2_r = (((int)(mod_smps_opp2/ \
			ab8500_ape_mod_step))*ab8500_ape_mod_step)/1000;
		mod_smps_opp2_reg = (char)(((((int)(mod_smps_opp2/ \
			ab8500_ape_mod_step))*ab8500_ape_mod_step) - \
			700.0)/ab8500_step);
	}

	/*
	 * Process limitation:	0.85V is the absolute minimum voltage at
	 * transistor level during active mode for MODEM power domain
	 */
	if (((mod_smps_opp2_r*1000) - volt_stack_mod_opp2) <= 850) {
		mod_smps_opp2_reg = vsmps_mod_min_reg;
		ape_smps_opp2_r = vsmps_ape_min;
	}

	/*
	 * Vsafe100OPP for SAFE
	 */
	safe_smps_opp1 = ((safe_avs100offset*ab8500_ape_mod_step) \
		+ vtx_safe_opp1*1000) + volt_stack_safe_opp1;
	/* Round to 25mV */
	if ((int)(safe_smps_opp1/ab8500_ape_mod_step) < \
			(safe_smps_opp1/ab8500_ape_mod_step))
		safe_smps_opp1_reg = (char)(((((int)(safe_smps_opp1/ \
			ab8500_ape_mod_step) + 1)*ab8500_ape_mod_step) - \
			700.0)/ab8500_step);
	else
		safe_smps_opp1_reg = (char)(((((int)(safe_smps_opp1/ \
			ab8500_ape_mod_step))*ab8500_ape_mod_step) - \
			700.0)/ab8500_step);

	/* VSafeRet for SAFE */
	/* Round to 25mV */
	if ((int)((smps_safe_ret*1000)/ab8500_ape_mod_step) <
	    ((smps_safe_ret*1000)/ab8500_ape_mod_step))
		safe_smps_ret_reg = (char)(((((int)((smps_safe_ret*1000)/ \
		ab8500_ape_mod_step) + 1)*ab8500_ape_mod_step) - 700.0)/ab8500_step);
	else
		safe_smps_ret_reg = (char)(((((int)((smps_safe_ret*1000)/ \
		ab8500_ape_mod_step))*ab8500_ape_mod_step) - 700.0)/ab8500_step);

	/* Rules applied for APE OPP1: */
	v_array = vsafe_smps_array*1000 + vsafe_trans_overshoot_opp1 + smps_accuracy;
	v_periph = ape_smps_opp1_r*1000 -
		(smps_accuracy + pcb_drop + vdrop_ape_opp1 + smps_transient_ape + temp_aging);
	if ((v_array-v_periph) >= 350)
		ape_smps_opp1_reg = ape_smps_opp1_reg + 0x2; /* add 1 step APE/MOD (25mV) */

	/* Rules applied for APE OPP2: */
	v_array = vsafe_smps_array*1000 + vsafe_trans_overshoot_opp2 + smps_accuracy;
	v_periph = ape_smps_opp2_r*1000 -
		(smps_accuracy + (pcb_drop + vdrop_ape_opp1 + smps_transient_ape)/2 + temp_aging);
	if ((v_array-v_periph) >= 350)
		ape_smps_opp2_reg = ape_smps_opp2_reg + 0x2; /* add 1 step APE/MOD (25mV) */

	/* Rules applied for MODEM OPP1: */
	v_array = vsafe_smps_array*1000 + vsafe_trans_overshoot_opp1 + smps_accuracy;
	v_periph = mod_smps_opp1_r*1000 -
		(smps_accuracy + pcb_drop + vdrop_mod_opp1 + smps_transient_mod + temp_aging);
	if ((v_array-v_periph) >= 350)
		mod_smps_opp1_reg = mod_smps_opp1_reg + 0x2;  /* add 1 step APE/MOD (25mV) */

	/* Rules applied for MODEM OPP2: */
	v_array = vsafe_smps_array*1000 + vsafe_trans_overshoot_opp2 + smps_accuracy;
	v_periph = mod_smps_opp2_r*1000 -
		(smps_accuracy + (pcb_drop + vdrop_mod_opp1 + smps_transient_mod)/2 + temp_aging);
	if ((v_array-v_periph) >= 350)
		mod_smps_opp2_reg = mod_smps_opp2_reg + 0x2;  /* add 1 step APE/MOD (25mV) */

	/*
	 * fill AVSData structure
	 */
	/* by default allow MAX opp. This is the aim of MASK_BIT7. */
	p_AVSData->Voltage[AVS_VARM_MAX_OPP] = (opp0_enable<<7)+arm_smps_opp0_reg;
	p_AVSData->Voltage[AVS_VARM_100_OPP]   = arm_smps_opp1_reg;
	p_AVSData->Voltage[AVS_VARM_50_OPP]   = arm_smps_opp2_reg;
	p_AVSData->Voltage[AVS_VARM_RET]   = arm_smps_ret_reg;
	p_AVSData->Voltage[AVS_VAPE_100_OPP]   = ape_smps_opp1_reg;
	p_AVSData->Voltage[AVS_VAPE_50_OPP]   = ape_smps_opp2_reg;
	p_AVSData->Voltage[AVS_VMOD_100_OPP]   = mod_smps_opp1_reg;
	p_AVSData->Voltage[AVS_VMOD_50_OPP]   = mod_smps_opp2_reg;
	p_AVSData->Voltage[AVS_VSAFE]   = safe_smps_opp1_reg;
	p_AVSData->Voltage[AVS_VSAFE_RET]   = safe_smps_ret_reg;

}

int avs8420_is_maxopp_fused(u32 avs_fuses[2])
{
	/* Check 1 GHz fuse in FVR21 bit 31 */
	return (avs_fuses[0] >> opp0_enable_shift) & opp0_enable_mask;
}

static unsigned read_biasing_fuse(u32 fuses[2])
{
	unsigned vbbn;
	unsigned vbbp;
	unsigned char vbbn_reg = 0;
	unsigned char vbbp_reg = 0;
	unsigned char BB_reg_value;

	/* Check if biasing is fused */

	if (is_fused(fuses)) {
		vbbn = (fuses[0] >> vbbn_shift) & vbbn_mask;
		vbbp = (fuses[0] >> vbbp_shift) & vbbp_mask;

		/* VBBN values */
		/* Check Vbbn_400 bit */
		if ((vbbn == 0x4) || (vbbn == 0x5))
			/* -400mV/-300mV => CLAMP: VBBN = -100mV (RBB) */
			vbbn_reg = 0x2;
		else if (vbbn == 0x6)
			/* -200mV => CLAMP: VBBN = -100mV (RBB) */
			vbbn_reg = 0x2;
		else if (vbbn == 0x7)
			/* -100mV => VBBN = -100mV (RBB) */
			vbbn_reg = 0x2;
		else if (vbbn == 0x0)
			/* 0mV => VBBN = 0mV (no biasing) */
			vbbn_reg = 0x0;
		else if (vbbn == 0x1)
			/* +100mV => VBBN = 100mV (FBB) */
			vbbn_reg = 0x9;
		else if (vbbn == 0x2)
			/* +200mV => VBBN = 200mV (FBB) */
			vbbn_reg = 0xB;
		else if (vbbn == 0x3)
			/* +300mV => VBBN = 300mV (FBB) */
			vbbn_reg = 0xD;

		/* VBBP values */
		if (get_cut_id() == CUT_ID_AB8500_V1) {
			/* Check Vbbp_400 bit */
			if ((vbbp == 0x4) || (vbbp == 0x5))
				/* -400mV/-300mV => CLAMP: VBBP = VDDCPU + 100mV (RBB) */
				vbbp_reg = 0x9;
			else if (vbbp == 0x6)
				/* -200mV => CLAMP: VBBP = VDDCPU + 100mV (RBB) */
				vbbp_reg = 0x9;
			else if (vbbp == 0x7)
				/* -100mV => VBBP = VDDCPU + 100mV (RBB) */
				vbbp_reg = 0x9;
			else if (vbbp == 0x0)
				/* 0mV => VBBP = VDDCPU (no biasing) */
				vbbp_reg = 0x0;
			else if (vbbp == 0x1)
				/* +100mV => VBBP = VDDCPU - 100mV (FBB) */
				vbbp_reg = 0x2;
			else if (vbbp == 0x2)
				/* +200mV => VBBP = VDDCPU - 200mV (FBB) */
				vbbp_reg = 0x4;
			else if (vbbp == 0x3)
				/* +300mV => VBBP = VDDCPU - 300mV (FBB) */
				vbbp_reg = 0x6;
		} else {
			/* Check Vbbp_400 bit */
			if ((vbbp == 0x4) || (vbbp == 0x5))
				/* -400mV/-300mV => CLAMP: VBBP = VDDCPU + 100mV (RBB) */
				vbbp_reg = 0x2;
			else if (vbbp == 0x6)
				/* -200mV => CLAMP: VBBP = VDDCPU + 100mV (RBB) */
				vbbp_reg = 0x2;
			else if (vbbp == 0x7)
				/* -100mV => VBBP = VDDCPU + 100mV (RBB) */
				vbbp_reg = 0x2;
			else if (vbbp == 0x0)
				/* 0mV => VBBP = VDDCPU (no biasing) */
				vbbp_reg = 0x0;
			else if (vbbp == 0x1)
				/* +100mV => VBBP = VDDCPU - 100mV (FBB) */
				vbbp_reg = 0x9;
			else if (vbbp == 0x2)
				/* +200mV => VBBP = VDDCPU - 200mV (FBB) */
				vbbp_reg = 0xB;
			else if (vbbp == 0x3)
				/* +300mV => VBBP = VDDCPU - 300mV (FBB) */
				vbbp_reg = 0xD;
		}

		BB_reg_value = (vbbp_reg << 4) | vbbn_reg;
	} else {
		BB_reg_value = VBB_MAX_OPP;
	}
	return (BB_reg_value);
}


/* calculate avs settings for non avs */
void avs8420_no_avs_calculate_settings(u32 avs_fuses[2], void *avs_data)
{
	unsigned char BB_rv;
	unsigned n;
	u8 *avd = avs_data;
	u8 *avs_pl;
	struct config *config = get_config();

	/* read base */

	if (get_cut_id() == CUT_ID_AB8500_V1) {
		loginfo("AB8505 V1 detected");
		if (config->overclock > OVERCLOCK_800MHZ)
			avs_pl = avs_payload_v1_overclock;
		else
			avs_pl = avs_payload_v1;
	}
	else {
		loginfo("AB8505 V2 detected");
		if (config->overclock > OVERCLOCK_800MHZ)
			avs_pl = avs_payload_overclock;
		else
			avs_pl = avs_payload;
	}

	for (n = 0; n < sizeof(t_AVSData); n++)
		avd[n] = avs_pl[n];


	BB_rv = read_biasing_fuse(avs_fuses);
	avd[1] = BB_rv;
	avd[2] = BB_rv;
	avd[3] = BB_rv;
	/* Remove MAX_OPP bit if configured for MAX_OPP but not fused for it */
	if ((config->overclock > OVERCLOCK_800MHZ) &&
	    (is_fused(avs_fuses) && !is_maxopp_fused(avs_fuses))) {
		avd[4] &= ~0x80;
	}
}
