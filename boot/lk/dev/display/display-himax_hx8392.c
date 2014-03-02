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
#include <platform/timer.h>
#include <err.h>
#include "db_gpio.h"

#include "target_config.h"
#include "dbx500-prcmu-regs.h"
#include "mcde_display.h"

#define RESET_DELAY_US		11000
#define RESET_LOW_DELAY_US	20
#define SLEEP_OUT_DELAY_MS	150
#define SLEEP_IN_DELAY_MS	150
#define DELAY_US		11000
#define IO_REGU			"vddi"
#define IO_REGU_MIN		1600000
#define IO_REGU_MAX		3300000

#define DSI_HS_FREQ_HZ		570000000
#define DSI_LP_FREQ_HZ		19200000

#define DCS_CMD_SET_POWER	0xB1
#define DCS_CMD_SET_DISP	0xB2
#define DCS_CMD_SET_VGH		0xB5
#define DCS_CMD_SET_EXTC	0xB9
#define DCS_CMD_SET_MIPI	0xBA
#define DCS_CMD_SET_DSIMO	0xC2
#define DCS_CMD_SET_BLANK_2	0xC7
#define DCS_CMD_SET_PANEL	0xCC
#define DCS_CMD_SET_EQ		0xD4

struct device_info {
	int reset_gpio;
	struct mcde_port port;
	struct regulator *regulator;
};

/* simulated drvdata */
static u32 _drvdata = 0;

static inline void dev_set_drvdata(void *dev, void *data)
{
	_drvdata = (u32)data;
}

static inline u32 dev_get_drvdata(void *dev)
{
	return _drvdata;
}

static inline struct device_info *get_drvdata(struct mcde_display_device *ddev)
{
	return (struct device_info *)dev_get_drvdata(&ddev->dev);
}

static int display_read_deviceid(struct mcde_display_device *dev, u16 *id)
{
	struct mcde_chnl_state *chnl;

	u8  id1, id2, id3;
	int len = 1;
	int ret = 0;
	int readret = 0;

	dev_dbg(&dev->dev, "%s: Read device id of the display\n", __func__);

	/* Acquire MCDE resources */
	chnl = mcde_chnl_get(dev->chnl_id, dev->fifo, dev->port);
	if (IS_ERR(chnl)) {
		ret = PTR_ERR(chnl);
		dev_warn(&dev->dev, "Failed to acquire MCDE channel\n");
		goto out;
	}

	/* plugnplay: use registers DA, DBh and DCh to detect display */
	readret = mcde_dsi_dcs_read(chnl, 0xDA, (u32 *)&id1, &len);
	if (!readret)
		readret = mcde_dsi_dcs_read(chnl, 0xDB, (u32 *)&id2, &len);
	if (!readret)
		readret = mcde_dsi_dcs_read(chnl, 0xDC, (u32 *)&id3, &len);
	if (readret) {
		dev_info(&dev->dev,
			"mcde_dsi_dcs_read failed to read display ID\n");
		goto read_fail;
	}

	*id = (id3 << 8) | id2;
read_fail:
	/* close  MCDE channel */
	mcde_chnl_put(chnl);
out:
	return ret;
}

static int power_on(struct mcde_display_device *dev)
{
	struct device_info *di = get_drvdata(dev);

	dev_dbg(&dev->dev,
		"%s: Reset & power on himax_hx8392 display\n", __func__);

	regulator_enable(di->regulator);
	usleep_range(RESET_DELAY_US, RESET_DELAY_US);
	gpio_set(di->reset_gpio, 1);
	usleep_range(RESET_DELAY_US, RESET_DELAY_US);
	gpio_set(di->reset_gpio, 0);
	udelay(RESET_LOW_DELAY_US);
	gpio_set(di->reset_gpio, 1);
	usleep_range(RESET_DELAY_US, RESET_DELAY_US);

	return 0;
}

static int power_off(struct mcde_display_device *dev)
{
	struct device_info *di = get_drvdata(dev);

	dev_dbg(&dev->dev,
		"%s:Reset & power off himax_hx8392 display\n", __func__);

	gpio_set(di->reset_gpio, 0);
	usleep_range(RESET_DELAY_US, RESET_DELAY_US);
	regulator_disable(di->regulator);

	return 0;
}

static int display_on(struct mcde_display_device *ddev)
{
	int ret;
	u8 wrbuf[MCDE_MAX_DSI_DIRECT_CMD_WRITE] = {0};

	dev_dbg(&ddev->dev, "Display on himax_hx8392 display\n");

	ret = mcde_dsi_dcs_write(ddev->chnl_state, DCS_CMD_EXIT_SLEEP_MODE,
								NULL, 0);
	if (ret)
		return ret;

	msleep(SLEEP_OUT_DELAY_MS);

	/* Set Password */
	wrbuf[0] = 0xFF;
	wrbuf[1] = 0x83;
	wrbuf[2] = 0x92;
	ret = mcde_dsi_dcs_write(ddev->chnl_state, DCS_CMD_SET_EXTC, wrbuf, 3);
	if (ret)
		return ret;

	usleep_range(DELAY_US, DELAY_US);

	wrbuf[0] = 0x0;
	ret = mcde_dsi_dcs_write(ddev->chnl_state,
						DCS_CMD_SET_TEAR_ON, wrbuf, 1);
	if (ret)
		dev_warn(&ddev->dev,
			"%s:Failed to enable synchronized update\n", __func__);

	usleep_range(DELAY_US, DELAY_US);

	wrbuf[0] = 0xA9;
	wrbuf[1] = 0x18;
	ret = mcde_dsi_dcs_write(ddev->chnl_state, DCS_CMD_SET_VGH, wrbuf, 2);
	if (ret)
		return ret;

	usleep_range(DELAY_US, DELAY_US);

	/* Set Power */
	/* VSN_EN, VSP_EN, VGL_EN, VGH_EN, VDDD_N_HZ */
	wrbuf[0] = 0x7C;
	wrbuf[1] = 0x00;
	/* FS1 & AP */
	wrbuf[2] = 0x44;
	/* VGHS = 4 VGLS = 5 */
	wrbuf[3] = 0x45;
	wrbuf[4] = 0x00;
	/* BTP = 16 */
	wrbuf[5] = 0x10;
	/* BTN = 16 */
	wrbuf[6] = 0x10;
	/* VRHP = 18 */
	wrbuf[7] = 0x12;
	/* VRHN = 31 */
	wrbuf[8] = 0x1F;
	/* VRMP = 63 */
	wrbuf[9] = 0x3F;
	/* VRMN = 63 */
	wrbuf[10] = 0x3F;
	/* APF_EN, PCCS = 2 */
	wrbuf[11] = 0x42;
	/* DC86_DIV = 7, XDK0 */
	wrbuf[12] = 0x72;
	ret = mcde_dsi_dcs_write(ddev->chnl_state, DCS_CMD_SET_POWER,
								wrbuf, 13);
	if (ret)
		return ret;

	usleep_range(DELAY_US, DELAY_US);

	/* Set Display related registers */
	/* D = Display source GON, DTE = EN */
	wrbuf[0] = 0x0F;
	wrbuf[1] = 0xC8;
	wrbuf[2] = 0x05;
	wrbuf[3] = 0x0F;
	wrbuf[4] = 0x08;
	wrbuf[5] = 0x84;
	wrbuf[6] = 0x00;
	wrbuf[7] = 0xFF;
	wrbuf[8] = 0x05;
	wrbuf[9] = 0x0F;
	wrbuf[10] = 0x04;
	/* 720 x 1280 */
	wrbuf[11] = 0x20;
	ret = mcde_dsi_dcs_write(ddev->chnl_state, DCS_CMD_SET_DISP,
								wrbuf, 12);
	if (ret)
		return ret;

	usleep_range(DELAY_US, DELAY_US);

	/* 3 datalanes, 12 Mhz low power clock */
	wrbuf[0] = 0x12;
	wrbuf[1] = 0x82;
	ret = mcde_dsi_dcs_write(ddev->chnl_state,
						DCS_CMD_SET_MIPI, wrbuf, 2);
	if (ret)
		return ret;

	usleep_range(DELAY_US, DELAY_US);

	/* Use internal GRAM */
	wrbuf[0] = 0x08;
	ret = mcde_dsi_dcs_write(ddev->chnl_state,
						DCS_CMD_SET_DSIMO, wrbuf, 1);

	usleep_range(DELAY_US, DELAY_US);

	wrbuf[0] = 0x00;
	wrbuf[1] = 0x40;
	ret = mcde_dsi_dcs_write(ddev->chnl_state,
						DCS_CMD_SET_BLANK_2, wrbuf, 2);
	usleep_range(DELAY_US, DELAY_US);

	/* SS_PANEL */
	wrbuf[0] = 0x08;
	ret = mcde_dsi_dcs_write(ddev->chnl_state,
						DCS_CMD_SET_PANEL, wrbuf, 1);

	usleep_range(DELAY_US, DELAY_US);

	wrbuf[0] = 0x0C;
	ret = mcde_dsi_dcs_write(ddev->chnl_state,
						DCS_CMD_SET_EQ, wrbuf, 1);

	usleep_range(DELAY_US, DELAY_US);

	ret = mcde_dsi_dcs_write(ddev->chnl_state,
					DCS_CMD_SET_DISPLAY_ON,	NULL, 0);
	if (ret)
		return ret;

	msleep(SLEEP_OUT_DELAY_MS);

	dev_dbg(&ddev->dev, "Display on himax_hx8392 display EXIT!\n");

	return 0;
}

static int display_off(struct mcde_display_device *ddev)
{
	int ret;

	dev_dbg(&ddev->dev, "Display off himax_hx8392 display\n");

	ret = mcde_dsi_dcs_write(ddev->chnl_state, DCS_CMD_SET_DISPLAY_OFF,
								NULL, 0);
	if (ret)
		return ret;

	msleep(SLEEP_IN_DELAY_MS);

	ret = mcde_dsi_dcs_write(ddev->chnl_state, DCS_CMD_ENTER_SLEEP_MODE,
								NULL, 0);
	msleep(SLEEP_IN_DELAY_MS);

	return ret;
}

static int himax_hx8392_set_power_mode(struct mcde_display_device *ddev,
	enum mcde_display_power_mode power_mode)
{
	int ret = 0;

	dev_dbg(&ddev->dev, "%s:Set Power mode (%d->%d)\n", __func__,
					(u32)ddev->power_mode, (u32)power_mode);

	/* OFF -> STANDBY */
	if (ddev->power_mode == MCDE_DISPLAY_PM_OFF &&
					power_mode != MCDE_DISPLAY_PM_OFF) {
		ret = power_on(ddev);
		if (ret)
			return ret;
		ddev->power_mode = MCDE_DISPLAY_PM_STANDBY;
	}

	/* STANDBY -> ON */
	if (ddev->power_mode == MCDE_DISPLAY_PM_STANDBY &&
					power_mode == MCDE_DISPLAY_PM_ON) {

		ret = display_on(ddev);
		if (ret)
			return ret;
		ddev->power_mode = MCDE_DISPLAY_PM_ON;
	}
	/* ON -> STANDBY */
	else if (ddev->power_mode == MCDE_DISPLAY_PM_ON &&
					power_mode <= MCDE_DISPLAY_PM_STANDBY) {

		ret = display_off(ddev);
		if (ret)
			return ret;
		ddev->power_mode = MCDE_DISPLAY_PM_STANDBY;
	}

	/* STANDBY -> OFF */
	if (ddev->power_mode == MCDE_DISPLAY_PM_STANDBY &&
					power_mode == MCDE_DISPLAY_PM_OFF) {
		ret = power_off(ddev);
		if (ret)
			return ret;
		ddev->power_mode = MCDE_DISPLAY_PM_OFF;
	}

	return mcde_chnl_set_power_mode(ddev->chnl_state, ddev->power_mode);
}

int __devinit himax_hx8392_probe(struct mcde_display_device *dev)
{
	int ret = 0;
	u16 id = 0;
	struct device_info *di;
	struct mcde_port *port;
	struct mcde_display_dsi_platform_data *pdata = dev->dev.platform_data;

	dev_dbg(&ddev->dev, "%s\n", __func__);
	if (pdata == NULL || !pdata->reset_gpio) {
		dev_err(&dev->dev, "Invalid platform data\n");
		return -EINVAL;
	}

	di = kzalloc(sizeof(*di), GFP_KERNEL);
	if (!di)
		return -ENOMEM;

	port = dev->port;
	port->link = pdata->link;
	di->reset_gpio = pdata->reset_gpio;
	di->port.type = MCDE_PORTTYPE_DSI;
	di->port.mode = MCDE_PORTMODE_CMD;
	di->port.pixel_format = MCDE_PORTPIXFMT_DSI_24BPP;
	di->port.sync_src = port->sync_src;
	di->port.frame_trig = port->frame_trig;
	di->port.phy.dsi.num_data_lanes = pdata->num_data_lanes;
	di->port.link = port->link;
	di->port.phy.dsi.host_eot_gen = true;
	di->port.phy.dsi.hs_freq = DSI_HS_FREQ_HZ;
	di->port.phy.dsi.lp_freq = DSI_LP_FREQ_HZ;

	/* Used to calculate necessary pixelfetchwtrmrklevel */
	di->port.refresh_rate = 60;

	gpio_set(di->reset_gpio, 1);
	usleep_range(RESET_DELAY_US, RESET_DELAY_US);
	di->regulator = regulator_get(&dev->dev, IO_REGU);
	if (IS_ERR(di->regulator)) {
		ret = PTR_ERR(di->regulator);
		di->regulator = NULL;
		goto regulator_get_failed;
	}
	ret = regulator_set_voltage(di->regulator, IO_REGU_MIN, IO_REGU_MAX);
	if (WARN_ON(ret))
		goto regulator_voltage_failed;

	dev->set_power_mode = himax_hx8392_set_power_mode;

	dev->port = &di->port;
	dev->native_x_res = 720;
	dev->native_y_res = 1280;
	dev->physical_width = 54;
	dev->physical_height = 95;
	dev_set_drvdata(&dev->dev, di);

	/*
	* When u-boot has display a startup screen.
	* U-boot has turned on display power however the
	* regulator framework does not know about that
	* This is the case here, the display driver has to
	* enable the regulator for the display.
	*/
	if (dev->power_mode != MCDE_DISPLAY_PM_OFF) {
		(void)regulator_enable(di->regulator);
	} else {
		power_on(dev);
		dev->power_mode = MCDE_DISPLAY_PM_STANDBY;
	}

	ret = display_read_deviceid(dev, &id);
	if (ret)
		goto read_id_failed;
	dev_info(&dev->dev,
			"Chimei 990001267 display (ID 0x%04X) probed\n", id);

	return 0;

read_id_failed:
regulator_voltage_failed:
	regulator_put(di->regulator);
regulator_get_failed:
	kfree(di);
	return ret;
}

static int  himax_hx8392_remove(struct mcde_display_device *dev)
{
	struct device_info *di = get_drvdata(dev);

	dev->set_power_mode(dev, MCDE_DISPLAY_PM_OFF);

	regulator_put(di->regulator);

	kfree(di);

	return 0;
}

#if !defined(CONFIG_HAS_EARLYSUSPEND) && defined(CONFIG_PM)
static int himax_hx8392_resume(struct mcde_display_device *ddev)
{
	int ret;

	/* set_power_mode will handle call platform_enable */
	ret = ddev->set_power_mode(ddev, MCDE_DISPLAY_PM_STANDBY);
	if (ret < 0)
		dev_warn(&ddev->dev, "%s:Failed to resume display\n"
			, __func__);
	return ret;
}

static int himax_hx8392_suspend(struct mcde_display_device *ddev, \
							pm_message_t state)
{
	int ret;

	/* set_power_mode will handle call platform_disable */
	ret = ddev->set_power_mode(ddev, MCDE_DISPLAY_PM_OFF);
	if (ret < 0)
		dev_warn(&ddev->dev, "%s:Failed to suspend display\n"
			, __func__);
	return ret;
}
#endif

static struct mcde_display_driver himax_hx8392_driver = {
	.probe	= himax_hx8392_probe,
	.remove = himax_hx8392_remove,
#if !defined(CONFIG_HAS_EARLYSUSPEND) && defined(CONFIG_PM)
	.suspend = himax_hx8392_suspend,
	.resume = himax_hx8392_resume,
#else
	.suspend = NULL,
	.resume = NULL,
#endif
	.driver = {
		.name	= "himax_hx8392",
	},
};

/* Module init */
int mcde_display_himax_hx8392_init(void)
{
	dev_dbg(&ddev->dev, "%s\n", __func__);
	return mcde_display_driver_register(&himax_hx8392_driver);
}

void mcde_display_himax_hx8392_exit(void)
{
	mcde_display_driver_unregister(&himax_hx8392_driver);
}
