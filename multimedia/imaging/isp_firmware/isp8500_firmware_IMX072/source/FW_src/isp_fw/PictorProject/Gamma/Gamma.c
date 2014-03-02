/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * Gamma.c
 *
 *  Created on: Feb 2, 2010
 *      Author: mamtac
 *   \brief This file is a part of the gamma module release code. It implements a driver layer for the gamma.
 *
 \ingroup Gamma
 *
 */
#include "Gamma.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "PictorProject_Gamma_GammaTraces.h"
#endif


GammaControl_ts                 g_CE_GammaControl[GAMMA_PIPE_COUNT] =
{
    { Gamma_Ce0GammaCurve, Gamma_Ce0GammaSharp_PixelInShiftValue, Gamma_Ce0GammaUnSharp_PixelInShiftValue, Coin_e_Heads},
    { Gamma_Ce1GammaCurve, Gamma_Ce1GammaSharp_PixelInShiftValue, Gamma_Ce1GammaUnSharp_PixelInShiftValue, Coin_e_Heads}
};

GammaStatus_ts                 g_CE_GammaStatus[GAMMA_PIPE_COUNT] =
{
    { Gamma_Ce0GammaCurve, Gamma_Ce0GammaSharp_PixelInShiftValue, Gamma_Ce0GammaUnSharp_PixelInShiftValue, Coin_e_Tails},
    { Gamma_Ce1GammaCurve, Gamma_Ce1GammaSharp_PixelInShiftValue, Gamma_Ce1GammaUnSharp_PixelInShiftValue, Coin_e_Tails}
};

GammaMemLUTAddress_ts            g_CE_GammaMemLutAddress[GAMMA_PIPE_COUNT] =
{
    {0, 0, 0, 0, 0, 0},      // all addresses are initializes to zero for CE0
    {0, 0, 0, 0, 0, 0}       // all addresses are initializes to zero for CE1
};


GammaLastPixelValueControl_ts   g_CE_GammaLastPixelValueControl[GAMMA_PIPE_COUNT] =
{
    {    Gamma_Ce0GammaSharp_LstGreenGIRValue,
         Gamma_Ce0GammaSharp_LstRedValue,
         Gamma_Ce0GammaSharp_LstBlueValue,
         Gamma_Ce0GammaSharp_LstGreenGIBValue,
         Gamma_Ce0GammaUnSharp_LstGreenGIRValue,
         Gamma_Ce0GammaUnSharp_LstRedValue,
         Gamma_Ce0GammaUnSharp_LstBlueValue,
         Gamma_Ce0GammaUnSharp_LstGreenGIBValue
    },
    {    Gamma_Ce1GammaSharp_LstGreenGIRValue,
         Gamma_Ce1GammaSharp_LstRedValue,
         Gamma_Ce1GammaSharp_LstBlueValue,
         Gamma_Ce1GammaSharp_LstGreenGIBValue,
         Gamma_Ce1GammaUnSharp_LstGreenGIRValue,
         Gamma_Ce1GammaUnSharp_LstRedValue,
         Gamma_Ce1GammaUnSharp_LstBlueValue,
         Gamma_Ce1GammaUnSharp_LstGreenGIBValue
    }
};

/* int lutModTable[128];
double pow_gamma(double a, double b) {
  int tmp = (*(1 + (int *)&a));
  int tmp2 = (int)(b * (tmp - 1072632447) + 1072632447);
  double p = 0.0;
  *(1 + (int * )&p) = tmp2;
  return p;
}

int gammageneration(){
  double exp = 2.2;
  double inverseexp = (double)1/(double)exp;
  double lut_index =0;
  double scaled_value =0;
  double lut_value1 =0;
  double lut_value2 =0;
  double lut_scaledvalue1 =0;
  double lut_scaledvalue2 =0;
  int lut_entry1 =0;
  int lut_entry2 =0;
  int i =0;
  //FILE *fp = fopen("Gamma_Curve.txt","w");
  //FILE *fpLut1 = fopen("GammaLut1.txt","w");
  //FILE *fpLut2 = fopen("GammaLut2.txt","w");
  //fprintf(fp,"Index   LutIndex  ScaledValue GammaExp   InverseExp    LutValue   LutScaled    LutEntry\n");
  for(i=0;i<128;i++){
    scaled_value = (double)(lut_index)/(double) 1023;
    //lut_value1 =  pow(scaled_value , inverseexp);
    //lut_value2 =  myPow(scaled_value , inverseexp);
    lut_scaledvalue1 = (double)lut_value1 * (double)1023;
    //lut_scaledvalue2 = (double)lut_value2 * (double)1023;
    lut_entry1 = (int)(lut_scaledvalue1 + 0.5);
    //lut_entry2 = (int)(lut_scaledvalue2 + 0.5);
   // fprintf(fp,"%d\t\t\t\t%f\t\t%f\t\t%f\t\t%f\t\t%f\t\t%f\t\t%d\n",i,lut_index,scaled_value,exp,inverseexp,lut_value,lut_scaledvalue,lut_entry);
    //fprintf(fpLut1,"%d\n",lut_entry1);
    //fprintf(fpLut2,"%d\n",lut_entry2);
    lut_index+= (double)((double)1023/(double)127);
  }
}
*/

/**
  \fn void Gamma_Commit(uint8_t u8_PipeNo)
   \brief    Top level gamma module function to be called to
            program gamma for ce0 and ce1

  \return   void
  \param    uint8_t u8_PipeNo               :   PipeNo
  \callgraph
  \callergraph
  \ingroup Gamma
*/
void
Gamma_Commit(
uint8_t u8_PipeNo)
{
    uint8_t     u8_LutIndex;
    uint16_t    *ptru16_GammaLutTable;

    uint32_t    *ptru32_Sharp_GIR;
    uint32_t    *ptru32_Sharp_Red;
    uint32_t    *ptru32_Sharp_Blue;

    uint32_t    *ptru32_UnSharp_GIR;
    uint32_t    *ptru32_UnSharp_Red;
    uint32_t    *ptru32_UnSharp_Blue;

    uint32_t    *ptu32_Sharp_GIR_Shared_Add;    //local pointers to contain base address (reading from PE) of shared memory containg sharp green custom values
    uint32_t    *ptu32_Sharp_Red_Shared_Add;    //local pointers to contain base address (reading from PE) of shared memory containg sharp red custom values
    uint32_t    *ptu32_Sharp_Blue_Shared_Add;   //local pointers to contain base address (reading from PE) of shared memory containg sharp blue custom values

    uint32_t    *ptu32_UnSharp_GIR_Shared_Add;  //local pointers to contain base address (reading from PE) of shared memory containg Unsharp green custom values
    uint32_t    *ptu32_UnSharp_Red_Shared_Add;  //local pointers to contain base address (reading from PE) of shared memory containg Unsharp red custom values
    uint32_t    *ptu32_UnSharp_Blue_Shared_Add; //local pointers to contain base address (reading from PE) of shared memory containg  Unsharp red custom values

    OstTraceInt0(TRACE_DEBUG, ">>>GAMMA:: Entering in gamma function");

    ptru16_GammaLutTable = ( uint16_t * ) Gamma_LutTable;

    if (0 == u8_PipeNo)
    {
        //OstTraceInt2(TRACE_DEBUG, ">>>GAMMA:: The value of curve and coin are %d %d", g_CE_GammaControl[0].e_GammaCurve, g_CE_GammaControl[0].e_Coin_Ctrl);
        //OstTraceInt0(TRACE_DEBUG, ">>>GAMMA:: In pipe 0");
        if (Gamma_GetGammaCurve_Ce0() == GammaCurve_Standard)
        {
            //OstTraceInt0(TRACE_DEBUG, ">>>GAMMA:: In pipe Standard");
            Gamma_Set_GammaSharp_Disable_Ce0();
            Gamma_Set_GammaUnSharp_Disable_Ce0();

            ptru32_Sharp_GIR = ( uint32_t * ) Gamma_GetCe0Sharp_GIRAddr();
            ptru32_Sharp_Red = ( uint32_t * ) Gamma_GetCe0Sharp_RedAddr();
            ptru32_Sharp_Blue = ( uint32_t * ) Gamma_GetCe0Sharp_BlueAddr();

            ptru32_UnSharp_GIR = ( uint32_t * ) Gamma_GetCe0UnSharp_GIRAddr();
            ptru32_UnSharp_Red = ( uint32_t * ) Gamma_GetCe0UnSharp_RedAddr();
            ptru32_UnSharp_Blue = ( uint32_t * ) Gamma_GetCe0UnSharp_BlueAddr();

            // Program LUT Memories
            for (u8_LutIndex = 0; u8_LutIndex < NO_OF_LUTBINS; u8_LutIndex++)
            {
                *(ptru32_Sharp_GIR + u8_LutIndex) = ptru16_GammaLutTable[u8_LutIndex];
                *(ptru32_Sharp_Red + u8_LutIndex) = ptru16_GammaLutTable[u8_LutIndex];
                *(ptru32_Sharp_Blue + u8_LutIndex) = ptru16_GammaLutTable[u8_LutIndex];

                *(ptru32_UnSharp_GIR + u8_LutIndex) = ptru16_GammaLutTable[u8_LutIndex];
                *(ptru32_UnSharp_Red + u8_LutIndex) = ptru16_GammaLutTable[u8_LutIndex];
                *(ptru32_UnSharp_Blue + u8_LutIndex) = ptru16_GammaLutTable[u8_LutIndex];
            }

            //Program GammaSharp Ce0 Hardware Registers
            Gamma_Set_GammaSharp_LstGreenGIRValue_Ce0(g_CE_GammaLastPixelValueControl[0].u16_Sharp_Lst_GreenGIR);
            Gamma_Set_GammaSharp_LstRedValue_Ce0(g_CE_GammaLastPixelValueControl[0].u16_Sharp_Lst_Red);
            Gamma_Set_GammaSharp_LstBlueValue_Ce0(g_CE_GammaLastPixelValueControl[0].u16_Sharp_Lst_Blue);
            Gamma_Set_GammaSharp_LstGreenGIBValue_Ce0(g_CE_GammaLastPixelValueControl[0].u16_Sharp_Lst_GreenGIB);
            Gamma_Set_GammaSharp_PixelInShift_Ce0(g_CE_GammaControl[0].u8_GammaPixelInShift_Sharp);

            //Program GammaUnSharp Ce0 Hardware Registers
            Gamma_Set_GammaUnSharp_LstGreenGIRValue_Ce0(g_CE_GammaLastPixelValueControl[0].u16_UnSharp_Lst_GreenGIR);
            Gamma_Set_GammaUnSharp_LstRedValue_Ce0(g_CE_GammaLastPixelValueControl[0].u16_UnSharp_Lst_Red);
            Gamma_Set_GammaUnSharp_LstBlueValue_Ce0(g_CE_GammaLastPixelValueControl[0].u16_UnSharp_Lst_Blue);
            Gamma_Set_GammaUnSharp_LstGreenGIBValue_Ce0(g_CE_GammaLastPixelValueControl[0].u16_UnSharp_Lst_GreenGIB);
            Gamma_Set_GammaUnSharp_PixelInShift_Ce0(g_CE_GammaControl[0].u8_GammaPixelInShift_UnSharp);

            Gamma_Set_GammaUnSharp_Enable_Ce0();
            Gamma_Set_GammaSharp_Enable_Ce0();

        }
        else if (Gamma_GetGammaCurve_Ce0() == GammaCurve_Custom)
        {

            //New code where in custom mode also LUT will be programmed by Firmware
            //Disable GammaSharp Ce0 and GammaUnSharpCe0 nothing
            Gamma_Set_GammaSharp_Disable_Ce0();
            Gamma_Set_GammaUnSharp_Disable_Ce0();

            //Assigning base addresses of LUT memories in the varaibles.
            ptru32_Sharp_GIR = ( uint32_t * ) Gamma_GetCe0Sharp_GIRAddr();
            ptru32_Sharp_Red = ( uint32_t * ) Gamma_GetCe0Sharp_RedAddr();
            ptru32_Sharp_Blue = ( uint32_t * ) Gamma_GetCe0Sharp_BlueAddr();

            ptru32_UnSharp_GIR = ( uint32_t * ) Gamma_GetCe0UnSharp_GIRAddr();
            ptru32_UnSharp_Red = ( uint32_t * ) Gamma_GetCe0UnSharp_RedAddr();
            ptru32_UnSharp_Blue = ( uint32_t * ) Gamma_GetCe0UnSharp_BlueAddr();

            //Assigning shared memories addresses given by host via page elements in local variables
            ptu32_Sharp_GIR_Shared_Add = (uint32_t *) g_CE_GammaMemLutAddress[0].u32_SharpGreenLutAddress;
            ptu32_Sharp_Red_Shared_Add = (uint32_t *) g_CE_GammaMemLutAddress[0].u32_SharpRedLutAddress;
            ptu32_Sharp_Blue_Shared_Add = (uint32_t *) g_CE_GammaMemLutAddress[0].u32_SharpBlueLutAddress;

            ptu32_UnSharp_GIR_Shared_Add = (uint32_t *) g_CE_GammaMemLutAddress[0].u32_UnSharpGreenLutAddress;
            ptu32_UnSharp_Red_Shared_Add = (uint32_t *) g_CE_GammaMemLutAddress[0].u32_UnSharpRedLutAddress;
            ptu32_UnSharp_Blue_Shared_Add = (uint32_t *) g_CE_GammaMemLutAddress[0].u32_UnSharpBlueLutAddress;

            //Assigning custom values to the LUT memories
            //OstTraceInt0(TRACE_DEBUG, "***************TIME BEFORE LOOP FOR CE0************** ");
            for (u8_LutIndex = 0; u8_LutIndex < NO_OF_LUTBINS; u8_LutIndex++)
            {
                *(ptru32_Sharp_GIR + u8_LutIndex) = ptu32_Sharp_GIR_Shared_Add[u8_LutIndex];
                *(ptru32_Sharp_Red + u8_LutIndex) = ptu32_Sharp_Red_Shared_Add[u8_LutIndex];
                *(ptru32_Sharp_Blue + u8_LutIndex) = ptu32_Sharp_Blue_Shared_Add[u8_LutIndex];

                *(ptru32_UnSharp_GIR + u8_LutIndex) = ptu32_UnSharp_GIR_Shared_Add[u8_LutIndex];
                *(ptru32_UnSharp_Red + u8_LutIndex) = ptu32_UnSharp_Red_Shared_Add[u8_LutIndex];
                *(ptru32_UnSharp_Blue + u8_LutIndex) = ptu32_UnSharp_Blue_Shared_Add[u8_LutIndex];
            }
            //OstTraceInt0(TRACE_DEBUG, "***************TIME AFTER LOOP FOR CE0************** ");

            //Host will program LUT Memories by itself,only firmware will program hardware registers
            //Program GammaSharp Ce0 Hardware Registers
            Gamma_Set_GammaSharp_LstGreenGIRValue_Ce0(g_CE_GammaLastPixelValueControl[0].u16_Sharp_Lst_GreenGIR);
            Gamma_Set_GammaSharp_LstRedValue_Ce0(g_CE_GammaLastPixelValueControl[0].u16_Sharp_Lst_Red);
            Gamma_Set_GammaSharp_LstBlueValue_Ce0(g_CE_GammaLastPixelValueControl[0].u16_Sharp_Lst_Blue);
            Gamma_Set_GammaSharp_LstGreenGIBValue_Ce0(g_CE_GammaLastPixelValueControl[0].u16_Sharp_Lst_GreenGIB);
            Gamma_Set_GammaSharp_PixelInShift_Ce0(g_CE_GammaControl[0].u8_GammaPixelInShift_Sharp);

            //Program GammaUnSharp Ce0 Hardware Registers
            Gamma_Set_GammaUnSharp_LstGreenGIRValue_Ce0(g_CE_GammaLastPixelValueControl[0].u16_UnSharp_Lst_GreenGIR);
            Gamma_Set_GammaUnSharp_LstRedValue_Ce0(g_CE_GammaLastPixelValueControl[0].u16_UnSharp_Lst_Red);
            Gamma_Set_GammaUnSharp_LstBlueValue_Ce0(g_CE_GammaLastPixelValueControl[0].u16_UnSharp_Lst_Blue);
            Gamma_Set_GammaUnSharp_LstGreenGIBValue_Ce0(g_CE_GammaLastPixelValueControl[0].u16_UnSharp_Lst_GreenGIB);
            Gamma_Set_GammaUnSharp_PixelInShift_Ce0(g_CE_GammaControl[0].u8_GammaPixelInShift_UnSharp);

            Gamma_Set_GammaUnSharp_Enable_Ce0();
            Gamma_Set_GammaSharp_Enable_Ce0();
        }
        else if (Gamma_GetGammaCurve_Ce0() == GammaCurve_Disable)
        {
            //Disable GammaSharp Ce0 and GammaUnSharpCe0 nothing
            Gamma_Set_GammaSharp_Disable_Ce0();
            Gamma_Set_GammaUnSharp_Disable_Ce0();
        }


    }
    else if (GAMMA_PIPE_COUNT > 1)
    {
        //OstTraceInt0(TRACE_DEBUG, ">>>GAMMA:: In pipe 1");
        //OstTraceInt2(TRACE_DEBUG, ">>>GAMMA:: The value of control curve and coin are %d %d", g_CE_GammaControl[1].e_GammaCurve, g_CE_GammaControl[1].e_Coin_Ctrl);
        if (Gamma_GetGammaCurve_Ce1() == GammaCurve_Standard)
        {
            //OstTraceInt0(TRACE_DEBUG, ">>>GAMMA:: In pipe Standard");
            Gamma_Set_GammaSharp_Disable_Ce1();
            Gamma_Set_GammaUnSharp_Disable_Ce1();

            /* There is bug in Pictor V1 which is fixed in V2
            [ PictorBug #91370 ] STXP70 is not able to access CE1 FlexTF Sharp and Unsharp Memories.
            */
            if (Is_8500v2() || Is_9540v1() || Is_8540v1())
            {
                ptru32_Sharp_GIR = ( uint32_t * ) Gamma_GetCe1Sharp_GIRAddr();
                ptru32_Sharp_Red = ( uint32_t * ) Gamma_GetCe1Sharp_RedAddr();
                ptru32_Sharp_Blue = ( uint32_t * ) Gamma_GetCe1Sharp_BlueAddr();

                ptru32_UnSharp_GIR = ( uint32_t * ) Gamma_GetCe1UnSharp_GIRAddr();
                ptru32_UnSharp_Red = ( uint32_t * ) Gamma_GetCe1UnSharp_RedAddr();
                ptru32_UnSharp_Blue = ( uint32_t * ) Gamma_GetCe1UnSharp_BlueAddr();
#if 0
                //Swapped Channels to apply Gamma effectively on the correct hardware
                ptru32_Sharp_GIR = ( uint32_t * ) Gamma_GetCe1Sharp_BlueAddr();
                ptru32_Sharp_Red = ( uint32_t * ) Gamma_GetCe1Sharp_GIRAddr();
                ptru32_Sharp_Blue = ( uint32_t * ) Gamma_GetCe1Sharp_RedAddr();

                ptru32_UnSharp_GIR = ( uint32_t * ) Gamma_GetCe1UnSharp_BlueAddr();
                ptru32_UnSharp_Red = ( uint32_t * ) Gamma_GetCe1UnSharp_GIRAddr();
                ptru32_UnSharp_Blue = ( uint32_t * ) Gamma_GetCe1UnSharp_RedAddr();
#endif
                 //OstTraceInt0(TRACE_DEBUG, ">>>GAMMA :: BEFORE FOR LOOP");
                // Program LUT Memories
                for (u8_LutIndex = 0; u8_LutIndex < NO_OF_LUTBINS; u8_LutIndex++)
                {
                    *(ptru32_Sharp_GIR + u8_LutIndex) = ptru16_GammaLutTable[u8_LutIndex];
                    *(ptru32_Sharp_Red + u8_LutIndex) = ptru16_GammaLutTable[u8_LutIndex];
                    *(ptru32_Sharp_Blue + u8_LutIndex) = ptru16_GammaLutTable[u8_LutIndex];

                    *(ptru32_UnSharp_GIR + u8_LutIndex) = ptru16_GammaLutTable[u8_LutIndex];
                    *(ptru32_UnSharp_Red + u8_LutIndex) = ptru16_GammaLutTable[u8_LutIndex];
                    *(ptru32_UnSharp_Blue + u8_LutIndex) = ptru16_GammaLutTable[u8_LutIndex];
                }
                //OstTraceInt0(TRACE_DEBUG, ">>>GAMMA :: BEFORE FOR LOOP");
            }


            //Program GammaSharp Ce1 Hardware Registers
            Gamma_Set_GammaSharp_LstGreenGIRValue_Ce1(g_CE_GammaLastPixelValueControl[1].u16_Sharp_Lst_GreenGIR);
            Gamma_Set_GammaSharp_LstRedValue_Ce1(g_CE_GammaLastPixelValueControl[1].u16_Sharp_Lst_Red);
            Gamma_Set_GammaSharp_LstBlueValue_Ce1(g_CE_GammaLastPixelValueControl[1].u16_Sharp_Lst_Blue);
            Gamma_Set_GammaSharp_LstGreenGIBValue_Ce1(g_CE_GammaLastPixelValueControl[1].u16_Sharp_Lst_GreenGIB);
            Gamma_Set_GammaSharp_PixelInShift_Ce1(g_CE_GammaControl[1].u8_GammaPixelInShift_Sharp);

            //Program GammaUnSharp Ce1 Hardware Registers
            Gamma_Set_GammaUnSharp_LstGreenGIRValue_Ce1(g_CE_GammaLastPixelValueControl[1].u16_UnSharp_Lst_GreenGIR);
            Gamma_Set_GammaUnSharp_LstRedValue_Ce1(g_CE_GammaLastPixelValueControl[1].u16_UnSharp_Lst_Red);
            Gamma_Set_GammaUnSharp_LstBlueValue_Ce1(g_CE_GammaLastPixelValueControl[1].u16_UnSharp_Lst_Blue);
            Gamma_Set_GammaUnSharp_LstGreenGIBValue_Ce1(g_CE_GammaLastPixelValueControl[1].u16_UnSharp_Lst_GreenGIB);
            Gamma_Set_GammaUnSharp_PixelInShift_Ce1(g_CE_GammaControl[1].u8_GammaPixelInShift_UnSharp);
            Gamma_Set_GammaUnSharp_Enable_Ce1();
            Gamma_Set_GammaSharp_Enable_Ce1();

        }
        else if (Gamma_GetGammaCurve_Ce1() == GammaCurve_Custom)
        {

            //New code where in custom mode also LUT will be programmed by Firmware
            //Disable GammaSharp Ce0 and GammaUnSharpCe0 nothing
            Gamma_Set_GammaSharp_Disable_Ce1();
            Gamma_Set_GammaUnSharp_Disable_Ce1();

            //Assigning base addresses of LUT memories in the varaibles.
            ptru32_Sharp_GIR = ( uint32_t * ) Gamma_GetCe1Sharp_GIRAddr();
            ptru32_Sharp_Red = ( uint32_t * ) Gamma_GetCe1Sharp_RedAddr();
            ptru32_Sharp_Blue = ( uint32_t * ) Gamma_GetCe1Sharp_BlueAddr();

            ptru32_UnSharp_GIR = ( uint32_t * ) Gamma_GetCe1UnSharp_GIRAddr();
            ptru32_UnSharp_Red = ( uint32_t * ) Gamma_GetCe1UnSharp_RedAddr();
            ptru32_UnSharp_Blue = ( uint32_t * ) Gamma_GetCe1UnSharp_BlueAddr();

            //Assigning shared memories addresses given by host via page elements in local variables
            ptu32_Sharp_GIR_Shared_Add = (uint32_t *) g_CE_GammaMemLutAddress[1].u32_SharpGreenLutAddress;
            ptu32_Sharp_Red_Shared_Add = (uint32_t *) g_CE_GammaMemLutAddress[1].u32_SharpRedLutAddress;
            ptu32_Sharp_Blue_Shared_Add = (uint32_t *) g_CE_GammaMemLutAddress[1].u32_SharpBlueLutAddress;

            ptu32_UnSharp_GIR_Shared_Add = (uint32_t *) g_CE_GammaMemLutAddress[1].u32_UnSharpGreenLutAddress;
            ptu32_UnSharp_Red_Shared_Add = (uint32_t *) g_CE_GammaMemLutAddress[1].u32_UnSharpRedLutAddress;
            ptu32_UnSharp_Blue_Shared_Add = (uint32_t *) g_CE_GammaMemLutAddress[1].u32_UnSharpBlueLutAddress;


            //OstTraceInt0(TRACE_DEBUG, "***************TIME BEFORE LOOP FOR CE1************** ");
            //Assigning custom values to the LUT memories
            for (u8_LutIndex = 0; u8_LutIndex < NO_OF_LUTBINS; u8_LutIndex++)
            {
                *(ptru32_Sharp_GIR + u8_LutIndex) = ptu32_Sharp_GIR_Shared_Add[u8_LutIndex];
                *(ptru32_Sharp_Red + u8_LutIndex) = ptu32_Sharp_Red_Shared_Add[u8_LutIndex];
                *(ptru32_Sharp_Blue + u8_LutIndex) = ptu32_Sharp_Blue_Shared_Add[u8_LutIndex];

                *(ptru32_UnSharp_GIR + u8_LutIndex) = ptu32_UnSharp_GIR_Shared_Add[u8_LutIndex];
                *(ptru32_UnSharp_Red + u8_LutIndex) = ptu32_UnSharp_Red_Shared_Add[u8_LutIndex];
                *(ptru32_UnSharp_Blue + u8_LutIndex) = ptu32_UnSharp_Blue_Shared_Add[u8_LutIndex];
            }
            //OstTraceInt0(TRACE_DEBUG, "***************TIME AFTER LOOP FOR CE1************** ");

            //Host will program LUT Memories by itself,only firmware will program hardware registers
            //Program GammaSharp Ce0 Hardware Registers
            Gamma_Set_GammaSharp_LstGreenGIRValue_Ce1(g_CE_GammaLastPixelValueControl[1].u16_Sharp_Lst_GreenGIR);
            Gamma_Set_GammaSharp_LstRedValue_Ce1(g_CE_GammaLastPixelValueControl[1].u16_Sharp_Lst_Red);
            Gamma_Set_GammaSharp_LstBlueValue_Ce1(g_CE_GammaLastPixelValueControl[1].u16_Sharp_Lst_Blue);
            Gamma_Set_GammaSharp_LstGreenGIBValue_Ce1(g_CE_GammaLastPixelValueControl[1].u16_Sharp_Lst_GreenGIB);
            Gamma_Set_GammaSharp_PixelInShift_Ce1(g_CE_GammaControl[1].u8_GammaPixelInShift_Sharp);

            //Program GammaUnSharp Ce0 Hardware Registers
            Gamma_Set_GammaUnSharp_LstGreenGIRValue_Ce1(g_CE_GammaLastPixelValueControl[1].u16_UnSharp_Lst_GreenGIR);
            Gamma_Set_GammaUnSharp_LstRedValue_Ce1(g_CE_GammaLastPixelValueControl[1].u16_UnSharp_Lst_Red);
            Gamma_Set_GammaUnSharp_LstBlueValue_Ce1(g_CE_GammaLastPixelValueControl[1].u16_UnSharp_Lst_Blue);
            Gamma_Set_GammaUnSharp_LstGreenGIBValue_Ce1(g_CE_GammaLastPixelValueControl[1].u16_UnSharp_Lst_GreenGIB);
            Gamma_Set_GammaUnSharp_PixelInShift_Ce1(g_CE_GammaControl[1].u8_GammaPixelInShift_UnSharp);

            Gamma_Set_GammaUnSharp_Enable_Ce1();
            Gamma_Set_GammaSharp_Enable_Ce1();
        }
        else if (Gamma_GetGammaCurve_Ce1() == GammaCurve_Disable)
        {
            //Disable GammaSharp Ce1 and GammaUnSharpCe1 nothing
            Gamma_Set_GammaSharp_Disable_Ce1();
            Gamma_Set_GammaUnSharp_Disable_Ce1();
        }
        else
        {
            //Do nothing
        }

    }


    return;
}


void Update_Gamma_Pipe(uint8_t pipe_no)
{
    //OstTraceInt1(TRACE_DEBUG, ">>>GAMMA:: Entering in gamma update function for pipe %d", pipe_no);

    if(Gamma_isUpdateGammaRequestPending(pipe_no))  //check if any gamma req is pending for requested pipe
    {
        Gamma_Commit(pipe_no);

        //Update CE_GammaStatus variables
        OstTraceInt1(TRACE_DEBUG, ">>>GAMMA :: Making control status same for for pipe %d", pipe_no);
        g_CE_GammaStatus[pipe_no].e_GammaCurve = g_CE_GammaControl[pipe_no].e_GammaCurve;
        g_CE_GammaStatus[pipe_no].u8_GammaPixelInShift_Sharp = g_CE_GammaControl[pipe_no].u8_GammaPixelInShift_Sharp;
        g_CE_GammaStatus[pipe_no].u8_GammaPixelInShift_UnSharp = g_CE_GammaControl[pipe_no].u8_GammaPixelInShift_UnSharp;
        g_CE_GammaStatus[pipe_no].e_Coin_Status= g_CE_GammaControl[pipe_no].e_Coin_Ctrl;

        //OstTraceInt2(TRACE_DEBUG, ">>>GAMMA:: The value of status curve and coin are %d %d", g_CE_GammaStatus[1].e_GammaCurve, g_CE_GammaStatus[1].e_Coin_Status);

        //Next to send the gamma update complete notification to the HOST
        if(pipe_no == 0)
            EventManager_GammaUpdateCompletePipe0_Notify();
        else
            EventManager_GammaUpdateCompletePipe1_Notify();
    }

}
