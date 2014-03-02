/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2010 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief
* \author ST-Ericsson
*/
/*****************************************************************************/
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "t_basicdefinitions.h"
#include "tg_tones.h"

#ifndef MIN
#define MIN(x,y) (x) < (y) ? (x) : (y)
#endif

// #define LOG_DEBUG

#ifdef LOG_DEBUG
#define LOGD printf("[Tonegen] "); printf
#else
#define LOGD(x,...)
#endif

#define IDENTIFIER_NOT_USED(x)

/*******************************************************************
*      File Scope types, constants and variables
*******************************************************************/
static void TG_Tones_FadeFrame(TG_Process_t *TG_p, const TG_PCM_Config_t* const PCM_Config_p, sint16* SampleBuf_p, uint32 Offset, uint32 NoOfSamples);
static void TG_Tones_Frame(TG_Process_t *TG_p, const TG_PCM_Config_t* const PCM_Config_p, sint16* SampleBuf_p, uint32 Offset, uint32 NoOfSamples);

//------------------------- ToneGen Definitions --------------------------

#ifndef TG_USE_DYNAMIC_TONE_WAVETABLE
const uint16 TG_FixedWaveTable1024[1 << TG_FIXED_WAVETABLE_INT_BITS] =
{
    0x0000,0x0032,0x0064,0x0096,0x00C9,0x00FB,0x012D,0x015F,0x0191,0x01C4,
    0x01F6,0x0228,0x025A,0x028C,0x02BE,0x02F0,0x0322,0x0354,0x0386,0x03B8,
    0x03EA,0x041C,0x044E,0x0480,0x04B1,0x04E3,0x0515,0x0546,0x0578,0x05A9,
    0x05DB,0x060C,0x063D,0x066F,0x06A0,0x06D1,0x0702,0x0733,0x0764,0x0795,
    0x07C6,0x07F6,0x0827,0x0858,0x0888,0x08B9,0x08E9,0x0919,0x0949,0x0979,
    0x09A9,0x09D9,0x0A09,0x0A39,0x0A68,0x0A98,0x0AC7,0x0AF6,0x0B25,0x0B54,
    0x0B83,0x0BB2,0x0BE1,0x0C10,0x0C3E,0x0C6C,0x0C9B,0x0CC9,0x0CF7,0x0D25,
    0x0D52,0x0D80,0x0DAE,0x0DDB,0x0E08,0x0E35,0x0E62,0x0E8F,0x0EBC,0x0EE8,
    0x0F15,0x0F41,0x0F6D,0x0F99,0x0FC5,0x0FF0,0x101C,0x1047,0x1073,0x109E,
    0x10C8,0x10F3,0x111E,0x1148,0x1172,0x119C,0x11C6,0x11F0,0x1219,0x1243,
    0x126C,0x1295,0x12BE,0x12E6,0x130F,0x1337,0x135F,0x1387,0x13AF,0x13D6,
    0x13FE,0x1425,0x144C,0x1473,0x1499,0x14BF,0x14E6,0x150C,0x1531,0x1557,
    0x157C,0x15A1,0x15C6,0x15EB,0x1610,0x1634,0x1658,0x167C,0x169F,0x16C3,
    0x16E6,0x1709,0x172C,0x174E,0x1771,0x1793,0x17B5,0x17D6,0x17F8,0x1819,
    0x183A,0x185B,0x187B,0x189B,0x18BB,0x18DB,0x18FB,0x191A,0x1939,0x1958,
    0x1976,0x1995,0x19B3,0x19D0,0x19EE,0x1A0B,0x1A28,0x1A45,0x1A62,0x1A7E,
    0x1A9A,0x1AB6,0x1AD1,0x1AED,0x1B08,0x1B22,0x1B3D,0x1B57,0x1B71,0x1B8B,
    0x1BA4,0x1BBD,0x1BD6,0x1BEF,0x1C07,0x1C1F,0x1C37,0x1C4F,0x1C66,0x1C7D,
    0x1C94,0x1CAA,0x1CC1,0x1CD6,0x1CEC,0x1D01,0x1D16,0x1D2B,0x1D40,0x1D54,
    0x1D68,0x1D7C,0x1D8F,0x1DA2,0x1DB5,0x1DC7,0x1DDA,0x1DEC,0x1DFD,0x1E0F,
    0x1E20,0x1E30,0x1E41,0x1E51,0x1E61,0x1E71,0x1E80,0x1E8F,0x1E9E,0x1EAC,
    0x1EBA,0x1EC8,0x1ED6,0x1EE3,0x1EF0,0x1EFD,0x1F09,0x1F15,0x1F21,0x1F2C,
    0x1F37,0x1F42,0x1F4D,0x1F57,0x1F61,0x1F6B,0x1F74,0x1F7D,0x1F86,0x1F8E,
    0x1F96,0x1F9E,0x1FA6,0x1FAD,0x1FB4,0x1FBB,0x1FC1,0x1FC7,0x1FCD,0x1FD2,
    0x1FD7,0x1FDC,0x1FE0,0x1FE4,0x1FE8,0x1FEC,0x1FEF,0x1FF2,0x1FF5,0x1FF7,
    0x1FF9,0x1FFB,0x1FFC,0x1FFD,0x1FFE,0x1FFE,0x1FFF,0x1FFE,0x1FFE,0x1FFD,
    0x1FFC,0x1FFB,0x1FF9,0x1FF7,0x1FF5,0x1FF2,0x1FEF,0x1FEC,0x1FE8,0x1FE4,
    0x1FE0,0x1FDC,0x1FD7,0x1FD2,0x1FCD,0x1FC7,0x1FC1,0x1FBB,0x1FB4,0x1FAD,
    0x1FA6,0x1F9E,0x1F96,0x1F8E,0x1F86,0x1F7D,0x1F74,0x1F6B,0x1F61,0x1F57,
    0x1F4D,0x1F42,0x1F37,0x1F2C,0x1F21,0x1F15,0x1F09,0x1EFD,0x1EF0,0x1EE3,
    0x1ED6,0x1EC8,0x1EBA,0x1EAC,0x1E9E,0x1E8F,0x1E80,0x1E71,0x1E61,0x1E51,
    0x1E41,0x1E30,0x1E20,0x1E0F,0x1DFD,0x1DEC,0x1DDA,0x1DC7,0x1DB5,0x1DA2,
    0x1D8F,0x1D7C,0x1D68,0x1D54,0x1D40,0x1D2B,0x1D16,0x1D01,0x1CEC,0x1CD6,
    0x1CC1,0x1CAA,0x1C94,0x1C7D,0x1C66,0x1C4F,0x1C37,0x1C1F,0x1C07,0x1BEF,
    0x1BD6,0x1BBD,0x1BA4,0x1B8B,0x1B71,0x1B57,0x1B3D,0x1B22,0x1B08,0x1AED,
    0x1AD1,0x1AB6,0x1A9A,0x1A7E,0x1A62,0x1A45,0x1A28,0x1A0B,0x19EE,0x19D0,
    0x19B3,0x1995,0x1976,0x1958,0x1939,0x191A,0x18FB,0x18DB,0x18BB,0x189B,
    0x187B,0x185B,0x183A,0x1819,0x17F8,0x17D6,0x17B5,0x1793,0x1771,0x174E,
    0x172C,0x1709,0x16E6,0x16C3,0x169F,0x167C,0x1658,0x1634,0x1610,0x15EB,
    0x15C6,0x15A1,0x157C,0x1557,0x1531,0x150C,0x14E6,0x14BF,0x1499,0x1473,
    0x144C,0x1425,0x13FE,0x13D6,0x13AF,0x1387,0x135F,0x1337,0x130F,0x12E6,
    0x12BE,0x1295,0x126C,0x1243,0x1219,0x11F0,0x11C6,0x119C,0x1172,0x1148,
    0x111E,0x10F3,0x10C8,0x109E,0x1073,0x1047,0x101C,0x0FF0,0x0FC5,0x0F99,
    0x0F6D,0x0F41,0x0F15,0x0EE8,0x0EBC,0x0E8F,0x0E62,0x0E35,0x0E08,0x0DDB,
    0x0DAE,0x0D80,0x0D52,0x0D25,0x0CF7,0x0CC9,0x0C9B,0x0C6C,0x0C3E,0x0C10,
    0x0BE1,0x0BB2,0x0B83,0x0B54,0x0B25,0x0AF6,0x0AC7,0x0A98,0x0A68,0x0A39,
    0x0A09,0x09D9,0x09A9,0x0979,0x0949,0x0919,0x08E9,0x08B9,0x0888,0x0858,
    0x0827,0x07F6,0x07C6,0x0795,0x0764,0x0733,0x0702,0x06D1,0x06A0,0x066F,
    0x063D,0x060C,0x05DB,0x05A9,0x0578,0x0546,0x0515,0x04E3,0x04B1,0x0480,
    0x044E,0x041C,0x03EA,0x03B8,0x0386,0x0354,0x0322,0x02F0,0x02BE,0x028C,
    0x025A,0x0228,0x01F6,0x01C4,0x0191,0x015F,0x012D,0x00FB,0x00C9,0x0096,
    0x0064,0x0032,0x0000,0xFFCE,0xFF9C,0xFF6A,0xFF37,0xFF05,0xFED3,0xFEA1,
    0xFE6F,0xFE3C,0xFE0A,0xFDD8,0xFDA6,0xFD74,0xFD42,0xFD10,0xFCDE,0xFCAC,
    0xFC7A,0xFC48,0xFC16,0xFBE4,0xFBB2,0xFB80,0xFB4F,0xFB1D,0xFAEB,0xFABA,
    0xFA88,0xFA57,0xFA25,0xF9F4,0xF9C3,0xF991,0xF960,0xF92F,0xF8FE,0xF8CD,
    0xF89C,0xF86B,0xF83A,0xF80A,0xF7D9,0xF7A8,0xF778,0xF747,0xF717,0xF6E7,
    0xF6B7,0xF687,0xF657,0xF627,0xF5F7,0xF5C7,0xF598,0xF568,0xF539,0xF50A,
    0xF4DB,0xF4AC,0xF47D,0xF44E,0xF41F,0xF3F0,0xF3C2,0xF394,0xF365,0xF337,
    0xF309,0xF2DB,0xF2AE,0xF280,0xF252,0xF225,0xF1F8,0xF1CB,0xF19E,0xF171,
    0xF144,0xF118,0xF0EB,0xF0BF,0xF093,0xF067,0xF03B,0xF010,0xEFE4,0xEFB9,
    0xEF8D,0xEF62,0xEF38,0xEF0D,0xEEE2,0xEEB8,0xEE8E,0xEE64,0xEE3A,0xEE10,
    0xEDE7,0xEDBD,0xED94,0xED6B,0xED42,0xED1A,0xECF1,0xECC9,0xECA1,0xEC79,
    0xEC51,0xEC2A,0xEC02,0xEBDB,0xEBB4,0xEB8D,0xEB67,0xEB41,0xEB1A,0xEAF4,
    0xEACF,0xEAA9,0xEA84,0xEA5F,0xEA3A,0xEA15,0xE9F0,0xE9CC,0xE9A8,0xE984,
    0xE961,0xE93D,0xE91A,0xE8F7,0xE8D4,0xE8B2,0xE88F,0xE86D,0xE84B,0xE82A,
    0xE808,0xE7E7,0xE7C6,0xE7A5,0xE785,0xE765,0xE745,0xE725,0xE705,0xE6E6,
    0xE6C7,0xE6A8,0xE68A,0xE66B,0xE64D,0xE630,0xE612,0xE5F5,0xE5D8,0xE5BB,
    0xE59E,0xE582,0xE566,0xE54A,0xE52F,0xE513,0xE4F8,0xE4DE,0xE4C3,0xE4A9,
    0xE48F,0xE475,0xE45C,0xE443,0xE42A,0xE411,0xE3F9,0xE3E1,0xE3C9,0xE3B1,
    0xE39A,0xE383,0xE36C,0xE356,0xE33F,0xE32A,0xE314,0xE2FF,0xE2EA,0xE2D5,
    0xE2C0,0xE2AC,0xE298,0xE284,0xE271,0xE25E,0xE24B,0xE239,0xE226,0xE214,
    0xE203,0xE1F1,0xE1E0,0xE1D0,0xE1BF,0xE1AF,0xE19F,0xE18F,0xE180,0xE171,
    0xE162,0xE154,0xE146,0xE138,0xE12A,0xE11D,0xE110,0xE103,0xE0F7,0xE0EB,
    0xE0DF,0xE0D4,0xE0C9,0xE0BE,0xE0B3,0xE0A9,0xE09F,0xE095,0xE08C,0xE083,
    0xE07A,0xE072,0xE06A,0xE062,0xE05A,0xE053,0xE04C,0xE045,0xE03F,0xE039,
    0xE033,0xE02E,0xE029,0xE024,0xE020,0xE01C,0xE018,0xE014,0xE011,0xE00E,
    0xE00B,0xE009,0xE007,0xE005,0xE004,0xE003,0xE002,0xE002,0xE001,0xE002,
    0xE002,0xE003,0xE004,0xE005,0xE007,0xE009,0xE00B,0xE00E,0xE011,0xE014,
    0xE018,0xE01C,0xE020,0xE024,0xE029,0xE02E,0xE033,0xE039,0xE03F,0xE045,
    0xE04C,0xE053,0xE05A,0xE062,0xE06A,0xE072,0xE07A,0xE083,0xE08C,0xE095,
    0xE09F,0xE0A9,0xE0B3,0xE0BE,0xE0C9,0xE0D4,0xE0DF,0xE0EB,0xE0F7,0xE103,
    0xE110,0xE11D,0xE12A,0xE138,0xE146,0xE154,0xE162,0xE171,0xE180,0xE18F,
    0xE19F,0xE1AF,0xE1BF,0xE1D0,0xE1E0,0xE1F1,0xE203,0xE214,0xE226,0xE239,
    0xE24B,0xE25E,0xE271,0xE284,0xE298,0xE2AC,0xE2C0,0xE2D5,0xE2EA,0xE2FF,
    0xE314,0xE32A,0xE33F,0xE356,0xE36C,0xE383,0xE39A,0xE3B1,0xE3C9,0xE3E1,
    0xE3F9,0xE411,0xE42A,0xE443,0xE45C,0xE475,0xE48F,0xE4A9,0xE4C3,0xE4DE,
    0xE4F8,0xE513,0xE52F,0xE54A,0xE566,0xE582,0xE59E,0xE5BB,0xE5D8,0xE5F5,
    0xE612,0xE630,0xE64D,0xE66B,0xE68A,0xE6A8,0xE6C7,0xE6E6,0xE705,0xE725,
    0xE745,0xE765,0xE785,0xE7A5,0xE7C6,0xE7E7,0xE808,0xE82A,0xE84B,0xE86D,
    0xE88F,0xE8B2,0xE8D4,0xE8F7,0xE91A,0xE93D,0xE961,0xE984,0xE9A8,0xE9CC,
    0xE9F0,0xEA15,0xEA3A,0xEA5F,0xEA84,0xEAA9,0xEACF,0xEAF4,0xEB1A,0xEB41,
    0xEB67,0xEB8D,0xEBB4,0xEBDB,0xEC02,0xEC2A,0xEC51,0xEC79,0xECA1,0xECC9,
    0xECF1,0xED1A,0xED42,0xED6B,0xED94,0xEDBD,0xEDE7,0xEE10,0xEE3A,0xEE64,
    0xEE8E,0xEEB8,0xEEE2,0xEF0D,0xEF38,0xEF62,0xEF8D,0xEFB9,0xEFE4,0xF010,
    0xF03B,0xF067,0xF093,0xF0BF,0xF0EB,0xF118,0xF144,0xF171,0xF19E,0xF1CB,
    0xF1F8,0xF225,0xF252,0xF280,0xF2AE,0xF2DB,0xF309,0xF337,0xF365,0xF394,
    0xF3C2,0xF3F0,0xF41F,0xF44E,0xF47D,0xF4AC,0xF4DB,0xF50A,0xF539,0xF568,
    0xF598,0xF5C7,0xF5F7,0xF627,0xF657,0xF687,0xF6B7,0xF6E7,0xF717,0xF747,
    0xF778,0xF7A8,0xF7D9,0xF80A,0xF83A,0xF86B,0xF89C,0xF8CD,0xF8FE,0xF92F,
    0xF960,0xF991,0xF9C3,0xF9F4,0xFA25,0xFA57,0xFA88,0xFABA,0xFAEB,0xFB1D,
    0xFB4F,0xFB80,0xFBB2,0xFBE4,0xFC16,0xFC48,0xFC7A,0xFCAC,0xFCDE,0xFD10,
    0xFD42,0xFD74,0xFDA6,0xFDD8,0xFE0A,0xFE3C,0xFE6F,0xFEA1,0xFED3,0xFF05,
    0xFF37,0xFF6A,0xFF9C,0xFFCE
};
#endif //TG_USE_DYNAMIC_TONE_WAVETABLE


#ifdef TG_USE_DYNAMIC_TONE_WAVETABLE
/*************************************************************
* TG_GetMSB
**************************************************************/
uint8 TG_GetMSB(uint32 Val)
{
  uint8 Msb = 0;
  if (Val & 0xffff0000) {
    Msb += 16;
    Val >>= 16;
  }
  if (Val & 0xff00) {
    Msb += 8;
    Val >>= 8;
  }
  if (Val & 0xf0) {
    Msb += 4;
    Val >>= 4;
  }
  if (Val & 0x0c) {
    Msb += 2;
    Val >>= 2;
  }
  if (Val & 0x02) {
    Msb += 1;
    Val >>= 1;
  }
  if (Val & 0x01) {
    Msb += 1;
  }
  return Msb;
}
#endif //TG_USE_DYNAMIC_TONE_WAVETABLE

//---------------------------------------------
// Called when reset a new tone
void TG_Tones_Set(TG_Process_t *TG_p, const TG_PCM_Config_t* const PCM_Config_p, sint32 NbrTones, const uint32* const Frq)
{
    sint32 i;

#ifdef TG_USE_DYNAMIC_TONE_WAVETABLE
    // Make sum of all tones fill up the max volume
    for (i = 0; i < NbrTones; i++) {
      (TG_p->Amplitude)[i] = TG_PCM_SAMPLE_MAX_AMPLITUDE / NbrTones;
    }
#else
    IDENTIFIER_NOT_USED(NbrTones);
#endif
  // Generate tone PCM data
  // Calc no of samples per wave period
  for (i = 0; i < TG_NBR_SIMULTANEOUS_TONES; i++) {
    if (Frq[i] != 0) {
      double d;
      uint32 Int;
      uint32 Frac;
      uint8 IntBits;
      uint8 FracBits;

#ifdef TG_USE_DYNAMIC_TONE_WAVETABLE
      sint32 j;
      float k;
      sint32 NofSamples;
      sint16 *Buf_p;
      uint32 Amp;
      boolean AllocationDone;

      // Calc optimal (minimal) length of table (2^N), where offset >= 1

      LOGD("[ToneGen] Sample period %u %u.\n", PCM_Config_p->Freq, Frq[i]);

      (TG_p->NoOfSamplesPerPeriod)[i] = PCM_Config_p->Freq / Frq[i];

      LOGD("[ToneGen] Sample period %d / %d = %d.\n", PCM_Config_p->Freq, Frq[i], (TG_p->NoOfSamplesPerPeriod)[i]);

      if ((TG_p->NoOfSamplesPerPeriod)[i] != PCM_Config_p->Freq * Frq[i]) {
        LOGD("[ToneGen] NoOfSamplesPerPeriod: %d\n", (TG_p->NoOfSamplesPerPeriod)[i]);
        // take ceil of division result
        (TG_p->NoOfSamplesPerPeriod)[i]++;
      }
      // check if power of 2
      if ((TG_p->NoOfSamplesPerPeriod)[i] & ((TG_p->NoOfSamplesPerPeriod)[i] - 1)) {
        // No power of 2
        IntBits = TG_GetMSB((TG_p->NoOfSamplesPerPeriod)[i]);
      }
      else {
        // ok, power of 2, keep length
        LOGD("[ToneGen] POWER OF 2 is %d.\n", (TG_p->NoOfSamplesPerPeriod)[i]);
        IntBits = TG_GetMSB((TG_p->NoOfSamplesPerPeriod)[i]) - 1;
      }

      // check that table is not too small
      if (IntBits < TG_MIN_NBR_BITS_FOR_TONE_WAVE_TABLE) {
        // limit to use min table entries
        IntBits = TG_MIN_NBR_BITS_FOR_TONE_WAVE_TABLE;
      }

      // check that table is not too large
      if (IntBits > TG_MAX_NBR_BITS_FOR_TONE_WAVE_TABLE) {
        // limit to use max table entries
        IntBits = TG_MAX_NBR_BITS_FOR_TONE_WAVE_TABLE;
      }

      // Try Alloc period buffer
      AllocationDone = FALSE;
      do {
        (TG_p->NoOfSamplesPerPeriod)[i] = (1 << IntBits);
        (TG_p->PeriodBuf_p)[i] = (sint16*)malloc((TG_p->NoOfSamplesPerPeriod)[i] * sizeof(sint16));
        if ((TG_p->PeriodBuf_p)[i] == NULL) {
          // if memory short, try alloc smaller table
          if (IntBits == 0) {
            LOGD("[ToneGen] Could not allocate 1 byte on HEAP.\n");
            A_ASSERT_(FALSE);
            break;
          }
          else {
            // try alloc a smaller chunk
            IntBits--;
          }
        }
        else {
          AllocationDone = TRUE;
        }
      } while (! AllocationDone);

      // if memory allocation successful
      if (AllocationDone) {
        // index in Int:Frac fix point format
        (TG_p->PeriodBufIdx)[i] = 0;

        LOGD("TG: sample period %d intbits %d\n", (TG_p->NoOfSamplesPerPeriod)[i], IntBits);

        FracBits = 32 - IntBits;

        // calc index offset adder
        d = (double)(Frq[i] << IntBits) / PCM_Config_p->Freq;

        // index offset adder in IntBits:FracBits fix point format
        Int = (uint32)d;
        d -= Int;
        d *= (1 << FracBits);
        Frac = (uint32)d;

        // Set fix point fraction number of how much to add to index each iteration
        (TG_p->PeriodBufIdxAdd)[i] = (Int << FracBits) | Frac;
        (TG_p->PeriodBufIdxAddFracBits)[i] = FracBits;

        // Load variables to registers to make code faster
        NofSamples = (TG_p->NoOfSamplesPerPeriod)[i];
        k = TG_PI2 / NofSamples;
        Buf_p = (TG_p->PeriodBuf_p)[i];
        Amp = (TG_p->Amplitude)[i];

        // Generate one wave period...
        for (j = NofSamples - 1; j >= 0; j--) {
          Buf_p[j] = (sint16)(Amp * sin(j * k));
        }
      } // if buf not NULL
#else //TG_USE_DYNAMIC_TONE_WAVETABLE

      IntBits = TG_FIXED_WAVETABLE_INT_BITS;
      FracBits = TG_FIXED_WAVETABLE_FRAC_BITS;

      (TG_p->NoOfSamplesPerPeriod)[i] = (1 << IntBits);
      (TG_p->PeriodBuf_p)[i] = (sint16*)TG_FixedWaveTable1024;

      // index in Int:Frac fix point format
      (TG_p->PeriodBufIdx)[i] = 0;

      // calc index offset adder (10 intbits)
      d = (double)(Frq[i] << IntBits) / PCM_Config_p->Freq; //lint !e790

      // index offset adder in IntBits:FracBits fix point format
      Int = (uint32)d;
      d -= Int;
      d *= (1 << FracBits); //lint !e790
      Frac = (uint32)d;

      LOGD("TG_Tones_Set: Frq[%d]: %u Freq: %u Int: %u Fraq: %u\n", i, Frq[i], PCM_Config_p->Freq, Int, Frac);
      // Set fix point fraction number of how much to add to index each iteration
      (TG_p->PeriodBufIdxAdd)[i] = (Int << FracBits) | Frac;
#endif //TG_USE_DYNAMIC_TONE_WAVETABLE
    } // if Frq != 0
    else {
      (TG_p->PeriodBuf_p)[i] = NULL;
    }
  } // for
}

//---------------------------------------------
// Init tone
void TG_Tones_Init(TG_Process_t *TG_p,
                   const TG_PCM_Config_t* const PCM_Config_p,
                   const TG_JobDescriptor_t* const Job_p)
{
  int i;
  uint32 Frq[TG_NBR_SIMULTANEOUS_TONES];
  sint32 NbrTones;

  for (i = 0; i < TG_NBR_SIMULTANEOUS_TONES; i++) {
    Frq[i] = 0;
#ifdef TG_USE_DYNAMIC_TONE_WAVETABLE
    (TG_p->Amplitude)[i] = 0;
#endif
    (TG_p->PeriodBuf_p)[i] = NULL;
  }

  switch (Job_p->JobId) {
    //--------------
    case TG_JOB_ID_SIMPLE_TONE:
    {
      NbrTones = 0;
      if (Job_p->Descriptor.Simple.Frequency  != 0)
        Frq[NbrTones++] = Job_p->Descriptor.Simple.Frequency;
      break;
    }

    //--------------
    case TG_JOB_ID_OWN_TONE:
    {
      NbrTones = 0;
      if (Job_p->Descriptor.Own.FirstFrequency  != 0)
        Frq[NbrTones++] = Job_p->Descriptor.Own.FirstFrequency;
      if (Job_p->Descriptor.Own.SecondFrequency != 0)
        Frq[NbrTones++] = Job_p->Descriptor.Own.SecondFrequency;
      if (Job_p->Descriptor.Own.ThirdFrequency  != 0)
        Frq[NbrTones++] = Job_p->Descriptor.Own.ThirdFrequency;
      break;
    }

    //--------------
    case TG_JOB_ID_DTMF_TONE:
    {
      NbrTones = 0;
      // Note; DTMF constants could be put in a table in future version.
      switch (Job_p->Descriptor.DTMF.Tone) {
        case TG_TONE_TONE_OFF:
          break;
        case TG_TONE_KEY_1:
          Frq[NbrTones++] = 697;
          Frq[NbrTones++] = 1209;
          break;
        case TG_TONE_KEY_2:
          Frq[NbrTones++] = 697;
          Frq[NbrTones++] = 1336;
        break;
        case TG_TONE_KEY_3:
          Frq[NbrTones++] = 697;
          Frq[NbrTones++] = 1477;
          break;
        case TG_TONE_KEY_4:
          Frq[NbrTones++] = 770;
          Frq[NbrTones++] = 1209;
          break;
        case TG_TONE_KEY_5:
          Frq[NbrTones++] = 770;
          Frq[NbrTones++] = 1336;
          break;
        case TG_TONE_KEY_6:
          Frq[NbrTones++] = 770;
          Frq[NbrTones++] = 1477;
          break;
        case TG_TONE_KEY_7:
          Frq[NbrTones++] = 852;
          Frq[NbrTones++] = 1209;
          break;
        case TG_TONE_KEY_8:
          Frq[NbrTones++] = 852;
          Frq[NbrTones++] = 1336;
          break;
        case TG_TONE_KEY_9:
          Frq[NbrTones++] = 852;
          Frq[NbrTones++] = 1477;
          break;
        case TG_TONE_KEY_0:
          Frq[NbrTones++] = 941;
          Frq[NbrTones++] = 1336;
          break;
        case TG_TONE_KEY_ASTERISK:
          Frq[NbrTones++] = 941;
          Frq[NbrTones++] = 1209;
          break;
        case TG_TONE_KEY_NUMBERSIGN:
          Frq[NbrTones++] = 941;
          Frq[NbrTones++] = 1477;
          break;
        case TG_TONE_KEY_A:
          Frq[NbrTones++] = 697;
          Frq[NbrTones++] = 1633;
          break;
        case TG_TONE_KEY_B:
          Frq[NbrTones++] = 770;
          Frq[NbrTones++] = 1633;
          break;
        case TG_TONE_KEY_C:
          Frq[NbrTones++] = 852;
          Frq[NbrTones++] = 1633;
          break;
        case TG_TONE_KEY_D:
          Frq[NbrTones++] = 941;
          Frq[NbrTones++] = 1633;
          break;
        case TG_TONE_SILENCE:
          break;
        case TG_TONE_RING_TONE:
          Frq[NbrTones++] = 425;
          break;
        case TG_TONE_TRIPLE_TONE_950_HZ:
          Frq[NbrTones++] = 950;
          break;
        case TG_TONE_TRIPLE_TONE_1400_HZ:
          Frq[NbrTones++] = 1400;
          break;
        case TG_TONE_TRIPLE_TONE_1800_HZ:
          Frq[NbrTones++] = 1800;
          break;
        case TG_TONE_RING_TONE_500_HZ:
          Frq[NbrTones++] = 500;
          break;
        case TG_TONE_RING_TONE_1056_HZ:
          Frq[NbrTones++] = 1056;
          break;
        case TG_TONE_RING_TONE_1750_HZ:
          Frq[NbrTones++] = 1750;
          break;
        case TG_TONE_RING_TONE_1900_HZ:
          Frq[NbrTones++] = 1900;
          break;
        case TG_TONE_RING_TONE_2000_HZ:
          Frq[NbrTones++] = 2000;
          break;
        case TG_TONE_RING_TONE_2200_HZ:
          Frq[NbrTones++] = 2200;
          break;
        case TG_TONE_RING_TONE_2800_HZ:
          Frq[NbrTones++] = 2800;
          break;
        case TG_TONE_RING_TONE_480_620_HZ:
          Frq[NbrTones++] = 620;
          Frq[NbrTones++] = 480;
          break;
        case TG_TONE_RING_TONE_440_HZ:
          Frq[NbrTones++] = 440;
          break;
        case TG_TONE_RING_TONE_440_480_HZ:
          Frq[NbrTones++] = 480;
          Frq[NbrTones++] = 440;
          break;
        default:
          // keep all zero
          break;
      } // switch
      break;
    } // case DTMF TONE

  default:
    NbrTones = 0;
    break;
  }

  TG_Tones_Set(TG_p, PCM_Config_p, NbrTones, (uint32*)Frq);
}

//---------------------------------------------
// Fill next frame with PCM output
void TG_Tones_NextFrame(TG_Process_t *TG_p, const TG_PCM_Config_t* const PCM_Config_p, sint16* SampleBuf_p, uint32 Offset, uint32 NoOfSamples)
{
  /**
   * Generate NoOfSamples samples. If tg is fading then fade-in/out NumberOfSamplesToFade and switch
   * state when done.
   */

  while (NoOfSamples != 0) {
    switch (TG_p->FadingState) {
      case TG_FADING_STATE_FADING_LEFT:
      case TG_FADING_STATE_FADING_RIGHT:
      case TG_FADING_STATE_FADING_BOTH:
        {
          /**
            * Tg is fading in/out. Generate up to NoOfSamples samples. Calculate number of
            * samples left to fade and change state when tg has produced that amount.
            */
          uint32 SamplesLeftToFade = 0;

          switch (TG_p->FadingState) {
            case TG_FADING_STATE_FADING_LEFT:
              SamplesLeftToFade = TG_p->channelVolume[0].NumberOfSamplesToFade -
                                  TG_p->channelVolume[0].FadeSampleCounter;
              break;
            case TG_FADING_STATE_FADING_RIGHT:
              SamplesLeftToFade = TG_p->channelVolume[1].NumberOfSamplesToFade -
                                  TG_p->channelVolume[1].FadeSampleCounter;
              break;
            case TG_FADING_STATE_FADING_BOTH:
              SamplesLeftToFade = MIN(TG_p->channelVolume[0].NumberOfSamplesToFade -
                                      TG_p->channelVolume[0].FadeSampleCounter,
                                      TG_p->channelVolume[1].NumberOfSamplesToFade -
                                      TG_p->channelVolume[1].FadeSampleCounter);
              break;
          }

          /**
            * Do the fading
            */
          if (SamplesLeftToFade > NoOfSamples) {
            TG_Tones_FadeFrame(TG_p, PCM_Config_p, SampleBuf_p, Offset, NoOfSamples);
            TG_p->channelVolume[0].FadeSampleCounter += NoOfSamples;
            TG_p->channelVolume[1].FadeSampleCounter += NoOfSamples;
            NoOfSamples = 0;
          }
          else {
            TG_Tones_FadeFrame(TG_p, PCM_Config_p, SampleBuf_p, Offset, SamplesLeftToFade);
            Offset += SamplesLeftToFade;
            NoOfSamples -= SamplesLeftToFade;
            TG_p->channelVolume[0].FadeSampleCounter += SamplesLeftToFade;
            TG_p->channelVolume[1].FadeSampleCounter += SamplesLeftToFade;

            switch (TG_p->FadingState) {
              case TG_FADING_STATE_FADING_LEFT:
              case TG_FADING_STATE_FADING_RIGHT:
                TG_p->FadingState = TG_FADING_STATE_NONE;
                break;
              case TG_FADING_STATE_FADING_BOTH:
                if (PCM_Config_p->NoOfChannels == 1) {
                  TG_p->FadingState = TG_FADING_STATE_NONE;
                }
                else if (TG_p->channelVolume[0].NumberOfSamplesToFade == TG_p->channelVolume[0].FadeSampleCounter &&
                         TG_p->channelVolume[1].NumberOfSamplesToFade == TG_p->channelVolume[1].FadeSampleCounter) {
                  TG_p->FadingState = TG_FADING_STATE_NONE;
                }
                else if (TG_p->channelVolume[0].NumberOfSamplesToFade == TG_p->channelVolume[0].FadeSampleCounter) {
                  TG_p->FadingState = TG_FADING_STATE_FADING_RIGHT;
                }
                else {
                  TG_p->FadingState = TG_FADING_STATE_FADING_LEFT;
                }
                break;
            }

            if (TG_p->FadingState == TG_FADING_STATE_NONE &&
                 ((PCM_Config_p->NoOfChannels == 1    && TG_p->channelVolume[0].Volume == 0) ||
                  (TG_p->channelVolume[0].Volume == 0 && TG_p->channelVolume[1].Volume == 0))) {
              TG_p->FadingState=TG_FADING_STATE_FADED;
            }
          }
        }
        break;

      case TG_FADING_STATE_FADED:
        /** Tg has faded out; Generate silence. */
        TG_Tones_ClearFrame(TG_p, PCM_Config_p, SampleBuf_p, Offset, NoOfSamples);
        NoOfSamples = 0;
        break;

      default:
        TG_Tones_Frame(TG_p, PCM_Config_p, SampleBuf_p, Offset, NoOfSamples);
        NoOfSamples = 0;
        break;
    }
  }
}

//---------------------------------------------
// Fill next frame with zero
void TG_Tones_ClearFrame(TG_Process_t *TG_p, const TG_PCM_Config_t* const PCM_Config_p, sint16* SampleBuf_p, uint32 Offset, uint32 NoOfSamples)
{
  uint32 SampleIdx;
  uint32 ChIdx;

  IDENTIFIER_NOT_USED(TG_p);

  // Generate data.
  for (SampleIdx = 0; SampleIdx < NoOfSamples; SampleIdx++) {
    // Copy sample to all channels
    for (ChIdx = 0; ChIdx < PCM_Config_p->NoOfChannels; ChIdx++) {
      SampleBuf_p[((SampleIdx + Offset) * PCM_Config_p->Interval) + PCM_Config_p->Channels[ChIdx].Offset] = 0; //lint !e737
    }
  }
}


//---------------------------------------------------------------------------
// Generate NoOfSamples samples worth of PCM output, with fading applied.
static void TG_Tones_FadeFrame(TG_Process_t *TG_p, const TG_PCM_Config_t* const PCM_Config_p, sint16* SampleBuf_p, uint32 Offset, uint32 NoOfSamples)
{
  uint32 SampleIdx;
  uint32 ChIdx;
  uint32 j;
  boolean Fading[2];
  uint16 Volume[2];

  if (TG_p->FadingState == TG_FADING_STATE_FADING_BOTH) {
    Fading[0] = TRUE;
    Fading[1] = TRUE;
  }
  else if (TG_p->FadingState == TG_FADING_STATE_FADING_LEFT) {
    Fading[0] = TRUE;
    Fading[1] = FALSE;
    Volume[1] = TG_p->channelVolume[1].Volume;
  }
  else {
    Fading[0] = FALSE;
    Fading[1] = TRUE;
    Volume[0] = TG_p->channelVolume[0].Volume;
  }

  // Generate data.
  for (SampleIdx = 0; SampleIdx < NoOfSamples; SampleIdx++) {
    sint16 Sample = 0;
    for (j = 0; j < TG_NBR_SIMULTANEOUS_TONES; j++) {
      sint16 *WaveTable_p = (TG_p->PeriodBuf_p)[j];
      if (WaveTable_p != NULL) {
#ifdef TG_USE_DYNAMIC_TONE_WAVETABLE
        Sample += WaveTable_p[ (TG_p->PeriodBufIdx)[j] >> (TG_p->PeriodBufIdxAddFracBits)[j] ];
#else
        Sample += WaveTable_p[ (TG_p->PeriodBufIdx)[j] >> TG_FIXED_WAVETABLE_FRAC_BITS ];
#endif
        // Add wave table offset
        (TG_p->PeriodBufIdx)[j] += (TG_p->PeriodBufIdxAdd)[j];
      }
    }

    // Copy sample to all channels
    for (ChIdx = 0; ChIdx < PCM_Config_p->NoOfChannels; ChIdx++) {
      sint16 Value;
      if (Fading[ChIdx]) {
        TG_p->channelVolume[ChIdx].FadingOffset += TG_p->channelVolume[ChIdx].FadingCoeff;
        Volume[ChIdx] = TG_p->channelVolume[ChIdx].FadingOffset;
      }
      Value = (sint16)(((sint32)Sample * Volume[ChIdx]) >> 15); //lint !e704
      SampleBuf_p[((SampleIdx + Offset) * PCM_Config_p->Interval) + PCM_Config_p->Channels[ChIdx].Offset] = Value; //lint !e737
    }
  }
}

//---------------------------------------------------------------------------
// Generate NoOfSamples samples worth of PCM output, without fading applied.
static void TG_Tones_Frame(TG_Process_t *TG_p, const TG_PCM_Config_t* const PCM_Config_p, sint16* SampleBuf_p, uint32 Offset, uint32 NoOfSamples)
{
  uint32 SampleIdx;
  uint32 ChIdx;
  uint32 j;

  // Generate data.
  for (SampleIdx = 0; SampleIdx < NoOfSamples; SampleIdx++) {
    sint16 Sample = 0;
    for (j = 0; j < TG_NBR_SIMULTANEOUS_TONES; j++) {
      sint16 *WaveTable_p = (TG_p->PeriodBuf_p)[j];
      if (WaveTable_p != NULL) {
#ifdef TG_USE_DYNAMIC_TONE_WAVETABLE
        Sample += WaveTable_p[ (TG_p->PeriodBufIdx)[j] >> (TG_p->PeriodBufIdxAddFracBits)[j] ];
#else
        Sample += WaveTable_p[ (TG_p->PeriodBufIdx)[j] >> TG_FIXED_WAVETABLE_FRAC_BITS ];
#endif
        // Add wave table offset
        (TG_p->PeriodBufIdx)[j] += (TG_p->PeriodBufIdxAdd)[j];
      }
    }

    // Copy sample to all channels
    for (ChIdx = 0; ChIdx < PCM_Config_p->NoOfChannels; ChIdx++) {
      sint16 Value = (sint16)(((sint32)Sample * TG_p->channelVolume[ChIdx].Volume) >> 15); //lint !e704
      SampleBuf_p[((SampleIdx + Offset) * PCM_Config_p->Interval) + PCM_Config_p->Channels[ChIdx].Offset] = Value; //lint !e737
    }
  }
}

//-----------------------------------------------------------------------
// Checks if Tone is a valid tone.
boolean ToneGen_IsValidTone(TG_Tone_t Tone)
{
  switch (Tone) {
    case TG_TONE_TONE_OFF:
    case TG_TONE_KEY_1:
    case TG_TONE_KEY_2:
    case TG_TONE_KEY_3:
    case TG_TONE_KEY_4:
    case TG_TONE_KEY_5:
    case TG_TONE_KEY_6:
    case TG_TONE_KEY_7:
    case TG_TONE_KEY_8:
    case TG_TONE_KEY_9:
    case TG_TONE_KEY_0:
    case TG_TONE_KEY_ASTERISK:
    case TG_TONE_KEY_NUMBERSIGN:
    case TG_TONE_KEY_A:
    case TG_TONE_KEY_B:
    case TG_TONE_KEY_C:
    case TG_TONE_KEY_D:
    case TG_TONE_SILENCE:
    case TG_TONE_RING_TONE:
    case TG_TONE_TRIPLE_TONE_950_HZ:
    case TG_TONE_TRIPLE_TONE_1400_HZ:
    case TG_TONE_TRIPLE_TONE_1800_HZ:
    case TG_TONE_RING_TONE_500_HZ:
    case TG_TONE_RING_TONE_1056_HZ:
    case TG_TONE_RING_TONE_1750_HZ:
    case TG_TONE_RING_TONE_1900_HZ:
    case TG_TONE_RING_TONE_2000_HZ:
    case TG_TONE_RING_TONE_2200_HZ:
    case TG_TONE_RING_TONE_2800_HZ:
    case TG_TONE_RING_TONE_480_620_HZ:
    case TG_TONE_RING_TONE_440_HZ:
    case TG_TONE_RING_TONE_440_480_HZ:
    case TG_TONE_OWNSOUND_0:
    case TG_TONE_OWNSOUND_1:
    case TG_TONE_OWNSOUND_2:
    case TG_TONE_OWNSOUND_3:
    case TG_TONE_OWNSOUND_4:
    case TG_TONE_OWNSOUND_5:
    case TG_TONE_OWNSOUND_6:
    case TG_TONE_OWNSOUND_7:
      return TRUE;
    default:
      return FALSE;
  }
}
