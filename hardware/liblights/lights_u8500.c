/*
 * Copyright (C) 2010 ST-Ericsson AB
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "lights"

#include <cutils/log.h>

#include <errno.h>
#include <fcntl.h>

#include <hardware/lights.h>

/* This implementation does not use the standard android
 * file system nodes. Instead it uses the nodes exposed by
 * the lp5521 linux driver that will be mainlined to linux kernel.
 *
 * The NotificationManager in Android states:
 *
 * //Don't start allowing notifications until the setup wizard has run once.
 * // After that, including subsequent boots, init with notifications turned on.
 * // This works on the first boot because the setup wizard will toggle this
 * // flag at least once and we'll go back to 0 after that.
 *
 * If the setup wizard is never run, a workaround to test the LEDs is to
 * disable this check in the NotificationManager.
 *
 */

#ifndef U9540_USE_REMAPPED_LED_PATHS
char const *const PATH_ENGINE1_MODE = "/sys/class/leds/lp5521_pri:channel0/device/engine1_mode";
char const *const PATH_ENGINE2_MODE = "/sys/class/leds/lp5521_pri:channel0/device/engine2_mode";
char const *const PATH_ENGINE3_MODE = "/sys/class/leds/lp5521_pri:channel0/device/engine3_mode";
char const *const PATH_ENGINE1_LOAD = "/sys/class/leds/lp5521_pri:channel0/device/engine1_load";
char const *const PATH_ENGINE2_LOAD = "/sys/class/leds/lp5521_pri:channel0/device/engine2_load";
char const *const PATH_ENGINE3_LOAD = "/sys/class/leds/lp5521_pri:channel0/device/engine3_load";
char const *const PATH_COLOR_CH1 = "/sys/class/leds/lp5521_pri:channel0/brightness";
char const *const PATH_COLOR_CH2 = "/sys/class/leds/lp5521_pri:channel1/brightness";
char const *const PATH_COLOR_CH3 = "/sys/class/leds/lp5521_pri:channel2/brightness";
#else
/*
 * The U9540 tablet and smartphone UIB boards contain an error which means that
 * the connection between the LP5521 driver and the LED device are incorrectly
 * mapped. (Red is connected to blue, green is connected to red and blue is
 * connected to green.)
 *
 * The following mapping of driver engine paths corrects these errors.
 */
char const *const PATH_ENGINE1_MODE = "/sys/class/leds/lp5521_pri:channel0/device/engine2_mode";
char const *const PATH_ENGINE2_MODE = "/sys/class/leds/lp5521_pri:channel0/device/engine3_mode";
char const *const PATH_ENGINE3_MODE = "/sys/class/leds/lp5521_pri:channel0/device/engine1_mode";
char const *const PATH_ENGINE1_LOAD = "/sys/class/leds/lp5521_pri:channel0/device/engine2_load";
char const *const PATH_ENGINE2_LOAD = "/sys/class/leds/lp5521_pri:channel0/device/engine3_load";
char const *const PATH_ENGINE3_LOAD = "/sys/class/leds/lp5521_pri:channel0/device/engine1_load";
char const *const PATH_COLOR_CH1 = "/sys/class/leds/lp5521_pri:channel1/brightness";
char const *const PATH_COLOR_CH2 = "/sys/class/leds/lp5521_pri:channel2/brightness";
char const *const PATH_COLOR_CH3 = "/sys/class/leds/lp5521_pri:channel0/brightness";
#endif


static pthread_once_t g_init = PTHREAD_ONCE_INIT;
static pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;

/* LCD backlight related variables */
static int g_backlight = 255;
char const *const LCD_FILE =  "/sys/class/leds/lcd-backlight/brightness";
char const *const BUTTON_FILE =  "/sys/class/leds/button-backlight/brightness";

static int write_cmd(char const *path, char *cmd, int size)
{
	int fd, ret;

	fd = open(path, O_WRONLY);
	if (fd < 0) {
		ALOGE("Cannot open %s\n", path);
		return -ENODEV;
	}

	ALOGD("write_cmd: %s to %s\n", cmd, path);
	ret = write(fd, cmd, size);
	if (ret != size)
		ALOGE("Error. Wrote: %d, should have written: %d\n", ret, size);

	close(fd);
	return ret;
}

static int close_lights(struct light_device_t *dev)
{
	if (dev)
		free(dev);
	return 0;
}

static int is_lit(struct light_state_t const* state)
{
    return state->color & 0x00ffffff;
}

static int rgb_to_brightness(struct light_state_t const* state)
{
	int color = state->color & 0x00ffffff;

	/*
	 * ref: hardware/libhardware/include/hardware/lights.h
	 * unsigned char brightness = ((77*((color>>16)&0x00ff))
	 *	+ (150*((color>>8)&0x00ff)) + (29*(color&0x00ff))) >> 8;
	 */
	return ((77*((color>>16)&0x00ff)) +
		(150*((color>>8)&0x00ff)) + (29*(color&0x00ff))) >> 8;
}

static int calc_sequence(int prescale, int steptime, int sign,
		int increment)
{
	int reg = 0;
	reg |= (prescale & 0x1) << 14;
	reg |= (steptime & 0x3f) << 8;
	reg |= (sign & 0x1) << 7;
	reg |= (increment & 0x7f);
	return reg;
}

static void set_sequence(char *color, unsigned int val, int prescaleon,
		int prescaleoff, int steptimeon, int steptimeoff)
{
	int regs[4] = { 0 };
	char cmd[17];		/* size of load command to kernel interface */
	int size;

	ALOGD("%s. color: %s val: 0x%X prescaleon: %d prescaleoff: %d steptimeon: %d steptimeoff: %d \n",
		 __FUNCTION__, color, val, prescaleon, prescaleoff, steptimeon, steptimeoff);


	/* 1 is the smallest steptime for the lp5521 led sensor */
	regs[0] = calc_sequence(0, 1, 0, val >> 1);
	regs[1] = calc_sequence(prescaleon, steptimeon, 0, 0x00);
	/* 1 is the smallest steptime for the lp5521 led sensor */
	regs[2] = calc_sequence(0, 1, 1, val >> 1);
	regs[3] = calc_sequence(prescaleoff, steptimeoff, 0, 0x00);

	ALOGD("regs[0]: 0x%.4x regs[1]: 0x%.4x regs[2]: 0x%.4x regs[3]: 0x%.4x\n",
		regs[0], regs[1], regs[2], regs[3]);

	/* Convert regs to string as that is required by the kernel interface */
	size =
		sprintf(cmd, "%.4x%.4x%.4x%.4x", regs[0], regs[1], regs[2],
				regs[3]);
	if (size > 16)
		ALOGE("sprintf wrote %d bytes too much\n", size - 16);
	else {
		if(!strcmp(color,"r"))
			write_cmd(PATH_ENGINE1_LOAD, cmd, strlen(cmd) + 1);
		else if(!strcmp(color,"g"))
			write_cmd(PATH_ENGINE2_LOAD, cmd, strlen(cmd) + 1);
		else if(!strcmp(color,"b"))
			write_cmd(PATH_ENGINE3_LOAD, cmd, strlen(cmd) + 1);
	}
}

static int set_light_notifications(struct light_device_t *dev,
		struct light_state_t const *state)
{
	unsigned char r, b, g;
	unsigned int color, steptimeon, steptimeoff, timeon, timeoff,
				 prescaleon, prescaleoff, flashMode;
	int size;
	char cmd[9];		/* size of color command to kernel interface */

	prescaleon = 0;
	prescaleoff = 0;

	pthread_mutex_lock(&g_lock);
	timeon = state->flashOnMS;
	timeoff = state->flashOffMS;
	color = state->color;
	flashMode = state->flashMode;
	pthread_mutex_unlock(&g_lock);

	ALOGD("color: 0x%X timeon: %d timeoff: %d \n", color, timeon, timeoff);

	r = (color >> 16) & 0xFF;
	g = (color >> 8) & 0xFF;
	b = (color >> 0) & 0xFF;

	ALOGD("r: 0x%X b: 0x%X g: 0x%X \n", r, b, g);

	if (flashMode == LIGHT_FLASH_NONE) {
		write_cmd(PATH_ENGINE1_MODE, "disabled", 9);
		size = sprintf(cmd, "%d\n", r);
		if (size > 2)
			ALOGE("sprintf wrote %d bytes too much\n", size - 2);
		else
			write_cmd(PATH_COLOR_CH1, cmd, size);
		write_cmd(PATH_ENGINE2_MODE, "disabled", 9);
		size = sprintf(cmd, "%d\n", g);
		if (size > 2)
			ALOGE("sprintf wrote %d bytes too much\n", size - 2);
		else
			write_cmd(PATH_COLOR_CH2, cmd, size);
		write_cmd(PATH_ENGINE3_MODE, "disabled", 9);
		size = sprintf(cmd, "%d\n", b);
		if (size > 2)
			ALOGE("sprintf wrote %d bytes too much\n", size - 2);
		else
			write_cmd(PATH_COLOR_CH3, cmd, size);
	} else {

		/* max step time for lp5521 led sensor is 1 second */
		if (timeon > 1000)
			ALOGE("On time = %d. Hardware max is 1s. Time set to 1s\n",
					timeon);
		if (timeoff > 1000)
			ALOGE("Off time = %d. Hardware max is 1s. Time set to 1s\n",
					timeoff);

		/* lp5521 led sensor can either run at 15.6ms or 0.49ms cycle time */
		if (timeon > 15.6)
			prescaleon = 1;
		if (timeoff > 15.6)
			prescaleoff = 1;

		steptimeon = timeon / (0.49 + prescaleon * (15.6 - 0.49));
		steptimeoff = timeoff / (0.49 + prescaleoff * (15.6 - 0.49));

		/*
		 * Max number of steps for lp5521 led sensor is 63, but the calculation
		 * above can give the value 64. Hence this "correction".
		 */
		if (steptimeoff > 63)
			steptimeoff = 63;
		if (steptimeon > 63)
			steptimeon = 63;

		if(r) {
			write_cmd(PATH_ENGINE1_MODE, "load", 5);
			set_sequence("r", r, prescaleon, prescaleoff, steptimeon,
				steptimeoff);
			write_cmd(PATH_ENGINE1_MODE, "run", 4);
		}
		if(g) {
			write_cmd(PATH_ENGINE2_MODE, "load", 5);
			set_sequence("g", g, prescaleon, prescaleoff, steptimeon,
				steptimeoff);
			write_cmd(PATH_ENGINE2_MODE, "run", 4);
		}
		if(b) {
			write_cmd(PATH_ENGINE3_MODE, "load", 5);
			set_sequence("b", b, prescaleon, prescaleoff, steptimeon,
				steptimeoff);
			write_cmd(PATH_ENGINE3_MODE, "run", 4);
		}
	}
	return 0;
}

static int set_light_keyboard(struct light_device_t *dev,
		struct light_state_t const *state)
{
	return 0;
}

static int set_light_buttons(struct light_device_t *dev,
		struct light_state_t const *state)
{
    int err = 0;
    char buffer[20];
	//int on = is_lit(state);
	pthread_mutex_lock(&g_lock);

    int brightness = is_lit(state)?180:0;
    int bytes = sprintf(buffer, "%d\n", brightness);	
    err = write_cmd(BUTTON_FILE, buffer, bytes); 

    pthread_mutex_unlock(&g_lock);
    return err;
}

static int set_light_battery(struct light_device_t *dev,
		struct light_state_t const *state)
{
	return 0;
}

static int set_light_attention(struct light_device_t *dev,
		struct light_state_t const *state)
{
	return 0;
}

static int set_light_backlight(struct light_device_t *dev,
		struct light_state_t const *state)
{
	int err = 0;
	char buffer[20];
	int brightness = rgb_to_brightness(state);

	ALOGV("%s : rgb_to_backlight = %d\n", __func__, brightness);
	pthread_mutex_lock(&g_lock);
	g_backlight = brightness;
	int bytes = sprintf(buffer, "%d\n", brightness);
	err = write_cmd(LCD_FILE, buffer, bytes);
	pthread_mutex_unlock(&g_lock);
	return err;
}

void init_globals(void)
{
	pthread_mutex_init(&g_lock, NULL);
}

static int open_lights(const struct hw_module_t *module, char const *name,
		struct hw_device_t **device)
{

	ALOGD("light ID: %s\n", name);

	int (*set_light) (struct light_device_t * dev,
			struct light_state_t const *state);

	if (0 == strcmp(LIGHT_ID_BACKLIGHT, name)) {
		set_light = set_light_backlight;
	} else if (0 == strcmp(LIGHT_ID_KEYBOARD, name)) {
		set_light = set_light_keyboard;
	} else if (0 == strcmp(LIGHT_ID_BUTTONS, name)) {
		set_light = set_light_buttons;
	} else if (0 == strcmp(LIGHT_ID_BATTERY, name)) {
		set_light = set_light_battery;
	} else if (0 == strcmp(LIGHT_ID_NOTIFICATIONS, name)) {
		set_light = set_light_notifications;
	} else if (0 == strcmp(LIGHT_ID_ATTENTION, name)) {
		set_light = set_light_attention;
	} else {
		return -EINVAL;
	}

	pthread_once(&g_init, init_globals);

	struct light_device_t *dev = malloc(sizeof(struct light_device_t));
	memset(dev, 0, sizeof(*dev));
	dev->common.tag = HARDWARE_DEVICE_TAG;
	dev->common.version = 0;
	dev->common.module = (struct hw_module_t *) module;
	dev->common.close = (int (*)(struct hw_device_t *)) close_lights;
	dev->set_light = set_light;

	*device = (struct hw_device_t *) dev;
	return 0;
}

static struct hw_module_methods_t lights_module_methods = {
	.open = open_lights,
};

struct hw_module_t HAL_MODULE_INFO_SYM = {
	.tag = HARDWARE_MODULE_TAG,
	.version_major = 1,
	.version_minor = 0,
	.id = LIGHTS_HARDWARE_MODULE_ID,
	.name = "Light module",
	.author = "ST-Ericsson",
	.methods = &lights_module_methods,
};
