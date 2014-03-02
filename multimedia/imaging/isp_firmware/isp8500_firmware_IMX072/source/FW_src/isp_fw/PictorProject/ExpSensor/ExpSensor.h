/**
 * \if       INCLUDE_IN_HTML_ONLY
 * \file      ExpSensor.h
 * \brief     Header File containing external function declarations and defines for
 *            the Layer between the Sensor & Exposure
 * \ingroup   ExpSensor
 * \endif
*/

#ifndef EXPSENSOR_H_
#define EXPSENSOR_H_
#include "Platform.h"

typedef enum
{
	/// The sensor supports only coarse integration lines
	IntegrationCapability_e_COARSE_ONLY,

	/// The sensor supports both coarse integration lines & fine integration pixels
	IntegrationCapability_e_COARSE_AND_FINE
} IntegrationCapability_te;

typedef enum
{
	/// The sensor does not support analog gain
	GainType_e_NONE,

	/// The sensor supports global analog gain only
	GainType_e_GLOBAL,

	/// The sensor supports 4 channel analog gains
	GainType_e_SEPARATE

} GainType_te;

/**

 * \struct 	  ExpSensor_SensorProperties_ts
 * \brief	  The ISP FW reports the following sensor properties
 * \ingroup   ExpCtrl

*/
typedef struct
{
	/// Specifies the SMIA minimum analog gain code for the active sensor
	uint16_t	u16_AnalogGainCodeMin;

	/// Specifies the SMIA maximum analog gain code for the active sensor
	uint16_t	u16_AnalogGainCodeMax;

	/// Specifies the SMIA analog gain code step for the active sensor
	uint16_t	u16_AnalogGainCodeStep;

	/// Specifies the SMIA minimum analog gain constant M0 for the active sensor
	int16_t		s16_AnalogGianConstantM0;

	/// Specifies the SMIA minimum analog gain constant C0 for the active sensor
	int16_t		s16_AnalogGianConstantC0;

	/// Specifies the SMIA minimum analog gain constant M1 for the active sensor
	int16_t		s16_AnalogGianConstantM1;

	/// Specifies the SMIA minimum analog gain constant C1 for the active sensor
	int16_t		s16_AnalogGianConstantC1;

	/// Specifies the minimum coarse integration lines for the active sensor
	uint16_t	u16_MinimumCoarseIntegrationLines;

	/// Specifies the SMIA coarse integration max margin for the active sensor (refer to SMIA specs for details)
	uint16_t	u16_CoarseIntegrationMaxMargin;

	/// Specifies the minimum fine integration pixels for the active sensor
	uint16_t	u16_MinimumFineIntegrationPixels;

	/// Specifies the fine integration max margin for the active sensor (refer to SMIA specs for details)
	uint16_t	u16_FineIntegrationMaxMargin;

	/// Specifies the integration capability (coarse only or coarse and fine) for the active sensor
	uint8_t		e_IntegrationCapability;
} ExpSensor_SensorProperties_ts;

extern ExpSensor_SensorProperties_ts	            g_ExpSensor_SensorProperties;

// for decreasing the analog gain, the relation between the Analog Gain
// and CodedGain must be known.
// if comstM1 is Zero, then CodedGain is directly proportional to AnalogGain x = gain*(c1/m0) - c0/m0 , gain = (m0*x + c0)/c1
// elif constM0 is Zero, then CodedGain is inversely proportional to AnalogGain x = c0/(m1*gain) - c1/m1 , gain = c0/(m1*x + c1)
// This function decrease analog gain by an amount equivalent to one coded gain step.


// for increasing the analog gain, the relation between the Analog Gain
// and CodedGain must be known.
// if comstM1 is Zero, then CodedGain is directly proportional to AnalogGain x = gain*(c1/m0) - c0/m0 , gain = (m0*x + c0)/c1
// elif constM0 is Zero, then CodedGain is inversely proportional to AnalogGain x = c0/(m1*gain) - c1/m1 , gain = c0/(m1*x + c1)
// This function decrease analog gain by an amount equivalent to one coded gain step.

#endif

