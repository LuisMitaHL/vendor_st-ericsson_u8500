#ifndef AVUTILS_AVS_H
#define AVUTILS_AVS_H

/* AVS release: v2.0.2 */


/* Function prototypes */
int avs_configure(unsigned int avs_fuseword0, unsigned int avs_fuseword1,
                                                          const int* avs_inptr);

void avs_r1_read_fuses(unsigned int avs_fuseword0, unsigned int avs_fuseword1,
                        int* avs_varm_enabled, int* avs_vape_enabled,
                         int* avs_varm_offset, int* avs_vape_offset);

void avs_r2_read_fuses(unsigned int avs_fuseword0, unsigned int avs_fuseword1,
                        int* avs_varm_enabled, int* avs_vape_enabled,
                         int* avs_varm_offset, int* avs_vape_offset);

int avs_twos_compl(int value, unsigned int nb_bits, unsigned int mask);

void avs_computestack(float* avs_varm_stack, float* avs_vape_stack);

void avs_computepetra(unsigned char* avs_varm_petra,
                       unsigned char* avs_vape_petra, int* avs_varm_offset,
                        int* avs_vape_offset, float* avs_varm_stack,
                         float* avs_vape_stack);

void avs_fill_avsstruct(int* avs_varm_enabled, int* avs_vape_enabled,
                        unsigned char* avs_varm_petra,
                         unsigned char* avs_vape_petra,
                          const int* avs_inptr);

int avs_checkupperlimits(int* avs_varm_enabled, int* avs_vape_enabled,
                          unsigned char* avs_varm_petra,
                           unsigned char* avs_vape_petra);

void avs_checklowerlimits(int* avs_varm_offset, int* avs_vape_offset);

/* Exit cases */
#define AVS_EXIT_SUCCESS 0 /**< AVS is ready to be activated */
#define AVS_EXIT_UNFUSEDDEVICE 1 /**< The device is not fused */
#define AVS_EXIT_UNKNOWNREV 2 /**< The AVS revision is unknown */
#define AVS_EXIT_OUTOFLIMITS 3 /**< At least one required voltage is too high */

/* Revision field at this position in Fuseword 0 */
#define AVS_REVISION 0x07000000
#define AVS_REVISION_SHIFT 0x18

/* AVS Fuseword 0 */
/* Rev 0: Prototypes only */
/* Rev 1 mapping */
#define AVS_R1_ARM400_OFFSET 0x0003FE00
#define AVS_R1_ARM400_OFFSET_SHIFT 0x9
#define AVS_R1_ARM800_OFFSET 0x000001FF
#define AVS_R1_ARM800_OFFSET_SHIFT 0x0
#define AVS_R1_ARM_OFFSETCONV 1 /**< Fused value Y means offset is Y x 1 mV */
/* Rev 2 mapping */
#define AVS_R2_ARM400_OFFSET 0x00FF0000
#define AVS_R2_ARM400_OFFSET_SHIFT 0x10
#define AVS_R2_ARM800_OFFSET 0x0000FF00
#define AVS_R2_ARM800_OFFSET_SHIFT 0x8
#define AVS_R2_ARM1000_OFFSET 0x000000FF
#define AVS_R2_ARM1000_OFFSET_SHIFT 0x0
#define AVS_R2_ARM_OFFSETCONV 2 /**< Fused value Y means offset is Y x 2 mV */

/* AVS Fuseword 1*/
/* Rev 0: Prototypes only */
/* Rev 1 mapping */
#define AVS_R1_APE_ENABLE 0x00080000
#define AVS_R1_APE_ENABLE_SHIFT 0x13
#define AVS_R1_ARM_ENABLE 0x00040000
#define AVS_R1_ARM_ENABLE_SHIFT 0x12
#define AVS_R1_APE_OFFSETCONV 1 /**< Fused value Y means offset is Y x 1 mV */
/* Rev 2 mapping */
#define AVS_R2_APE200_OFFSET 0xFF000000
#define AVS_R2_APE200_OFFSET_SHIFT 0x18
#define AVS_R2_APE266_OFFSET 0x00FF0000
#define AVS_R2_APE266_OFFSET_SHIFT 0x10
#define AVS_R2_APE333_OFFSET 0x0000FF00
#define AVS_R2_APE333_OFFSET_SHIFT 0x8
#define AVS_R2_APE_OFFSETCONV 2 /**< Fused value Y means offset is Y x 2 mV */

/* Stack parameters */
/* Independant from AVS revision */
#define AVS_ARM_GSS_ENABLE 0 /**< GSS not enabled yet */
#define AVS_ARM800_GSSOFFSET 25 /**< Benefit from GSS, negative number */
#define AVS_ARM1000_GSSOFFSET 25 /**< Benefit from GSS, negative number */
#define AVS_ARM400_UNDERSHOOT 38
#define AVS_ARM800_UNDERSHOOT 60
#define AVS_ARM1000_UNDERSHOOT 75
#define AVS_ARM_TEMPAGING 25
#define AVS_ARM_SMPSACCURACY 8 /**< +8/-8 mV */
#define AVS_ARM400_INTOFFSET 15
#define AVS_ARM800_INTOFFSET 24
#define AVS_ARM1000_INTOFFSET 30
#define AVS_ARM400_PCBOFFSET 0
#define AVS_ARM800_PCBOFFSET 0
#define AVS_ARM1000_PCBOFFSET 0

#define AVS_APE200_UNDERSHOOT 30
#define AVS_APE266_UNDERSHOOT 40
#define AVS_APE333_UNDERSHOOT 50
#define AVS_APE_TEMPAGING 25 
#define AVS_APE_SMPSACCURACY 8 /**< +8/-8 mV */
#define AVS_APE200_INTOFFSET 27
#define AVS_APE266_INTOFFSET 36
#define AVS_APE333_INTOFFSET 45
#define AVS_APE200_PCBOFFSET 5
#define AVS_APE266_PCBOFFSET 7
#define AVS_APE333_PCBOFFSET 8
#define AVS_APE200_ATEOFFSET 0 /**< ATE offset is handled here for Vape */
#define AVS_APE266_ATEOFFSET 0 /**< ATE offset is handled here for Vape */
#define AVS_APE333_ATEOFFSET 0 /**< ATE offset is handled here for Vape */

/**
 * Test offset (positive or negative) to compensate for characterisation errors
 * Note: MAKE SURE those values won't create a negative stack (probably unlikely)
 * If it does happen, minimum allowed voltage might not be met
 * Software fix will have to be implemented
 */
/* Rev 0: Prototypes only */
/* Rev 1 */
#define AVS_R1_ARM400_TESTOFFSET 0
#define AVS_R1_ARM800_TESTOFFSET 0
/* Rev 2 */
#define AVS_R2_ARM400_TESTOFFSET 0
#define AVS_R2_ARM800_TESTOFFSET 0
#define AVS_R2_ARM1000_TESTOFFSET 0
#define AVS_R2_APE200_TESTOFFSET 0 
#define AVS_R2_APE266_TESTOFFSET 0 
#define AVS_R2_APE333_TESTOFFSET 0 

/* Min + Max related voltages */
#define AVS_ARM_MINVOLTAGE 950 /**< Min voltage allowed, before adding stack */
#define AVS_ARM_MAXVOLTAGE 1400 /**< Max sustained voltage */
#define AVS_ARM_MAXTRVOLTAGE 1500 /**< Max transient voltage */ //FIXME TBC
#define AVS_APE_MINVOLTAGE 850 /**< Min voltage allowed, before adding stack */
#define AVS_APE_MAXVOLTAGE 1260 /**< Max sustained voltage */ 
#define AVS_APE_MAXTRVOLTAGE 1320 /**< Max transient voltage */

/* Reference voltages to apply offsets (revision dependent, but not yet) */
#define AVS_ARM400_VTX 950 /**< Ref voltage to apply offsets. ARM 400MHz */ 
#define AVS_ARM800_VTX 1150 /**< Ref voltage to apply offsets. ARM 800MHz */
#define AVS_ARM1000_VTX 1350 /**< Ref voltage to apply offsets. ARM 1000MHz */ 
#define AVS_APE200_VTX 950 /**< Ref voltage to apply offsets. APE 200MHz */
#define AVS_APE266_VTX 1050 /**< Ref voltage to apply offsets. APE 266MHz */
#define AVS_APE333_VTX 1150 /**< Ref voltage to apply offsets. APE 333MHz */ 

/* Petra related parameters */
/** !\ Warning! Not valid for all supplies, but valid for Varm and Vape */
/** /!\ Warning! Valid for Petra V2.x ONLY */
#define AVS_PETRA_STEP 10 /**< One Petra step equals to 10 mV */
#define AVS_PETRA_BASEV 650 /**< Petra base voltage. 0x0 = 650 mV */
#define AVS_PETRA_MINV 700 /**< Petra min usable voltage. 0x5 = 700 mV */
#define AVS_PETRA_MAXV 0x41 /**< Petra max voltage. 0x41 = 1300 mV */


#endif
