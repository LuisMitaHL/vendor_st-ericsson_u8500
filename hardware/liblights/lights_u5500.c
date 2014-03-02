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
#include <pthread.h>

#include <hardware/lights.h>

/* This implementation does not use the standard android
 * file system nodes. Instead it uses the nodes exposed by
 * the ab5500-led linux driver that will be mainlined to linux kernel.
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

#define LED_LCD			"/sys/class/leds/lcd-backlight"
#define LED_RED			"/sys/class/leds/red"
#define LED_GREEN		"/sys/class/leds/green"
#define LED_BLUE		"/sys/class/leds/blue"
#define LED_BRIGHTNESS		"/brightness"
#define LED_TRIGGER		"/trigger"
#define LED_TRIG_DELAYON	"/delay_on"
#define LED_TRIG_DELAYOFF	"/delay_off"
#define LED_FADE_AUTO		"/fade_auto"
#define LED_FADE_DELAY		"/fade_delay"


#define LED_FADE_DELAY_HALFSEC	1
#define LED_FADE_DELAY_ONESEC	2
#define LED_FADE_DELAY_TWOSEC	3

static pthread_once_t g_init = PTHREAD_ONCE_INIT;
static pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;

void *leds_enable_disable();

static int write_data(char const *colorpath, char const *opr, unsigned int data)
{
	int dev_fd, ret;
	char val[16] = "\0";
	char filepath[64] = "\0";
	int size;

	sprintf(filepath, "%s%s", colorpath, opr);
	size = sprintf(val, "%d", data);

	dev_fd = open(filepath, O_WRONLY);
	if (dev_fd < 0) {
		ALOGE("Cannot open %s\n", filepath);
		return -ENODEV;
	}

	ALOGD("write_data: %s to %s\n", val, filepath);

	ret = write(dev_fd, val, size);
	if (ret != size)
		ALOGE("Error. Wrote: %d, should have written: %d\n", ret, size);

	close(dev_fd);
	return ret;
}

static int close_lights(struct light_device_t *dev)
{
	if (dev)
		free(dev);
	return 0;
}

/*
 * Check if sw trigger is supported and timer is enabled
 * Returns 0 if enabled otherwise < 0
 */
static int is_trigger_enabled(char const *colorpath)
{
	int ret = -1;
	char filepath[64] = "\0";

	sprintf(filepath, "%s%s", colorpath, LED_TRIGGER);
	ret = access(filepath, F_OK);
	if (ret < 0) {
		ALOGE("Error. LEDS_TRIGGERS not enabled in kernel\n");
		return ret;
	}

	sprintf(filepath, "%s%s", colorpath, LED_TRIG_DELAYON);
	ret = access(filepath, F_OK);
	if (ret < 0) {
		ALOGE("Error. timer trigger not set for LEDS_TRIGGERS\n");
		return ret;
	}

	return ret;
}

/*
 * Check if hw auto fade is supported
 * Returns 0 if enabled otherwise < 0
 */
static int is_hw_fade_enabled(char const *colorpath)
{
	int ret = -1;
	char filepath[64] = "\0";

	sprintf(filepath, "%s%s", colorpath, LED_FADE_AUTO);
	ret = access(filepath, F_OK);
	if (ret < 0) {
		ALOGE("Error. HW AUTO FADE not supported\n");
		return ret;
	}

	sprintf(filepath, "%s%s", colorpath, LED_FADE_DELAY);
	ret = access(filepath, F_OK);
	if (ret < 0) {
		ALOGE("Error. HW AUTO FADE Delay not available\n");
		return ret;
	}

	return ret;
}

static int led_blink(unsigned int r, unsigned int g, unsigned int b,
		unsigned int timeOn, unsigned int timeOff)
{
	int ret = 0;
	int hw_fadeAuto = 1;
	int hw_fade = LED_FADE_DELAY_ONESEC;

	/* Don't quit on first error; Try each LED */

	if (!is_hw_fade_enabled(LED_RED) &&
		!is_hw_fade_enabled(LED_GREEN) &&
		!is_hw_fade_enabled(LED_BLUE))
	{
		if (!timeOn && !timeOff) {
			ret = write_data(LED_RED, LED_FADE_AUTO, 0);
			ret |= write_data(LED_GREEN, LED_FADE_AUTO, 0);
			ret |= write_data(LED_BLUE, LED_FADE_AUTO, 0);
			return ret;
		}

		if ((timeOn > 999) || (timeOff > 999))
			hw_fade = LED_FADE_DELAY_TWOSEC;

		if(r) {
			ret = write_data(LED_RED, LED_FADE_DELAY, hw_fade);
			ret |= write_data(LED_RED, LED_FADE_AUTO, hw_fadeAuto);
		}
		if(g) {
			ret = write_data(LED_GREEN, LED_FADE_DELAY, hw_fade);
			ret |= write_data(LED_GREEN, LED_FADE_AUTO, hw_fadeAuto);
		}
		if(b) {
			ret = write_data(LED_BLUE, LED_FADE_DELAY, hw_fade);
			ret |= write_data(LED_BLUE, LED_FADE_AUTO, hw_fadeAuto);
		}
		return ret;
	}

	/* Fallback to sw trigger */
	if (r &&
		(is_trigger_enabled(LED_RED) ||
		 write_data(LED_RED, LED_TRIG_DELAYON, timeOn) ||
		 write_data(LED_RED, LED_TRIG_DELAYOFF, timeOff)))
		ret = -1;

	if (g &&
		(is_trigger_enabled(LED_GREEN) ||
		 write_data(LED_GREEN, LED_TRIG_DELAYON, timeOn) ||
		 write_data(LED_GREEN, LED_TRIG_DELAYOFF, timeOff)))
		ret |= -1;

	if (b &&
		(is_trigger_enabled(LED_BLUE) ||
		 write_data(LED_BLUE, LED_TRIG_DELAYON, timeOn) ||
		 write_data(LED_BLUE, LED_TRIG_DELAYOFF, timeOff)))
		ret |= -1;

	return ret;
}

static int rgb_to_brightness(struct light_state_t const *state)
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

static int set_light_notifications(struct light_device_t *dev,
		struct light_state_t const *state)
{
	int ret = 0;
	unsigned char r, b, g;
	unsigned int color, timeOn, timeOff, flashMode;

	pthread_mutex_lock(&g_lock);
	timeOn = state->flashOnMS;
	timeOff = state->flashOffMS;
	color = state->color;
	flashMode = state->flashMode;
	pthread_mutex_unlock(&g_lock);

	ALOGD("color: 0x%X timeOn: %d timeOff: %d\n", color, timeOn, timeOff);

	r = (color >> 16) & 0xFF;
	g = (color >> 8) & 0xFF;
	b = (color >> 0) & 0xFF;

	ALOGD("r: 0x%X b: 0x%X g: 0x%X\n", r, b, g);

	/* Don't quit on first error; Try each LED */
	ret = write_data(LED_RED, LED_BRIGHTNESS, r);
	ret |= write_data(LED_GREEN, LED_BRIGHTNESS, b);
	ret |= write_data(LED_BLUE, LED_BRIGHTNESS, g);

	if (flashMode != LIGHT_FLASH_NONE) {
		ret |= led_blink(r, g, b, timeOn, timeOff);
		if (ret < 0)
			return ret;
	}

	return ret;
}

static int set_light_keyboard(struct light_device_t *dev,
		struct light_state_t const *state)
{
	return 0;
}

static int set_light_buttons(struct light_device_t *dev,
		struct light_state_t const *state)
{
	return 0;
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
	int brightness = rgb_to_brightness(state);
	int bytes;

	ALOGV("%s : rgb_to_backlight = %d\n", __func__, brightness);
	pthread_mutex_lock(&g_lock);
	err = write_data(LED_LCD, LED_BRIGHTNESS, brightness);
	pthread_mutex_unlock(&g_lock);

	return err;
}

void init_globals(void)
{
	pthread_mutex_init(&g_lock, NULL);
}

void *leds_enable_disable()
{
	int ledFull = 255;
	/* Set leds once for V2 HW Bug */
	write_data(LED_RED, LED_BRIGHTNESS, ledFull);
	write_data(LED_GREEN, LED_BRIGHTNESS, ledFull);
	write_data(LED_BLUE, LED_BRIGHTNESS, ledFull);
	sleep(1);
	write_data(LED_RED, LED_BRIGHTNESS, 0);
	write_data(LED_GREEN, LED_BRIGHTNESS, 0);
	write_data(LED_BLUE, LED_BRIGHTNESS, 0);
	return NULL;
}

static int open_lights(const struct hw_module_t *module, char const *name,
		struct hw_device_t **device)
{
	pthread_attr_t attr;
	pthread_t thread;
	int ledFull = 255;

	ALOGD("light ID: %s\n", name);

	int (*set_light) (struct light_device_t *dev,
			struct light_state_t const *state);

	if (0 == strcmp(LIGHT_ID_BACKLIGHT, name))
		set_light = set_light_backlight;
	else if (0 == strcmp(LIGHT_ID_KEYBOARD, name))
		set_light = set_light_keyboard;
	else if (0 == strcmp(LIGHT_ID_BUTTONS, name))
		set_light = set_light_buttons;
	else if (0 == strcmp(LIGHT_ID_BATTERY, name))
		set_light = set_light_battery;
	else if (0 == strcmp(LIGHT_ID_NOTIFICATIONS, name))
		set_light = set_light_notifications;
	else if (0 == strcmp(LIGHT_ID_ATTENTION, name))
		set_light = set_light_attention;
	else
		return -EINVAL;

	pthread_once(&g_init, init_globals);

	struct light_device_t *dev = malloc(sizeof(struct light_device_t));
	memset(dev, 0, sizeof(*dev));
	dev->common.tag = HARDWARE_DEVICE_TAG;
	dev->common.version = 0;
	dev->common.module = (struct hw_module_t *) module;
	dev->common.close = (int (*)(struct hw_device_t *)) close_lights;
	dev->set_light = set_light;

	/*
	 * Create a thread to do work around for the hardware bug
	 * work around is needed only once, so thread is created for one instance.
	 */
	if (0 == strcmp(LIGHT_ID_BACKLIGHT, name)) {
		pthread_attr_init (&attr);
		pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);
		pthread_create (&thread, &attr, &leds_enable_disable, NULL);
		pthread_attr_destroy (&attr);
	}
	*device = (struct hw_device_t *) dev;
	return 0;
}

static struct hw_module_methods_t lights_module_methods = {
	.open = open_lights,
};

const struct hw_module_t HAL_MODULE_INFO_SYM = {
	.tag = HARDWARE_MODULE_TAG,
	.version_major = 1,
	.version_minor = 0,
	.id = LIGHTS_HARDWARE_MODULE_ID,
	.name = "Light module",
	.author = "ST-Ericsson",
	.methods = &lights_module_methods,
};
