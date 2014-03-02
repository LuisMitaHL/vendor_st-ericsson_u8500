/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __ITE_ISPBLOCKS_TESTS_H__
#define __ITE_ISPBLOCKS_TESTS_H__

/* For adding a new ISP Block, additions have to be done at the following places:
 * 1. DECL_ISPBLOCK_OPERATIONS macro in ite_ispblocks_tests.c
 * 2. ADD_ISPBLOCK_ENTRY macro in ite_ispblocks_tests.c
 * 3. Define the relevant functions corresponding to those in DECL_ISPBLOCK_OPERATIONS.
 *
 * PS: The Isp Block Name (for eg., RSO) used at places 1 to 3 above, should be exactly the same.
 * Further generic implementaion (i.e., declaring the isp block name at one place only) has not been done,
 * to preserve code clarity.
 */


/* Config Parameters */
//RSO
#define  RSO_NEW_ENABLE                                            Flag_e_TRUE
#define  RSO_NEW_MODE                                              RSO_Mode_e_Manual
#define  RSO_NEW_GR_X_COEFF                                        2
#define  RSO_NEW_GR_Y_COEFF                                        2
#define  RSO_NEW_R_X_COEFF                                         2
#define  RSO_NEW_R_Y_COEFF                                         2
#define  RSO_NEW_B_X_COEFF                                         2
#define  RSO_NEW_B_Y_COEFF                                         2
#define  RSO_NEW_GB_X_COEFF                                        2
#define  RSO_NEW_GB_Y_COEFF                                        2
#define  RSO_NEW_GR_DC_TERM                                        2
#define  RSO_NEW_R_DC_TERM                                         2
#define  RSO_NEW_B_DC_TERM                                         2
#define  RSO_NEW_GB_DC_TERM                                        2
#define  RSO_NEW_X_SLANT_ORIGIN                                    2
#define  RSO_NEW_Y_SLANT_ORIGIN                                    2

//SDL
#define  SDL_NEW_ENABLE                                            SDL_Custom
#define  SDL_ELT_LASTELEMENTINLUT_GIR_BYTE0                        0x0A
#define  SDL_NEW_LASTELEMENTINLUT_RED_BYTE0                        0x0B
#define  SDL_NEW_LASTELEMENTINLUT_BLUE_BYTE0                       0x0C
#define  SDL_NEW_LASTELEMENTINLUT_GIB_BYTE0                        0x0D
#define  SDL_NEW_PIXELSHIFT_BYTE0                                  0x01

    
//Gridiron
#define  GRIDIRON_NEW_ENABLE_DISABLE                               Flag_e_TRUE
#define  GRIDIRON_NEW_LIVECAST                                     4
#define  GRIDIRON_NEW_CAST_POSITION0                               3
#define  GRIDIRON_NEW_CAST_POSITION1                               4
#define  GRIDIRON_NEW_CAST_POSITION2                               5
#define  GRIDIRON_NEW_CAST_POSITION3                               7
#define  GRIDIRON_NEW_FPHASE                                       1
#define  GRIDIRON_NEW_U8_PHASE                                     1
#define  GRIDIRON_NEW_MODE                                         GridironMode_Manual
#define  GRIDIRON_NEW_EN_CAST0                                     Flag_e_TRUE
#define  GRIDIRON_NEW_EN_CAST1                                     Flag_e_TRUE
#define  GRIDIRON_NEW_EN_CAST2                                     Flag_e_FALSE
#define  GRIDIRON_NEW_EN_CAST3                                     Flag_e_TRUE
#define  GRIDIRON_NEW_HOR_FLIP                                     Flag_e_TRUE
#define  GRIDIRON_NEW_VER_FLIP                                     Flag_e_TRUE

//Channel Gains
#define  CG_NEW_ENABLE                                             Flag_e_TRUE
#define  CG_NEW_COMBINED_RED_GAIN                                  2.0
#define  CG_NEW_COMBINED_GREENINRED_GAIN                           2.0
#define  CG_NEW_COMBINED_GREENINBLUE_GAIN                          2.0
#define  CG_NEW_COMBINED_BLUE_GAIN                                 2.0

//SCORPIO
#define  SCORPIO_NEW_ENABLE                                        Flag_e_TRUE
#define  SCORPIO_NEW_MODE                                          ScorpioMode_e_Manual
#define  SCORPIO_NEW_CORING_LEVEL                                  1

//Dustor
//TBD

//Binning Repair
#define  BINNING_NEW_REPAIR_ENABLE                                 Flag_e_TRUE
#define  BINNING_NEW_REPAIR_H_JOG_ENABLE                           Flag_e_TRUE
#define  BINNING_NEW_REPAIR_V_JOG_ENABLE                           Flag_e_TRUE
#define  BINNING_NEW_REPAIR_MODE                                   BinningRepairMode_e_Custom
#define  BINNING_NEW_REPAIR_COEFF_00                               2
#define  BINNING_NEW_REPAIR_COEFF_01                               18
#define  BINNING_NEW_REPAIR_COEFF_10                               14
#define  BINNING_NEW_REPAIR_COEFF_11                               98
#define  BINNING_NEW_REPAIR_COEFF_SHIFT                            6
#define  BINNING_NEW_REPAIR_FACTOR                                 2
         
//Babylon
#define  BABYLON_NEW_ENABLE                                        (Flag_e_TRUE)
#define  BABYLON_NEW_ZIPPERKILL                                    (0xC)
#define  BABYLON_NEW_FLAT_THRESHOLD                                (0x20)
#define  BABYLON_NEW_MODE                                          (BabylonMode_e_Manual)

//Colour Matrix
//CE0
#define  CE_COLOURMATRIX_NEW_FLOAT_0_REDINRED                      2.5
#define  CE_COLOURMATRIX_NEW_FLOAT_0_GREENINRED                    2.3 //0x40133333 
#define  CE_COLOURMATRIX_NEW_FLOAT_0_BLUEINRED                     0.3//0x3E999999 
#define  CE_COLOURMATRIX_NEW_FLOAT_0_REDINGREEN                    5.0
#define  CE_COLOURMATRIX_NEW_FLOAT_0_GREENINGREEN                  6.0
#define  CE_COLOURMATRIX_NEW_FLOAT_0_BLUEINGREEN                   4.0
#define  CE_COLOURMATRIX_NEW_FLOAT_0_REDINBLUE                    -3.9
#define  CE_COLOURMATRIX_NEW_FLOAT_0_GREENINBLUE                   0.0
#define  CE_COLOURMATRIX_NEW_FLOAT_0_BLUEINBLUE                    0.2
#define  CE_COLOURMATRIX_NEW_FLOAT_0_SWAPCOLOUR_RED                SwapColour_e_RedSwap  
#define  CE_COLOURMATRIX_NEW_FLOAT_0_SWAPCOLOUR_GREEN              SwapColour_e_GreenSwap
#define  CE_COLOURMATRIX_NEW_FLOAT_0_SWAPCOLOUR_BLUE               SwapColour_e_BlueSwap                                                                  
#define  CE_COLOURMATRIX_NEW_CTRL_0_OFFSET_R                       2
#define  CE_COLOURMATRIX_NEW_CTRL_0_OFFSET_G                       2
#define  CE_COLOURMATRIX_NEW_CTRL_0_OFFSET_B                       2
#define  CE_COLOURMATRIX_NEW_0_MODE                                ColourMatrixMode_e_Auto

                                                                   
//CE1                                                              
#define  CE_COLOURMATRIX_NEW_FLOAT_1_REDINRED                      2.0                      
#define  CE_COLOURMATRIX_NEW_FLOAT_1_GREENINRED                    1.0                      
#define  CE_COLOURMATRIX_NEW_FLOAT_1_BLUEINRED                     1.0                      
#define  CE_COLOURMATRIX_NEW_FLOAT_1_REDINGREEN                    1.0                      
#define  CE_COLOURMATRIX_NEW_FLOAT_1_GREENINGREEN                  2.0                      
#define  CE_COLOURMATRIX_NEW_FLOAT_1_BLUEINGREEN                   1.0                      
#define  CE_COLOURMATRIX_NEW_FLOAT_1_REDINBLUE                     1.0                      
#define  CE_COLOURMATRIX_NEW_FLOAT_1_GREENINBLUE                   1.0                      
#define  CE_COLOURMATRIX_NEW_FLOAT_1_BLUEINBLUE                    2.0                      
#define  CE_COLOURMATRIX_NEW_FLOAT_1_SWAPCOLOUR_RED                SwapColour_e_RedSwap
#define  CE_COLOURMATRIX_NEW_FLOAT_1_SWAPCOLOUR_GREEN              SwapColour_e_GreenSwap
#define  CE_COLOURMATRIX_NEW_FLOAT_1_SWAPCOLOUR_BLUE               SwapColour_e_BlueSwap
#define  CE_COLOURMATRIX_NEW_CTRL_1_OFFSET_R                       1
#define  CE_COLOURMATRIX_NEW_CTRL_1_OFFSET_G                       1
#define  CE_COLOURMATRIX_NEW_CTRL_1_OFFSET_B                       1
#define  CE_COLOURMATRIX_NEW_1_MODE                                ColourMatrixMode_e_Auto

/* Exports */
void Init_ISP_Blocks_Configure_ITECmdList(void);
CMD_COMPLETION C_ite_dbg_TestIspBlock_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_ConfigureIspBlocks_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_VerifyParamsApplied_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_VerifyGlaceEvent_cmd(int a_nb_args, char ** ap_args);

#endif /*__ITE_ISPBLOCKS_TESTS_H__*/
