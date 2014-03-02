/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

#ifndef _OMXILOSALDATETIMEIMPL_H
#define _OMXILOSALDATETIMEIMPL_H

#include <omxilosalservices.h>
using namespace omxilosalservices;

class OmxIlOsalDateTimeImpl : public OmxIlOsalDateTime
{
protected:

    friend void OmxIlOsalDateTime::OmxIlOsalGetDateTime(OmxIlOsalDateTimeType *);

private:

};

#endif // _OMXILOSALDATETIMEIMPL_H
