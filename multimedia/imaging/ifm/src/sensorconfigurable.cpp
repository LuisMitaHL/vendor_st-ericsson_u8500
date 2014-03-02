/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "variationif.h"
#include "sensorconfigurable.h"

CSensorConfigurable::CSensorConfigurable(enumCameraSlot slot) : CSensor(slot), resolutionsLoaded(false)
{
    resolutions.count = 0;
    for (int i=0 ; i<MAX_SENSOR_RESOLUTIONS ; i++) {
        resolutions.res[i].fov_x = 0;
        resolutions.res[i].fov_y = 0;
        resolutions.res[i].x = 0;
        resolutions.res[i].y = 0;
    }
}

CSensorConfigurable::~CSensorConfigurable()
{
}

bool CSensorConfigurable::getResolutions(t_sensorResolutions *resolutions)
{
  // check if sensorId is set
  resolutions->count = 0;
  if (!i_SensorDescriptor.manufacturer || !i_SensorDescriptor.model)
    return false;

  // check if already loaded
  if (!loadResolutions())
    return false;

  memcpy(resolutions, &this->resolutions, sizeof(this->resolutions));
  return true;
}

t_uint32 CSensorConfigurable::getBayerWidth()
{
  if (!loadResolutions())
    MSG1("WARNING! loading resolutions from CDCC failed, reverting to default width = %ld\n", i_BayerWidth);

  return CSensor::getBayerWidth();
}

void CSensorConfigurable::GetLensParameters(t_LensParameters *LensParameters)
{
  CSensor::GetLensParameters(LensParameters);
}

t_uint32 CSensorConfigurable::getBayerHeight()
{
  if (!loadResolutions())
    MSG1("WARNING! loading resolutions from CDCC failed, reverting to default height = %ld\n", i_BayerHeight);

  return CSensor::getBayerHeight();
}

bool CSensorConfigurable::loadResolutions()
{

#if defined (__ARM_ANDROID)
	int nonActiveLines = -1;
	return true;
#else
  int nonActiveLines = -1;
  if (resolutionsLoaded)
    return true;

  // try to load
  VariationIf *loader = VariationIf::getInstance();
  if (!loader) {
    return false;
  }
  bool ret = false;
  if (OMX_ErrorNone == loader->listOfResolutions(&i_SensorDescriptor, &this->resolutions, &nonActiveLines) &&
      this->resolutions.count > 0) {
    resolutionsLoaded = true;
    ret = true;

    // calculate bayer resolution, take the biggest one with 4:3 aspect ratio
    // if none, take biggest with any aspect ratio
    t_sensorResolutions res4to3;
    t_sensorResolution *bayerReso = &this->resolutions.res[0];
    memcpy(&res4to3, &this->resolutions, sizeof(t_sensorResolutions));
    filterResolutionsAspectRatio(&res4to3, ASPECT_RATIO_4TO3);
    if (res4to3.count > 0)
      bayerReso = &res4to3.res[0];

    if (nonActiveLines >= 0)
      setNberOfNonActiveLines(nonActiveLines);

    setBayerHeight(bayerReso->y + getNberOfNonActiveLines());
    setBayerWidth(bayerReso->x);
  }

  VariationIf::releaseInstance(loader);
  return ret;
 #endif
}
