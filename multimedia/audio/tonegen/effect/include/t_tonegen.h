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
#ifndef INCLUSION_GUARD_T_TONEGEN_H
#define INCLUSION_GUARD_T_TONEGEN_H

/*************************************************************************
* Includes
*************************************************************************/

#include "t_basicdefinitions.h"

/*************************************************************************
* Types, constants and external variables
**************************************************************************/



//------------------------------- Tones ----------------------------------
/**
 * Tone types
 * Similar logic names and same ids as old ToneGen to keep compatibility.
 * An additional CUSTOM tone is added last, which is used as a sequence id.
 *
 * @param TG_TONE_TONE_OFF                Id for tone off
 * @param TG_TONE_KEY_1                   Id for key 1
 * @param TG_TONE_KEY_2                   Id for key 2
 * @param TG_TONE_KEY_3                   Id for key 3
 * @param TG_TONE_KEY_4                   Id for key 4
 * @param TG_TONE_KEY_5                   Id for key 5
 * @param TG_TONE_KEY_6                   Id for key 6
 * @param TG_TONE_KEY_7                   Id for key 7
 * @param TG_TONE_KEY_8                   Id for key 8
 * @param TG_TONE_KEY_9                   Id for key 9
 * @param TG_TONE_KEY_0                   Id for key 0
 * @param TG_TONE_KEY_ASTERISK            Id for key *
 * @param TG_TONE_KEY_NUMBERSIGN          Id for key #
 * @param TG_TONE_KEY_A                   Id for key a
 * @param TG_TONE_KEY_B                   Id for key B
 * @param TG_TONE_KEY_C                   Id for key C
 * @param TG_TONE_KEY_D                   Id for key D
 * @param TG_TONE_SILENCE                 Id for silence
 * @param TG_TONE_RING_TONE               Id for ring tone
 * @param TG_TONE_TRIPLE_TONE_950_HZ      Id for triple tone1
 * @param TG_TONE_TRIPLE_TONE_1400_HZ     Id for triple tone2
 * @param TG_TONE_TRIPLE_TONE_1800_HZ     Id for triple tone3
 * @param TG_TONE_RING_TONE_500_HZ        Id for ring tone1
 * @param TG_TONE_RING_TONE_1056_HZ       Id for ring tone2
 * @param TG_TONE_RING_TONE_1750_HZ       Id for ring tone3
 * @param TG_TONE_RING_TONE_1900_HZ       Id for ring tone4
 * @param TG_TONE_RING_TONE_2000_HZ       Id for ring tone5
 * @param TG_TONE_RING_TONE_2200_HZ       Id for ring tone6
 * @param TG_TONE_RING_TONE_2800_HZ       Id for ring tone7
 * @param TG_TONE_RING_TONE_480_620_HZ    Id for ring tone8
 * @param TG_TONE_RING_TONE_440_HZ        Id for ring tone9
 * @param TG_TONE_RING_TONE_440_480_HZ    Id for ring tone10
 * @param TG_TONE_OWNSOUND_0              Id for ring ownsound0
 * @param TG_TONE_OWNSOUND_1              Id for ring ownsound1
 * @param TG_TONE_OWNSOUND_2              Id for ring ownsound3
 * @param TG_TONE_OWNSOUND_3              Id for ring ownsound4
 * @param TG_TONE_OWNSOUND_4              Id for ring ownsound5
 * @param TG_TONE_OWNSOUND_5              Id for ring ownsound6
 * @param TG_TONE_OWNSOUND_6              Id for ring ownsound7
 * @param TG_TONE_OWNSOUND_7              Id for ring ownsound8
 * @param TG_TONE_CUSTOM                  Id for a custom tone
 * @param TG_TONE_EOL                     Id for end of list
 */
TYPEDEF_ENUM {
  TG_TONE_TONE_OFF                            = 0x0000,
  TG_TONE_KEY_1                               = 0x0100,
  TG_TONE_KEY_2                               = 0x0200,
  TG_TONE_KEY_3                               = 0x0300,
  TG_TONE_KEY_4                               = 0x0400,
  TG_TONE_KEY_5                               = 0x0500,
  TG_TONE_KEY_6                               = 0x0600,
  TG_TONE_KEY_7                               = 0x0700,
  TG_TONE_KEY_8                               = 0x0800,
  TG_TONE_KEY_9                               = 0x0900,
  TG_TONE_KEY_0                               = 0x0a00,
  TG_TONE_KEY_ASTERISK                        = 0x0b00,
  TG_TONE_KEY_NUMBERSIGN                      = 0x0c00,
  TG_TONE_KEY_A                               = 0x0d00,
  TG_TONE_KEY_B                               = 0x0e00,
  TG_TONE_KEY_C                               = 0x0f00,
  TG_TONE_KEY_D                               = 0x1000,
  TG_TONE_SILENCE                             = 0x1100,
  TG_TONE_RING_TONE                           = 0x1200,
  TG_TONE_TRIPLE_TONE_950_HZ                  = 0x1300,
  TG_TONE_TRIPLE_TONE_1400_HZ                 = 0x1400,
  TG_TONE_TRIPLE_TONE_1800_HZ                 = 0x1500,
  TG_TONE_RING_TONE_500_HZ                    = 0x1600,
  TG_TONE_RING_TONE_1056_HZ                   = 0x1700,
  TG_TONE_RING_TONE_1750_HZ                   = 0x1800,
  TG_TONE_RING_TONE_1900_HZ                   = 0x1900,
  TG_TONE_RING_TONE_2000_HZ                   = 0x1a00,
  TG_TONE_RING_TONE_2200_HZ                   = 0x1b00,
  TG_TONE_RING_TONE_2800_HZ                   = 0x1c00,
  TG_TONE_RING_TONE_480_620_HZ                = 0x1d00,
  TG_TONE_RING_TONE_440_HZ                    = 0x1e00,
  TG_TONE_RING_TONE_440_480_HZ                = 0x1f00,
  TG_TONE_OWNSOUND_0                          = 0x2000,
  TG_TONE_OWNSOUND_1                          = 0x2100,
  TG_TONE_OWNSOUND_2                          = 0x2200,
  TG_TONE_OWNSOUND_3                          = 0x2300,
  TG_TONE_OWNSOUND_4                          = 0x2400,
  TG_TONE_OWNSOUND_5                          = 0x2500,
  TG_TONE_OWNSOUND_6                          = 0x2600,
  TG_TONE_OWNSOUND_7                          = 0x2700,
  TG_TONE_CUSTOM                              = 0x2800,
  TG_TONE_EOL                                 = 0x2900  /* End Of List */
} SIGNED_ENUM16(TG_Tone_t);

/**
 *  Type to access the ToneGen instance.
 */
typedef uint32 TG_Handle_t;

/**
 * ToneGen job IDs. Used in the Job Descriptor.
 *
 * @param TG_JOB_ID_SIMPLE_TONE   Id for Simple tone. Tone composed of one frequency component.
 * @param TG_JOB_ID_DTMF_TONE     Id for DTMF tone.
 * @param TG_JOB_ID_CUSTOM_TONE   Id for Custom tone.
 * @param TG_JOB_ID_COMFORT_TONE  Id for Comfort tone.
 * @param TG_JOB_ID_OWN_TONE      Id for Own tone. Tone composed of three frequency components.
 * @param TG_JOB_ID_KEYCLICK      Id for KeyClick
 */
TYPEDEF_ENUM {
  TG_JOB_ID_SIMPLE_TONE,
  TG_JOB_ID_DTMF_TONE,
  TG_JOB_ID_CUSTOM_TONE,
  TG_JOB_ID_COMFORT_TONE,
  TG_JOB_ID_OWN_TONE,
  TG_JOB_ID_KEYCLICK
} SIGNED_ENUM16(TG_JobId_t);

/**
 * ToneGen job seq types. Use to describe a sequence.
 *
 * @param TG_JOB_SEQUENCE_START_MARK    Id for a start marker of a sequence
 * @param TG_JOB_SEQUENCE_DTMF_TONE     Sequence id for DTMF tone
 * @param TG_JOB_SEQUENCE_CUSTOM_TONE   Sequence id for Custom tone
 * @param TG_JOB_SEQUENCE_COMFORT_TONE  Sequence id for Comfort tone
 * @param TG_JOB_SEQUENCE_KEYCLICK      Sequence id for KeyClick
 * @param TG_JOB_SEQUENCE_END_MARK      Id for an end marker of a sequence
 * @param TG_JOB_SEQUENCE_EOS           Id for an end of sequence marker
 */
TYPEDEF_ENUM
{
  TG_JOB_SEQUENCE_START_MARK   = 0x0001, //
  TG_JOB_SEQUENCE_DTMF_TONE    = 0x0002, // followed by <int16> tone id
  TG_JOB_SEQUENCE_CUSTOM_TONE  = 0x0004, // followed by <int16> <int16> <int16> tone frequencies, 0=off
  TG_JOB_SEQUENCE_COMFORT_TONE = 0x0008, // followed by <int16> tone id
  TG_JOB_SEQUENCE_KEYCLICK     = 0x0010, // no additional data
  TG_JOB_SEQUENCE_END_MARK     = 0x0020, // followed by <int16> number of loops, -1 for infinite.
  TG_JOB_SEQUENCE_EOS          = 0x0040  // End Of Sequence
} SIGNED_ENUM16(TG_JobSequence_t);

/**
 * Job descriptor type for simple tones.
 *
 * @param Frequency Frequency of simple tone in Hz
 */
typedef struct
{
  uint16 Frequency;
} TG_SimpleToneDescriptor_t;


/**
 * Job descriptor for patch tone. Tone composed of three
 * frequencies.
 *
 *  @param FirstFrequency     First  frequency
 *  @param SecondFrequency    Second frequency
 *  @param ThirdFrequency     Third  frequency
 */
typedef struct
{
  uint16 FirstFrequency;
  uint16 SecondFrequency;
  uint16 ThirdFrequency;
} TG_OwnToneDescriptor_t;


/**
 * Job descriptor type for DTMF tones.
 *
 * @param Tone Tone id
 */
typedef struct
{
  TG_Tone_t Tone;
} TG_DTMF_ToneDescriptor_t;


/**
 * ToneGen job descriptor. Describes a job for the ToneGen.
 * Also a handle for the job descriptor is defined.
 * Note that KeyClick doesn't need any additional descriptor.
 *
 * @param Descriptor  Descriptor
 * @param Simple      Simple tone descriptor
 * @param Own         Own tone descriptor
 * @param DTMF        DTMF tone descriptor
 * @param Custom      Custom tone descriptor
 * @param Comfort     Comfort tone descriptor
 */
typedef struct {
  TG_JobId_t JobId;
  /* The following three variables is put in an union since it's an
     adequate solution eventhough SWA rules says that it should be avoided.*/
  union {
    TG_SimpleToneDescriptor_t  Simple;
    TG_OwnToneDescriptor_t     Own;
    TG_DTMF_ToneDescriptor_t   DTMF;
//    TG_CustomToneDescriptor_t  Custom;
//    TG_ComfortToneDescriptor_t Comfort;
  } Descriptor;
} TG_JobDescriptor_t;

/**
 *  A job descriptor handle.
 */
typedef TG_JobDescriptor_t* TG_JobHandle_t;


/**
 * This struct describes a PCM channel.
 *
 * @param Offset  The buffer offset where the first sample of this channel can be found.
 *                Graded in number of samples from the beginning of the buffer.
 *                Default value is 0 for first channel, 1 for second channel etc.
 */
typedef struct
{
  sint32 Offset;
} TG_PCM_ChannelConfig_t;


/**
 * Max number of channels in a PCM audio stream.
 */
#define TG_PCM_MAX_NO_OF_CHANNELS 2

/**
 * This struct contains PCM format configuration.
 *
 * @param Freq          The audio frequency / sample rate. Default value is 8 kHz.
 *                      This is a controller setting.
 * @param Resolution    The sample resolution, e.g. 16 bits. Default value is 16 bits.
 *                      This is a controller setting.
 * @param NoOfChannels  The number of channels, e.g. mono or stereo. Default value is mono (1).
 *                      This is a controller setting.
 * @param Interval      The interval in buffer between subsequent samples of one channel.
 *                      Graded in number of samples.
 *                      Default value is 1 if one channel, 2 if two channels etc, which
 *                      means continuous buffer layout if one channel and interleaved buffer
 *                      layout if multiple channels.
 *                      This is an endpoint setting.
 * @param Channels      The configuration of each channel.
 */
typedef struct {
  uint32 Freq;
  uint32 Resolution;
  uint32 NoOfChannels;
  sint32 Interval;
  TG_PCM_ChannelConfig_t Channels[TG_PCM_MAX_NO_OF_CHANNELS];
} TG_PCM_Config_t;


/**
 * TG result types
 *
 * @param TG_RESULT_OK                Result OK
 * @param TG_RESULT_OUT_OF_MEMORY     Error: Out of memory
 * @param TG_RESULT_NOT_IMPLEMENTED   Error: not implemented
 */
TYPEDEF_ENUM {
  TG_RESULT_OK,
  TG_RESULT_OUT_OF_MEMORY,
  TG_RESULT_NOT_IMPLEMENTED
} SIGNED_ENUM32(TG_Result_t);

#endif //INCLUSION_GUARD_T_TONEGEN_H
