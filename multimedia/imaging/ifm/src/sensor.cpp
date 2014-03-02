/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/* +CR337836 */
#define DECLARE_AUTOVAR
/* -CR337836 */


#include "sensor.h"
#undef _CNAME_
#define _CNAME_ sensor
#define DUMP_THREAD_ID


/****************************************************************************/
/**
 * \brief 	standard constructor
 * \author 	monteilm
 * \        do nothing special at that time
 **/
/****************************************************************************/
CSensor::CSensor(enumCameraSlot aCcp_slot)
{
/* +CR337836 */
	GET_AND_SET_TRACE_LEVEL(ifm);
/* -CR337836 */

    MSG0("CSensor constructor called\n");
    i_CamSlot                     = aCcp_slot;
    iSensorPreBootNbElement       = 0;
    iSensorPostBootNbElement      = 0;
    iSensorSpecificNbElement      = 0;
    iSensorPatchSequenceNbElement = 0;
    iFocalLengthMm                = 0.0;

    i_SensorDescriptor.manufacturer = 0;
    i_SensorDescriptor.model        = 0;
    i_SensorDescriptor.revisionNumber = 0;
    i_SensorDescriptor.Fuse_Id[0] = 0;
    i_SensorDescriptor.Fuse_Id[1] = 0;
    i_SensorDescriptor.Fuse_Id[2] = 0;
    i_SensorDescriptor.Fuse_Id4   = 0;

    i_NberofNonActiveLines = 0;
    
    pSensorModes = NULL;
    iSensorModesNber = 0;
    iLensParameters.FocalLength = 0;
    iLensParameters.HorizontolViewAngle = 0;
    iLensParameters.VerticalViewAngle = 0;

    i_BayerWidth = 0;
    i_BayerHeight = 0;
    memset((void*)&iSensorPatchSequence,0,sizeof(iSensorPatchSequence[PATCH_SIZE_SEQUENCE]));
    memset((void*)&iSensorPostBootPEs,0,sizeof(iSensorPostBootPEs[MAX_POSTBOOT_PE]));
    memset((void*)&iSensorPreBootPEs,0,sizeof(iSensorPreBootPEs[MAX_PREBOOT_PE]));
    memset((void*)&iSensorSpecificPEs,0,sizeof(iSensorSpecificPEs[MAX_SPECIFIC_PE]));

    /* PE patches common to all sensors can be defined here:
     * iSensorPreBootPEs[iSensorPreBootNbElement].pe_addr   =  foo;
     * iSensorPreBootPEs[iSensorPreBootNbElement++].pe_data =  bar;
     */
}

/****************************************************************************/
/**
 * \brief 	brief
 * \author 	monteilm
 * \out     nb_element: pointer on number of element in the Preboot table to be return
 * \return  return the address of the Preboot table
 **/
/****************************************************************************/
ts_PageElement* CSensor::GetSensorPreBootTable(t_uint32 *aNb_Element)
{
    *aNb_Element = iSensorPreBootNbElement;
    return iSensorPreBootPEs;
}


/****************************************************************************/
/**
 * \brief   brief
 * \author  monteilm
 * \out     nb_element: pointer on number of element in the Postboot table to be return
 * \return  return the address of the Postboot table
 **/
/****************************************************************************/
ts_PageElement* CSensor::GetSensorPostBootTable(t_uint32 *aNb_Element)
{
    *aNb_Element = iSensorPostBootNbElement;
    return iSensorPostBootPEs;
}


/****************************************************************************/
/**
 * \brief   brief
 * \author  monteilm
 * \in      index: address of the page element the user wants the value
 * \return  return the value of the page element
 **/
/****************************************************************************/
t_uint16 CSensor::GetSensorPeByIndex(t_uint16 aIndex)
{
    t_uint32 i=0;

    for(i=0; i<iSensorPreBootNbElement; i++)
    {
        if(iSensorPreBootPEs[i].pe_addr == aIndex)
        {
            return iSensorPreBootPEs[i].pe_data;
        }
    }

    for(i=0; i<iSensorPostBootNbElement; i++)
    {
        if(iSensorPostBootPEs[i].pe_addr == aIndex)
        {
            return iSensorPostBootPEs[i].pe_data;
        }
    }

    DBC_ASSERT(false);
    return 0;
}


/****************************************************************************/
/**
 * \brief   brief
 * \out     aNb_Element: pointer on number of element in the Patch table to be return
 * \return  return the address of the Patch table
 *
 **/
/****************************************************************************/
ts_I2CReg* CSensor::GetSensorPatchTable(t_uint32 *aNb_Element)
{
	*aNb_Element=iSensorPatchSequenceNbElement;
    return(iSensorPatchSequence);
}



/****************************************************************************/
/**
 * \brief   brief
 * \out     aNb_Element: pointer on number of element in the Specific PE table to be return
 * \return  return the address of the Specific PE table
 *
 **/
/****************************************************************************/
ts_PageElement* CSensor::GetSensorSpecific(t_uint32 *aNb_Element)
{
	*aNb_Element=iSensorSpecificNbElement;
    return(iSensorSpecificPEs);
}

/****************************************************************************/
/**
 * \brief   Set the focal length
 * \in      Focal length in mm
 * \return  void
 *
 **/
/****************************************************************************/
void CSensor::SetFocalLengthMm(float aFocalLengthMm)
{
    iFocalLengthMm = aFocalLengthMm;
}

/****************************************************************************/
/**
 * \brief   Return the focal length
 * \return  Focal lenght in mm
 *
 **/
/****************************************************************************/
float CSensor::GetFocalLengthMm()
{
    return iFocalLengthMm;
}

/****************************************************************************/
/**
 * \brief 	Sensor class destructor
 * \author 	monteilm
 * \        do nothing special at that time
 *
 **/
/****************************************************************************/
CSensor::~CSensor()
{
    MSG0("destructor called\n");
}

void CSensor::SetSensorDescriptor(t_sensorDesciptor *pDesc )
{
    i_SensorDescriptor = *pDesc;
}

t_uint16 CSensor::GetSensorModel(void)
{
    return (i_SensorDescriptor.model);
}

t_uint8 CSensor::GetSensorManufacturer(void)
{
    return (i_SensorDescriptor.manufacturer);
}

t_uint8 CSensor::GetSensorRevNumber(void)
{
    return (i_SensorDescriptor.revisionNumber);
}

t_uint8 CSensor::GetFuseId1(void)
{
    return (i_SensorDescriptor.Fuse_Id[0]);
}

t_uint8 CSensor::GetFuseId2(void)
{
    return (i_SensorDescriptor.Fuse_Id[1]);
}

t_uint8 CSensor::GetFuseId3(void)
{
    return (i_SensorDescriptor.Fuse_Id[2]);
}

t_uint32 CSensor::GetFuseId4(void)
{
    return (i_SensorDescriptor.Fuse_Id4);
}

t_uint32 CSensor::getBayerWidth(void)
{
    return (i_BayerWidth);
}

t_uint32 CSensor::getBayerHeight(void)
{
    return (i_BayerHeight);
}

t_uint8 CSensor::getNberOfNonActiveLines(void)
{
    return (i_NberofNonActiveLines);
}

void CSensor::setBayerWidth(t_uint32 width)
{
    i_BayerWidth = width;
}

void CSensor::setBayerHeight(t_uint32 height)
{
    i_BayerHeight = height;
}

void CSensor::setNberOfNonActiveLines(t_uint8 nber)
{
    i_NberofNonActiveLines = nber;
}

void CSensor::setSensorModesPointer(t_uint8* pSensorMode)
{
    pSensorModes = pSensorMode;
}

t_uint8* CSensor::getSensorModesPointer(void)
{
    return (pSensorModes);
}

void CSensor::setSensorModesNber(t_uint16 SensorModesNber)
{
    iSensorModesNber = SensorModesNber;
}

t_uint16 CSensor::getSensorModesNber(void)
{
    return (iSensorModesNber);
}


bool CSensor::getResolutions(t_sensorResolutions *resolutions)
{
  return false;
}

void CSensor::GetLensParameters(t_LensParameters *LensParameters)
{
	*(t_LensParameters *)LensParameters = iLensParameters;
}

static bool isEqual(int x, int y, float ratio_fixed)
{
  const float delta = 0.005f;
  float ratio = (float)x / (float)y;
  if (ratio < ratio_fixed + delta && ratio > ratio_fixed - delta)
    return true;
  else
    return false;
}

void CSensor::filterResolutionsAspectRatio(t_sensorResolutions *resolutions, int filter)
{
  const float ratio_16_9 = 16.0f/9.0f;
  const float ratio_4_3 = 4.0f/3.0f;
  const float ratio_3_2 = 3.0f/2.0f;
  t_sensorResolutions old;
  MSG1("filter=0x%x\n", filter);
  if (filter == (int)ASPECT_RATIO_ALL)
    return;
  memcpy(&old, resolutions, sizeof(t_sensorResolutions));
  if (old.count > MAX_SENSOR_RESOLUTIONS)
    old.count = MAX_SENSOR_RESOLUTIONS;
  resolutions->count = 0;

  for (int i = 0; i < old.count; i++) {
    int match = 0;
    if (isEqual(old.res[i].x, old.res[i].y, ratio_4_3))
      match = ASPECT_RATIO_4TO3;
    else if (isEqual(old.res[i].x, old.res[i].y, ratio_16_9))
      match = ASPECT_RATIO_16TO9;
    else if (isEqual(old.res[i].x, old.res[i].y, ratio_3_2))
      match = ASPECT_RATIO_3TO2;
    else if (old.res[i].x == old.res[i].y)
      match = ASPECT_RATIO_1TO1;
    else
      match = ASPECT_RATIO_OTHER;

    MSG4("Reso %dx%d match=0x%x AR=%f\n", old.res[i].x, old.res[i].y, match,
	 (float)old.res[i].x/(float)old.res[i].y);

    if (match & filter) {
      /* include in result */
      resolutions->res[resolutions->count].x = old.res[i].x;
      resolutions->res[resolutions->count].y = old.res[i].y;
      resolutions->res[resolutions->count].fov_x = old.res[i].fov_x;
      resolutions->res[resolutions->count].fov_y = old.res[i].fov_y;
      resolutions->count++;
    }
  }
}
