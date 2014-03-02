/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


/*
 * Get the light level from sensor that shall be used by the camera
 * data = light level used, in lux (lux)
 * return = 0 if successful
 * All calculations done in the context of this function to be used
 * for camera
*/
int als_getlux(float *data)
{
*data = 7000.0;
return 0;
}

/*
 * To enable the sensor before calling get lux
 * return = 0 if successful
 * All calculations done in the context of this function to be used
 * for camera
*/
int als_enable()
{
return 0;
}

/*
 * To disable the sensor after calling get lux
 * return = 0 if successful
 * All calculations done in the context of this function to be used
 * for camera
*/
int als_disable()
{
return 0;
}


