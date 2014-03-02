/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdlib.h>
#include <string.h>
#include <err.h>
#include "mcde_kcompat.h"
#include "mcde_device.h"
#include "mcde_display.h"

#define to_mcde_display_driver(__drv) \
	container_of((__drv), struct mcde_display_driver, driver)

static int mcde_drv_suspend(struct device *_dev, pm_message_t state);
static int mcde_drv_resume(struct device *_dev);

struct device_driver *registered_driver = NULL;
struct mcde_display_device *registered_device = NULL;

static int mcde_suspend_device(struct device *dev, void *data)
{
	return 0;
}

static int mcde_resume_device(struct device *dev, void *data)
{
	return 0;
}

/* Bus driver */

static int mcde_bus_match(struct device *_dev, struct device_driver *driver)
{
	return 0;
}

static int mcde_bus_suspend(struct device *_dev, pm_message_t state)
{
	return 0;
}

static int mcde_bus_resume(struct device *_dev)
{
	return 0;
}

static int mcde_drv_probe(struct device *_dev)
{
	struct mcde_display_driver *drv = (struct mcde_display_driver *)&_dev->driver;
	struct mcde_display_device *dev = (struct mcde_display_device *)_dev;

	return drv->probe(dev);
}

static int mcde_drv_remove(struct device *_dev)
{
	struct mcde_display_driver *drv = (struct mcde_display_driver *)&_dev->driver;
	struct mcde_display_device *dev = (struct mcde_display_device *)_dev;

	return drv->remove(dev);
}

static void mcde_drv_shutdown(struct device *_dev)
{
	struct mcde_display_driver *drv = (struct mcde_display_driver *)&_dev->driver;
	struct mcde_display_device *dev = (struct mcde_display_device *)_dev;

	drv->shutdown(dev);
}

static int mcde_drv_suspend(struct device *_dev, pm_message_t state)
{
	struct mcde_display_driver *drv = (struct mcde_display_driver *)&_dev->driver;
	struct mcde_display_device *dev = (struct mcde_display_device *)_dev;

	if (drv->suspend)
		return drv->suspend(dev, state);
	else
#if !defined(CONFIG_HAS_EARLYSUSPEND) && defined(CONFIG_PM)
		return dev->set_power_mode(dev, MCDE_DISPLAY_PM_OFF);
#else
		return 0;
#endif
}

static int mcde_drv_resume(struct device *_dev)
{
	struct mcde_display_driver *drv = (struct mcde_display_driver *)&_dev->driver;
	struct mcde_display_device *dev = (struct mcde_display_device *)_dev;

	if (drv->resume)
		return drv->resume(dev);
	else
#if !defined(CONFIG_HAS_EARLYSUSPEND) && defined(CONFIG_PM)
		return dev->set_power_mode(dev, MCDE_DISPLAY_PM_STANDBY);
#else
		return 0;
#endif
}

/* Public bus API */

int mcde_display_driver_register(struct mcde_display_driver *drv)
{
	drv->driver.probe = mcde_drv_probe;
	if (drv->remove)
		drv->driver.remove = mcde_drv_remove;
	if (drv->shutdown)
		drv->driver.shutdown = mcde_drv_shutdown;
	drv->driver.suspend = mcde_drv_suspend;
	drv->driver.resume = mcde_drv_resume;

	if (registered_driver)
		return -1;
	registered_driver = &drv->driver;
	return 0;
}

void mcde_display_driver_unregister(struct mcde_display_driver *drv)
{
	if (registered_driver == &drv->driver)
		registered_driver = NULL;
}

static void mcde_display_dev_release(struct device *dev)
{
	/* Do nothing */
}

int device_register(struct device *dev)
{
	if (registered_device)
		return -1;
	registered_device = (struct mcde_display_device *)dev;
	return 0;
}

int mcde_display_device_register(struct mcde_display_device *dev)
{
	int ret;

	/* Setup device */
	if (!dev)
		return -EINVAL;
	dev->dev.release = mcde_display_dev_release;

	mcde_display_init_device(dev);

	ret = device_register(&dev->dev);
	return ret;
}

void mcde_display_device_unregister(struct mcde_display_device *dev)
{
	if (registered_device == dev)
		registered_device = NULL;
}



/* Driver init/exit */

int mcde_display_init(void)
{
	return 0;
}

void mcde_display_exit(void)
{
}
