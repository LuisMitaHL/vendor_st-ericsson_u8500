/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include "DeviceParameter.h"


#if ((USE_OST_TRACES == PROTOCOL_ID_ASCII) || (USE_OST_TRACES == PROTOCOL_ID_BINARY))

uint8_t st_timestamp[8] = { 0xd0, 0x00, 0x03, 0x2f, 0x3e, 0x89, 0x6e, 0x3b };
uint8_t   g_Channel = 5;
/*---------------------------Code for Mapping IO space for traces --------------------------------STARTS*/

/*--Common to both ASCII and Binary--*/
#   define SIA_STM_V1  0xE1104000u          // For IO space 1 Mapping
#   define SIA_STM_V2  0xE2104000u          // For IO space 1 Mapping
typedef volatile struct
{
    union
    {
        unsigned char       no_stamp8;
        unsigned short      no_stamp16;
        unsigned int        no_stamp32;
        unsigned long long  no_stamp64;
    } x_u;
    union
    {
        unsigned char       stamp8;
        unsigned short      stamp16;
        unsigned int        stamp32;
        unsigned long long  stamp64;
    } y_u;
} stm_channel_ts;

stm_channel_ts   *stm_channels = ( stm_channel_ts * ) NULL;

#   define DEFLLTFUN(type, size)                                                          \
    void                                                                                  \
    stm_trace_##size(                                                                     \
    unsigned char   channel,                                                              \
    unsigned type   data)                                                                 \
    {                                                                                     \
        *(( volatile unsigned type * ) &(stm_channels[channel].x_u.no_stamp##size)) = data; \
    }                                                                                     \
                                                                                       \
                                                                                       \
    void                                                                                  \
    stm_tracet_##size(                                                                    \
    unsigned char   channel,                                                              \
    unsigned type   data)                                                                 \
    {                                                                                     \
        *(( volatile unsigned type * ) &(stm_channels[channel].y_u.stamp##size)) = data;    \
    }


//Call to macros for creating functions to write Trace data to required memory location.
DEFLLTFUN ( char, 8 )
DEFLLTFUN ( short, 16 )
DEFLLTFUN ( int, 32 )
//DEFLLTFUN ( long long, 64 ) //  We aint writing 8bytes at a time in our code. So commenting it.

/**
* Function Name   : Map_STM_Registers
* Description        : Map IO region 1 for XP70 to send data to STM
* Input                 : None
* Return              : None
*/
void
Map_STM_Registers(void)
{
    volatile unsigned short *p_ISPReg;

    // Program the address to be mapped IO 1
    p_ISPReg = ( volatile unsigned short * ) (0xF0002000 + 0x62);   // 0xA0254000 is the address of STxP70's ISP reg's from CA9 side
    *p_ISPReg = 0x8000;

    if (Is_8500v2() || Is_9540v1() || Is_8540v1())
    {
        stm_channels = ( stm_channel_ts * ) SIA_STM_V2;
    }
    else if (Is_8500v1())
    {
        stm_channels = ( stm_channel_ts * ) SIA_STM_V1;
    }
}
#endif //#if ((USE_OST_TRACES == PROTOCOL_ID_ASCII) || (USE_OST_TRACES == PROTOCOL_ID_BINARY))


/**
* Function Name   : Write_Payload
* Description        : Updates the payload buffer with the passed string and arguments
* Input                :
    int8_t u8_Payload[256]: Buffer to be updated
    int8_t * pBuffer: String passed for display
    float_t f_Param1 : First Parameter passed to be displayed
    float_t f_Param2 : Second Parameter passed to be displayed
    float_t f_Param3 : Third Parameter passed to be displayed
    float_t f_Param4 : Fourth Parameter passed to be displayed
    uint16_t u16_MaxSizeToHandle: Max Trace buffer to be stuffed inside the passed buffer
* Return              : size of the buffer to be written
*/

uint16_t
Write_Payload(
uint8_t            *pu8_Payload,
uint8_t            *pBuffer,
float_t           f_Param1,
float_t           f_Param2,
float_t           f_Param3,
float_t           f_Param4,
uint16_t        u16_MaxSizeToHandle)
{
    float_t           f_ParamPassed[4] = { f_Param1,
                    f_Param2,
                    f_Param3,
                    f_Param4 };
    uint32_t u32_ValueInt;
    uint8_t   u8_DigitCount,
                 u8_SaveDigitCount;
    uint8_t   u8_DecimalPlace,
                 u8_SaveDecimalPlace;
    uint8_t   u8_paramNo;
    uint16_t u16_size;

    u8_paramNo = 0;
    u16_size = 0;

    while (*pBuffer != '\0')
    {
        if ((u16_MaxSizeToHandle - 1) <= u16_size)
        {
            pu8_Payload[u16_size] = '\0';               // write a null at the last memory location of the passed buffer. Buffer overflow case
            break;
        }


        if (*pBuffer != '%')
        {
            pu8_Payload[u16_size] = *pBuffer;
        }
        else
        {
            switch (*(pBuffer + 1))
            {
                case 'd':
                case 'i':
                    // for signed variable display
                    if (f_ParamPassed[u8_paramNo] < 0)
                    {
                        pu8_Payload[u16_size++] = '-';
                        f_ParamPassed[u8_paramNo] = f_ParamPassed[u8_paramNo] * (-1);
                    }


                /* Yes, I know I did not placed a break; here.. ;)  Intentionally done to reuse code of unsigned buffer filling,
                                                     as -ve sign is now taken care off.. <sandeep>*/
                case 'u':
                    //for integer display
                    u32_ValueInt = ( unsigned int ) f_ParamPassed[u8_paramNo];
                    u8_DigitCount = 0;
                    u8_SaveDigitCount = 0;
                    while (u32_ValueInt)
                    {
                        u32_ValueInt = u32_ValueInt / 10;
                        u8_DigitCount++;
                    }


                    // Check how many buffer bytes will be filled to write to passed buffer
                    if ((u16_MaxSizeToHandle - 1) <= u16_size + u8_DigitCount)
                    {   // Cant write variable's complete value. So go out directly.
                        pu8_Payload[u16_size] = '\0';   // write a null at the last memory location of the passed buffer. Buffer overflow case
                        break;
                    }


                    u32_ValueInt = ( unsigned int ) f_ParamPassed[u8_paramNo];
                    u8_SaveDigitCount = u8_DigitCount;

                    if (u8_DigitCount)
                    {
                        while (u8_DigitCount)
                        {
                            pu8_Payload[u16_size + u8_DigitCount - 1] = 0x30 + (u32_ValueInt % 10);
                            u32_ValueInt = u32_ValueInt / 10;
                            u8_DigitCount--;
                        }


                        u16_size = u16_size + u8_SaveDigitCount - 1;
                    }
                    else
                    {
                        pu8_Payload[u16_size] = 0x30;   // write a zero then
                    }


                    u8_paramNo++;
                    *pBuffer++;
                    break;

                case 'x':
                case 'X':
                    // for hexadecimal display
                    u8_DigitCount = 0;
                    u8_SaveDigitCount = 0;
                    u32_ValueInt = ( unsigned int ) f_ParamPassed[u8_paramNo];
                    while (u32_ValueInt)
                    {
                        u32_ValueInt = u32_ValueInt / 16;
                        u8_DigitCount++;
                    }


                    // Check how many buffer bytes will be filled to write to passed buffer
                    if ((u16_MaxSizeToHandle - 1) <= u16_size + u8_DigitCount + 2 /*+2 for writing 0x*/ )
                    {   // Cant write variable's complete value. So go out directly.
                        pu8_Payload[u16_size] = '\0';   // write a null at the last memory location of the passed buffer. Buffer overflow case
                        break;
                    }


                    u32_ValueInt = ( unsigned int ) f_ParamPassed[u8_paramNo];
                    u8_SaveDigitCount = u8_DigitCount;

                    pu8_Payload[u16_size++] = '0';
                    pu8_Payload[u16_size++] = 'x';

                    if (u8_DigitCount)
                    {
                        while (u8_DigitCount)
                        {
                            if ((u32_ValueInt % 16) < 10)
                            {
                                pu8_Payload[u16_size + u8_DigitCount - 1] = 0x30 + (u32_ValueInt % 16);
                            }
                            else
                            {
                                pu8_Payload[u16_size + u8_DigitCount - 1] = 0x37 + (u32_ValueInt % 16);
                            }


                            u32_ValueInt = u32_ValueInt / 16;
                            u8_DigitCount--;
                        }


                        u16_size = u16_size + u8_SaveDigitCount - 1;
                    }
                    else
                    {
                        pu8_Payload[u16_size] = 0x30;   // write a zero then
                    }


                    u8_paramNo++;
                    *pBuffer++;

                    break;

                case 'f':
                case 'F':
                    //for float numbers
                    if (f_ParamPassed[u8_paramNo] < 0)
                    {
                        pu8_Payload[u16_size++] = '-';
                        f_ParamPassed[u8_paramNo] = f_ParamPassed[u8_paramNo] * (-1);
                    }


                    u8_DecimalPlace = 2;
                    u32_ValueInt = ( unsigned int ) f_ParamPassed[u8_paramNo];
                    u8_DigitCount = 0;
                    u8_SaveDigitCount = 0;
                    while (u32_ValueInt)
                    {
                        u32_ValueInt = u32_ValueInt / 10;
                        u8_DigitCount++;
                    }


                    // Check how many buffer bytes will be filled to write to passed buffer
                    if ((u16_MaxSizeToHandle - 1) <= u16_size + u8_DigitCount + 1 /*+1 for writing (.)*/ + 2 /*+2 for writing fractional part*/)
                    {   // Cant write variable's complete value. So go out directly.
                        pu8_Payload[u16_size] = '\0';   // write a null at the last memory location of the passed buffer. Buffer overflow case
                        break;
                    }


                    u32_ValueInt = ( unsigned int ) f_ParamPassed[u8_paramNo];
                    u8_SaveDigitCount = u8_DigitCount;

                    if (u8_DigitCount)
                    {
                        while (u8_DigitCount)
                        {
                            pu8_Payload[u16_size + u8_DigitCount - 1] = 0x30 + (u32_ValueInt % 10);
                            u32_ValueInt = u32_ValueInt / 10;
                            u8_DigitCount--;
                        }


                        u16_size = u16_size + u8_SaveDigitCount - 1;
                    }
                    else
                    {
                        pu8_Payload[u16_size] = 0x30;   // write a zero then
                    }


                    pu8_Payload[++u16_size] = '.';      // Decimal Point ascii value  '.' 0xFE
                    u8_SaveDecimalPlace = u8_DecimalPlace;
                    u32_ValueInt = ( unsigned int ) f_ParamPassed[u8_paramNo];
                    f_ParamPassed[u8_paramNo] = f_ParamPassed[u8_paramNo] - u32_ValueInt;
                    while (u8_DecimalPlace)
                    {
                        f_ParamPassed[u8_paramNo] = f_ParamPassed[u8_paramNo] * 10;
                        u8_DecimalPlace--;
                    }


                    u8_DecimalPlace = u8_SaveDecimalPlace;
                    u32_ValueInt = ( unsigned int ) f_ParamPassed[u8_paramNo];
                    while (u8_DecimalPlace)
                    {
                        pu8_Payload[u16_size + u8_DecimalPlace] = 0x30 + (u32_ValueInt % 10);
                        u32_ValueInt = u32_ValueInt / 10;
                        u8_DecimalPlace--;
                    }


                    u16_size = u16_size + u8_SaveDecimalPlace;

                    u8_paramNo++;
                    *pBuffer++;
                    break;

                case '%':
                    pu8_Payload[u16_size] = '%';
                    break;

                default:
                    break;
            }
        }


        *pBuffer++;
        u16_size++;
    }

    pu8_Payload[u16_size] = '\0'; // Add a null char at the end of trace mesg dump..

    return (u16_size);
}

