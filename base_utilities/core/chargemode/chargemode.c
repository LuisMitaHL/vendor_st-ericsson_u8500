/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Author:     Andrew Lynn <andrew.lynn@stericsson.com>
 * Author:     Peter Enderborg <peter.enderborg@stericsson.com>
 */


#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/reboot.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>
#include <poll.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <pthread.h>
#include <linux/fb.h>
#include <linux/input.h>
#include <linux/reboot.h>

FILE *logFP = NULL;
#define DEBUG_LOGGING 1
#define ERROR(...) do{fprintf((logFP?logFP:stderr), __VA_ARGS__);fflush(logFP?logFP:stderr);}while(0)
#define INFO(...)  do{fprintf((logFP?logFP:stderr), __VA_ARGS__);fflush(logFP?logFP:stderr);}while(0)
#ifdef DEBUG_LOGGING
#define DEBUG(...) do{fprintf((logFP?logFP:stderr), __VA_ARGS__);fflush(logFP?logFP:stderr);}while(0)
#else
#define DEBUG(...)
#endif

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#define FALSE (0)
#define TRUE (!FALSE)

#define POWER_SUPPLY_PATH "/sys/class/power_supply"
#define PLATFORM_PATH "/sys/devices/platform"
#define RESET_REASON_PATH "/sys/devices/soc0/reset_reason"
#define MACHINE_ID_PATH "/sys/devices/soc0/machine"
#define WAKEALARM_PATH "/sys/class/rtc/rtc0/wakealarm"
#define CURTIME_PATH "/sys/class/rtc/rtc0/since_epoch"
#define SUSPEND_PATH "/sys/power/state"
#define BACKLIGHT_PATH "/sys/class/leds/lcd-backlight/brightness"
#define BATTERY_EVENT_CHANGE "/sys/battery/charge_full"
#define DISP_POWER_MODE_PATH "/sys/devices/mcde_disp_sharp_lq043t1.0/power_mode"
#define DEV_PATH_TO_INPUT "/dev/input"
#define PIC_FILE_PATH "/system/usr/share/charge/res/"
#define BOOT_LOGO_PATH "/system/usr/share/charge/res/snda.rle"
#define BATTERY_CAP_PATH "/sys/class/power_supply/ab8500_fg/capacity"
#define INPUT_NUMERIC_ID (sizeof("input") - 1)
#define TURN_ON_STATUS_MAIN_CH_DET 0x10
#define TURN_ON_STATUS_VBUS_DET 0x20
#define TURN_ON_STATUS_CHARGER_DET (TURN_ON_STATUS_MAIN_CH_DET | TURN_ON_STATUS_VBUS_DET)
#define TURN_ON_STATUS_P_ON_KEY1_DET 0x02
#define TURN_ON_STATUS_P_ON_KEY2_DET 0x04
#define TURN_ON_STATUS_P_ON_KEY_DET (TURN_ON_STATUS_P_ON_KEY1_DET | TURN_ON_STATUS_P_ON_KEY2_DET)
#define TURN_ON_STATUS_RTC_DET 0x08
#define UNKNOWN_BAT "Unknown"
#define SUSPEND_TIMEOUT 900000 /*in ms, 15 minutes */
#define SLEEP_COST_EFFECTIVE 3000 /* ms */
#define SYSFS_IO_SIZE 4096
#define PWR_ON_DURATION 2000 /*ms*/

#define	MCDE_DISPLAY_PM_OFF       "0" /* Power off */
#define	MCDE_DISPLAY_PM_STANDBY   "1" /* DCS sleep mode */
#define	MCDE_DISPLAY_PM_ON        "2" /* DCS normal mode, display on */

struct PowerSupplyPaths {
	char* acPresentPath;
	char* usbPresentPath;
	char* turnOnPath;
	char* resetReasonPath;
	char* batTechnologyPath;
	char* batStatusPath;
	char* batEventPath;
	char* machineIdPath;
};

typedef struct PicDescribtion{
    char name[50];
    unsigned int size;
    unsigned short *data;
}PicDescribtion;

typedef struct FrameBuffer {
    int fd;
    unsigned short *bits;
    unsigned int size;
    struct fb_fix_screeninfo fi;
    struct fb_var_screeninfo vi;
}FrameBuffer;

struct DisplayThreadData {
	PicDescribtion	*picdata;
	FrameBuffer		*fbdata;
};

static struct PowerSupplyPaths paths = { NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};

enum {
	POLL_FDS_USB,
	POLL_FDS_AC,
	POLL_FDS_POK,
	POLL_FDS_BAT, /* battery status change node */
	POLL_FDS_NR
};

static const char *reset_reasons[] = {
	"factory-reset",
	"crash",
	"none"
};

static int pwrkey_press_time = 0;

/*
 * read_int_from_file
 * reads an integer from file, returning TRUE if successful
 */
static int read_int_from_file(const char* file, int *i)
{
	FILE *f = fopen(file, "r");
	int count;

	if (!f) {
		ERROR("Could not open '%s': %s\n", file, strerror(errno));
		return FALSE;
	}

	do {
		count = fscanf(f, "%i", i);
	} while (count != 1 && errno == -EINTR);

#ifdef DEBUG_LOGGING
	if (count != 1)
		DEBUG("Could not read from file '%s': %s\n", file, strerror(errno));
#endif

	fclose(f);

	return count == 1;
}

/*
 * read_string_from_file
 * reads a string from file, returning the number of chars successfully read
 */
static int read_string_from_file(const char* path, char* buf, size_t size)
{
	FILE *f = fopen(path, "r");
	char *p;

	if (!f) {
		ERROR("Could not open '%s': %s\n", path, strerror(errno));
		*buf = '\0';
		return 0;
	}

	if (!fgets(buf, size, f)) {
		if (ferror(f))
			ERROR("Cannot read '%s': %s\n", path, strerror(errno));
		else if (feof(f))
			ERROR("Cannot read '%s': End of file encountered\n", path);

		fclose(f);
		*buf = '\0';
		return 0;
	}

	fclose(f);

	p = strchr(buf, '\n');
	if (p)
		*p = '\0';

	return strlen(buf);
}

static int write_cmd(char const *path, char *cmd, int size)
{
	int fd, ret;

	fd = open(path, O_WRONLY);
	if (fd < 0) {
		INFO("Cannot open %s\n", path);
		return -1;
	}

	ret = write(fd, cmd, size);

	close(fd);
	return ret;
}

/*
 * initialise_platform_paths
 * initialises the turn on and power on file paths by searching the sysfs nodes
 * for a match with turn_on_status and then the sub-directories for a match with
 * poweron, returning TRUE if successful
 */
static int initialise_platform_paths()
{
	char   path[PATH_MAX];
	struct dirent* entry;
	int    result = FALSE;
	DIR* dir = opendir(PLATFORM_PATH);

	paths.turnOnPath = NULL;

	if (!dir) {
		ERROR("Could not open %s\n", PLATFORM_PATH);
		return FALSE;
	}

	while ((entry = readdir(dir))) {
		const char* name = entry->d_name;

		// ignore "." and ".."
		if (name[0] == '.' && (name[1] == 0 || (name[1] == '.' && name[2] == 0)))
			continue;
		// Look for a "turn_on_status" file in each subdirectory
		snprintf(path, sizeof(path), "%s/%s/turn_on_status", PLATFORM_PATH, name);

		if (access(path, R_OK) == 0) {
			paths.turnOnPath = strdup(path);

			snprintf(path, sizeof(path), "%s/%s", PLATFORM_PATH, name);
			result = TRUE;
		}
	}

	if (paths.turnOnPath)
		INFO("turnOnPath initialised: %s\n", paths.turnOnPath);
	else
		ERROR("Failed to initialise turnOnPath!\n");
	closedir(dir);
	return result;
}

/*
 * initialise_power_paths
 * initilises the power file paths by searching the sysfs nodes for
 * a match with AC power and/or USB, returning TRUE if successful
 */
static int initialise_power_paths()
{
	char   path[PATH_MAX];
	char   buf[PATH_MAX];
	struct dirent* entry;
	int paths_found = 0;
	int expected_path_found = 4;

	paths.acPresentPath = NULL;
	paths.usbPresentPath = NULL;
	paths.batTechnologyPath = NULL;
	paths.batStatusPath = NULL;
	paths.batEventPath = strdup(BATTERY_EVENT_CHANGE);

	DIR* dir = opendir(POWER_SUPPLY_PATH);
	if (!dir) {
		ERROR("Could not open %s\n", POWER_SUPPLY_PATH);
		return FALSE;
	}

	while ((entry = readdir(dir)) && paths_found < expected_path_found) {
		const char* name = entry->d_name;
		int length;

		// ignore "." and ".."
		if (name[0] == '.' && (name[1] == 0 || (name[1] == '.' && name[2] == 0)))
			continue;

		/* Search for power supply technology */
		snprintf(buf, sizeof(path), "%s/%s/technology", POWER_SUPPLY_PATH, name);
		if (access(buf, R_OK) == 0) {
			paths.batTechnologyPath = strdup(buf);
			paths_found++;
		}

		/* search for uevent for the battery. Note that type is not useful
		   since bttemp and battery is of type battery. Probably a bug... */

		snprintf(buf, sizeof(path), "%s/%s/charge_full", POWER_SUPPLY_PATH, name);
		if (access(buf, R_OK) == 0) {
			snprintf(path, sizeof(path), "%s/%s/uevent", POWER_SUPPLY_PATH,name);
			if (access(path, R_OK) == 0) {
				paths.batStatusPath = strdup(path);
				paths_found++;
			}
		}

		// Look for "type" file in each subdirectory
		snprintf(path, sizeof(path), "%s/%s/type", POWER_SUPPLY_PATH, name);

		length = read_string_from_file(path, buf, sizeof(buf));
		if (length > 0) {
		  DEBUG("buf:%s\n",buf);

			if (strcmp(buf, "Mains") == 0) {
				snprintf(path, sizeof(path), "%s/%s/present", POWER_SUPPLY_PATH, name);
				if (access(path, R_OK) == 0) {
					paths.acPresentPath = strdup(path);
					paths_found++;
				}
			} else if (strcmp(buf, "USB") == 0) {
				snprintf(path, sizeof(path), "%s/%s/present", POWER_SUPPLY_PATH, name);
				if (access(path, R_OK) == 0) {
					paths.usbPresentPath = strdup(path);
					paths_found++;
				}
			}

		}
	}

	if (paths.acPresentPath)
		INFO("acPresentPath initialised: %s\n", paths.acPresentPath);
	else
		ERROR("Failed to initialise acPresentPath!\n");
	if (paths.usbPresentPath)
		INFO("usbPresentPath initialised: %s\n", paths.usbPresentPath);
	else
		ERROR("Failed to initialise usbPresentPath!\n");
	if (paths.batStatusPath)
		INFO("batStatusPath initialised: %s\n", paths.batStatusPath);
	else
		ERROR("Failed to initialise batStatusPath!\n");
	closedir(dir);

	/*
	 * Some boards support only usb charging.
	 * In case of both ac and usb supported for some reason either of ac or
	 * usb path is found and charger for the available path(ac/usb) is
	 * plugged in charge only mode will continue instead of exiting with
	 * error condition.
	 */
	return (paths.acPresentPath || paths.usbPresentPath) && paths.batStatusPath;
}

/* should return current value. The driver is not returning
   right value in some cases. So we use zero */
unsigned char backlight_set(unsigned char level)
{
	FILE	*bl;

	bl = fopen(BACKLIGHT_PATH,"w");
	if (bl == NULL) {
		INFO("Backlight error\n");
		return 0;
	}
	fprintf(bl,"%d\n",level);
	fclose(bl);
	return 0;
}

void charge_full_led(int flag)
{
	/* on the href we dont have any other led than backlight
	 * accessable from userspace */

	if (flag) {
		backlight_set(255);
	} else {
		backlight_set(0);
	}
}

int battery_status(void)
{
	FILE *st;
	int retval = 0;

	st = fopen(paths.batStatusPath,"r");
	if (st == NULL) {
		INFO("Battery error on %s\n",paths.batStatusPath);
	} else {
		char buf[100];
		while(fgets(buf,100,st)) {
			DEBUG("Battery status: %s",buf);
			if (strstr(buf,"POWER_SUPPLY_CAPACITY_LEVEL")) {
				if (strcasestr(buf,"full")) {
					INFO("Charge full\n");
					retval = 1;
				}
			}
		}
		fclose(st);
	}
	return retval;
}
int process_ponkey_event(int fd)
{
	int len;
	unsigned int i;
	struct input_event ev[64];
	len = read(fd,ev,sizeof(ev));
	DEBUG("Power on key:%s(%d)\n",(char*)ev,len);
	for (i = 0; i < len/sizeof(struct input_event); i++) {
		switch (ev[i].type) {
		case EV_SYN:
			break;
		case EV_KEY:
			if (ev[i].value == 1 && ev[i].code == KEY_POWER)
				read_int_from_file(CURTIME_PATH, &pwrkey_press_time);
			else {
                pwrkey_press_time = 0;
                
                return 1;
            }
            
			break;
		default:
			ERROR("Unknown event:%d %d %d\n", ev[i].type,
			      ev[i].code,ev[i].value);
		}
	}
	/* no valid event for wake found. */
	return 0;
}

int is_psu_present(struct pollfd *fds, int id)
{
	int stat;
	char buf[2];

	lseek(fds[id].fd, 0, SEEK_SET);
	stat = read(fds[id].fd, buf, 2);
	DEBUG("psu stat %d read:%s[%d]\n", stat, buf, fds[id].fd);
	if (stat > 0 && buf[0] == '1')
		return 1;
	return 0;
}

int process_psu_event(int fd,struct pollfd *fds)
{
	DEBUG("psu event on %d\n", fd);
	if (!is_psu_present(fds, POLL_FDS_AC) &&
	    !is_psu_present(fds, POLL_FDS_USB)) {
		INFO("Power supply on %d disconnected. Shutting down.\n", fd);
		charge_full_led(1);
		sync();
		reboot(RB_POWER_OFF);
		return 1;
	}
	return 0;
}

/* return the kernel created device node coresponding to the
   input position */
char* find_device(char *where)
{
	DIR *dev;
	struct dirent *input;

	dev = opendir(where);
	while ((input = readdir(dev))) {
		if (input->d_type == DT_REG) {
			if (strncmp(input->d_name,"uevent",6) == 0) {
			  int fd;
			  char rbuf[SYSFS_IO_SIZE];
			  char rep[SYSFS_IO_SIZE];
			  char *token;
			  char *save;

			  snprintf(rbuf,SYSFS_IO_SIZE,"%s/%s",where,input->d_name);
			  fd = open(rbuf,O_RDONLY);
			  read(fd,rep,SYSFS_IO_SIZE);
			  token = strtok_r(rep,"=\n",&save);
			  while (token)	{
				if (strcmp(token,"DEVNAME") == 0) {
					char buf[SYSFS_IO_SIZE];
					token = strtok_r(NULL,"=\n",&save);
					snprintf(buf,SYSFS_IO_SIZE,"/dev/%s",token);
					closedir(dev);
					return strdup(buf);
				}
				token = strtok_r(NULL,"=\n",&save);
			  }
			}
		}
		if ((input->d_type == DT_DIR) && (input->d_name[0] != '.')) {
			char buf[SYSFS_IO_SIZE];
			char *ret;
			snprintf(buf,SYSFS_IO_SIZE,"%s/%s",where,input->d_name);
			ret = find_device(buf);
			if (ret) {
				closedir(dev);
				return ret;
			}
		}
	}
	closedir(dev);
	return NULL;
}

/* find first input device that has a function name matching type.
 * recurse is the number of device tree remaining recurse levels,
 * link is if we follow symbolic links or not.
 */
char* find_input(char *where,char* type,int link,int recurse)
{
	DIR *dev;
	struct dirent *input;

	dev = opendir(where);
	while ((input = readdir(dev))) {
		if (input->d_type == DT_REG) {
			if (strncmp(input->d_name,"uevent",6) == 0) {
				int fd;
				char rbuf[SYSFS_IO_SIZE];
				char rep[SYSFS_IO_SIZE];
				char *token;
				char *save;

				snprintf(rbuf,SYSFS_IO_SIZE,"%s/%s",where,input->d_name);
				fd = open(rbuf,O_RDONLY);
				if (fd == -1) {
					DEBUG("Error %s %s(%d)\n",rbuf,
					      strerror(errno),errno);
					return NULL;
				}

				read(fd,rep,SYSFS_IO_SIZE);
				token = strtok_r(rep,"=\n",&save);
				while (token) {
					if (strcasecmp(token,"NAME") == 0) {
						char buf[SYSFS_IO_SIZE];
						token = strtok_r(NULL,"=\n",&save);
						if (strcasestr(token,type) != 0) {
							snprintf(buf,
								 SYSFS_IO_SIZE,
								 "%s",where);
							DEBUG("result:%s\n",buf);
							closedir(dev);
							return strdup(buf);
						}
					}
					token = strtok_r(NULL,"=\n",&save);
				}
			}
		}
		if (((input->d_type == DT_LNK) && (link > 0) && (recurse > 0)) ||
		    ((input->d_type == DT_DIR) && (input->d_name[0] != '.'))) {
			char buf[SYSFS_IO_SIZE];
			char *ret;

			snprintf(buf,SYSFS_IO_SIZE,"%s/%s",where,input->d_name);
			ret = find_input(buf,type,0,recurse - 1);
			if (ret) {
				DEBUG("res:%s\n",ret);
				closedir(dev);
				return ret;
			}
		}
	}
	closedir(dev);
	return NULL;
}

static int display_thread_alive = FALSE;
struct DisplayThreadData tdata;

static int charge_read_cap(const char *path)
{
    const int SIZE = 16;
    char buf[SIZE];
    int value = 0;
    int count = 0;
    int fd = open(path, O_RDONLY, 0);

    if (fd < 0) {
        ERROR("Could not open '%s'", path);
        return -1;
    }
    
    count = read(fd, buf, SIZE);
    if (count > 0) {
        count = (count < SIZE) ? count : SIZE - 1;
        while (count > 0 && buf[count-1] == '\n') count--;
        buf[count] = '\0';
    } else {
        buf[0] = '\0';
    } 
    close(fd);
    value = atoi(buf);

    return value;
}

static void fb_update(struct FrameBuffer *fb)
{
    DEBUG("fb_update start\n");
    fb->vi.yoffset = 1;
    ioctl(fb->fd, FBIOPUT_VSCREENINFO, &fb->vi);
    fb->vi.yoffset = 0;
    ioctl(fb->fd, FBIOPUT_VSCREENINFO, &fb->vi);
    DEBUG("fb_update end\n");
}

#define from565_r(x) ((((x) >> 11) & 0x1f) * 255 / 31)
#define from565_g(x) ((((x) >> 5) & 0x3f) * 255 / 63)
#define from565_b(x) (((x) & 0x1f) * 255 / 31)

static int rle28880(const char* file_name, char* buffer, unsigned length)
{
	int fd;
	unsigned short in[2], count;
	char r, g, b;
	unsigned i = 0;

	fd = open(file_name, O_RDONLY);
	if (fd < 0) {
		ERROR("Open file %s error.\n", file_name);
		return -1;
	}

	while(read(fd, in, 4) == 4) {
		count = in[0];
		r = from565_r(in[1]);
		g = from565_g(in[1]);
		b = from565_b(in[1]);
		while (count && i < length) {
			buffer[i] = b;
			buffer[i + 1] = g;
			buffer[i + 2] = r;
			buffer[i + 3] = 0xFF;
			i += 4;
			count--;
		}
	}
	DEBUG("%s buffer size: %d.\n", file_name, i);
	close(fd);

	return 0;
}

static void android_memset32(void *_ptr, void *_val, unsigned count)
{
    unsigned short *ptr = _ptr;
    unsigned short *val = _val;

    while(count--)
    {
        *ptr++ = *val++;
    }
}

static void display_one_relpic(PicDescribtion *pd, FrameBuffer *fb)
{
    unsigned int count;//len1, len2, count, max;
    unsigned short *bits, *ptr;//*data, *bits, *ptr;
	DEBUG("enter display_one_relpic\n");
    count = fb->vi.xres * fb->vi.yres * 2;
    bits = fb->bits;
    ptr = pd->data;

    android_memset32(bits, ptr, count);
    //memset(bits, 0xFF, count*2);

	fb_update(fb);
}

void set_screen_status(int on)
{
	if (on) {
		write_cmd(DISP_POWER_MODE_PATH, MCDE_DISPLAY_PM_ON, sizeof(MCDE_DISPLAY_PM_ON));
		backlight_set(20);
		DEBUG("DISP POWER ON.\n");
	} else {
		backlight_set(1);
        write_cmd(DISP_POWER_MODE_PATH, MCDE_DISPLAY_PM_STANDBY, sizeof(MCDE_DISPLAY_PM_STANDBY));        
		DEBUG("DISP POWER OFF.\n");
	}
}

//void thread_exit(struct DisplayThreadData *tdata)
void thread_exit(int sig)
{
	int ret;

	switch (sig) {
	case SIGUSR1:
	if(tdata.picdata->data)
		free(tdata.picdata->data);
	if(tdata.fbdata->bits){
		ret = munmap(tdata.fbdata->bits, tdata.fbdata->fi.smem_len);
		if (ret) {
			ERROR("munmap error.\n");
		}
    }
	close(tdata.fbdata->fd);

	set_screen_status(0);
	display_thread_alive = FALSE;
	DEBUG("display thread exit.\n");
	pthread_exit(NULL);
	break;

	default:
		break;
	}
}

static void * charge_display_thread(void * arg)
{
	struct sigaction act;
    PicDescribtion pd;
    FrameBuffer frmb;
    int ret = 0;//, unit = 0,i;
    int fd, cap = 0;
    int i;
    int charge_full=0;

	arg = NULL;
    memset(&frmb, 0, sizeof(frmb));
    memset(&pd, 0, sizeof(pd));

    /*init the framebuffer*/
    fd = open("/dev/graphics/fb0", O_RDWR);
    if (fd < 0){
        ERROR("fail to open fb0 device\n");
        goto display_error_exit;
    }
    frmb.fd = fd;
    ret = ioctl(fd, FBIOGET_VSCREENINFO, &(frmb.vi));
    if (ret < 0){
        ERROR("fail to ioctl FBIOGET_VSCREENINFO\n");
        goto display_error_exit;
    }
    if (ioctl(fd, FBIOGET_FSCREENINFO, &(frmb.fi)) < 0){
        ERROR("fail to ioctl FBIOGET_FSCREENINFO\n");
        goto display_error_exit;
    }
    frmb.bits = mmap(0, frmb.fi.smem_len, PROT_READ | PROT_WRITE,
                    MAP_SHARED, fd, 0);
    if (frmb.bits == MAP_FAILED){
        frmb.bits = NULL;
        ERROR("fail to mmap framebuffer\n");
        goto display_error_exit;
    }

	tdata.fbdata = &frmb;
	tdata.picdata = &pd;

    /*init the picture resources*/
	act.sa_handler = thread_exit;
	act.sa_flags = 0;
	sigaction(SIGUSR1, &act, NULL);
	display_thread_alive = TRUE;

    /*check the battery status and display the picture*/
	pd.size = frmb.vi.xres * frmb.vi.yres * 4;
	pd.data = (unsigned short *)malloc(pd.size);
	if (pd.data == NULL) {
		ERROR("Picture malloc error.\n");
        goto display_error_exit;
	}

    for (i = 0; i < 15; ++i) {
        cap = charge_read_cap(BATTERY_CAP_PATH);
        if (cap != 0) break;
        sleep(1);
    }
    DEBUG("charge_read_cap cap is %d\n", cap);
    switch (cap) {
        case 0 ... 19:
            sprintf(pd.name, PIC_FILE_PATH "pic_0.rle");
            break;
        case 20 ... 39:
            sprintf(pd.name, PIC_FILE_PATH "pic_1.rle");
            break;
        case 40 ... 59:
            sprintf(pd.name, PIC_FILE_PATH "pic_2.rle");
            break;
        case 60 ... 79:
            sprintf(pd.name, PIC_FILE_PATH "pic_3.rle");
            break;
        case 80 ... 99:
            sprintf(pd.name, PIC_FILE_PATH "pic_4.rle");
            break;
        case 100:
            sprintf(pd.name, PIC_FILE_PATH "pic_5.rle");
            break;
         default:
            break;
    }

	charge_full = battery_status();
	if (charge_full){
        sprintf(pd.name, PIC_FILE_PATH "pic_5.rle");
	}

	DEBUG("file:%s size%d.\n", pd.name, pd.size);
	ret = rle28880(pd.name, (char *)pd.data, pd.size);
	if (ret < 0) {
		ERROR("Read picture data error.\n");
        goto display_error_exit;
	}

	display_one_relpic(&pd, &frmb);

	set_screen_status(1);

	sleep(15);
	DEBUG("display thread normal exit.\n");


display_error_exit:
	if(pd.data)
		free(pd.data);
    if(frmb.bits){
        munmap(frmb.bits, frmb.fi.smem_len);
    }
    if(frmb.fd > 0){
        close(frmb.fd);
    }

	set_screen_status(0);
	display_thread_alive = FALSE;

	return 0;
}

static void display_picture(const char *pic_path)
{
    PicDescribtion pd;
    FrameBuffer frmb;
	int ret;

	if (pic_path == NULL)
		return;

    /*init the picture resources*/
	INFO(" charge_display_thread\n");
    memset(&frmb, 0, sizeof(frmb));
    memset(&pd, 0, sizeof(pd));

    /*init the framebuffer*/
    frmb.fd = open("/dev/graphics/fb0", O_RDWR);
    if (frmb.fd < 0){
        ERROR("fail to open fb0 device\n");
        goto display_error_exit;
    }
    if (ioctl(frmb.fd, FBIOGET_VSCREENINFO, &(frmb.vi)) < 0){
        ERROR("fail to ioctl FBIOGET_VSCREENINFO\n");
        goto display_error_exit;
    }
    if (ioctl(frmb.fd, FBIOGET_FSCREENINFO, &(frmb.fi)) < 0){
        ERROR("fail to ioctl FBIOGET_FSCREENINFO\n");
        goto display_error_exit;
    }
    frmb.bits = mmap(0, frmb.fi.smem_len, PROT_READ | PROT_WRITE,
                    MAP_SHARED, frmb.fd, 0);
    if (frmb.bits == MAP_FAILED){
        frmb.bits = NULL;
        ERROR("fail to mmap framebuffer\n");
        goto display_error_exit;
    }

	pd.size = frmb.vi.xres * frmb.vi.yres * 4;
	pd.data = (unsigned short *)malloc(pd.size);
	if (pd.data == NULL) {
		ERROR("Display picture malloc error.\n");
        goto display_error_exit;
	}

	ret = rle28880(pic_path, (char *)pd.data, pd.size);
	if (ret < 0) {
		ERROR("Read picture data error.\n");
        goto display_error_exit;
	}

    display_one_relpic(&pd, &frmb);

	set_screen_status(1);

display_error_exit:
	if (pd.data)
	    free(pd.data);

    if(frmb.bits){
        munmap(frmb.bits, frmb.fi.smem_len);
    }
    if(frmb.fd > 0){
        close(frmb.fd);
    }
	return;
}

int main(int argc,char **argv)
{
	int alarm_time;
	int now_time;
	int charge_full = 0;
	char *power_on_key_dev = NULL;
	struct pollfd fds[POLL_FDS_NR];
	int ret = EXIT_SUCCESS;
	FILE *suspend_fp;
	int charge_full_led_state = 0;
	int charger_present = FALSE;
	int exit_flag = 0;
	char reset[15];
	char machine[10];
	unsigned int i;
	int turn_on_status;
	int sleep_state = 0; /* 0->Not Initialized, 1->Sleep 2->Dont Sleep */
	char *inputpath;
    pthread_t display_thread;
	logFP = fopen("/tmp/chargemode.log", "wt");
    
	freopen("/dev/kmsg", "a", stderr);

	if (!initialise_platform_paths()) {
		goto exit;
	}

	paths.machineIdPath = MACHINE_ID_PATH;
	read_string_from_file(paths.machineIdPath, machine, sizeof(machine));

	paths.resetReasonPath = RESET_REASON_PATH;
	read_string_from_file(paths.resetReasonPath, reset, sizeof(reset));

	if (!read_int_from_file(paths.turnOnPath, &turn_on_status)) {
		exit(EXIT_FAILURE);
	}
	DEBUG("Startup reason;0x%x\n",turn_on_status);
	if (strcmp(reset, "charging") == 0)
		INFO("Startup requests charge mode\n");
	else if (!(turn_on_status & TURN_ON_STATUS_CHARGER_DET)
			|| (turn_on_status & TURN_ON_STATUS_P_ON_KEY_DET)) {
		INFO("Startup cause not VbusDet - not entering charge mode\n");
		if (argc > 1) {
			DEBUG("Argument to app. Do not end\n");
		} else {
			DEBUG("No argument. Bye.");
		    goto boot_logo;
		}
	}

	for (i = 0; i < ARRAY_SIZE(reset_reasons); i++)
		if (!strcmp(reset, reset_reasons[i])) {
			INFO("Not a regular reboot - not entering charge mode\n");
			exit(EXIT_SUCCESS);
		}

	inputpath = find_input("/sys/class/input","poweron",1,2);
	power_on_key_dev = find_device(inputpath);
	if (!power_on_key_dev) {
		ERROR("Could not find power key device\n");
		exit(EXIT_FAILURE);
	}
	DEBUG("power_on_key_dev:%s\n", power_on_key_dev);

#ifdef ANDROID
	alarm_time = 0;
#else
	if (!read_int_from_file(WAKEALARM_PATH, &alarm_time)) {
		alarm_time = 0;
	}
#endif
	read_int_from_file(CURTIME_PATH, &now_time);
	INFO("Alarm Time:%i %i %d\n", alarm_time,now_time,alarm_time-now_time);

	if (!initialise_power_paths())
		goto exit;

	/* If unknown battery exit */
	if (paths.batTechnologyPath) {
		char techno[sizeof(UNKNOWN_BAT)];
		read_string_from_file(paths.batTechnologyPath, techno, sizeof(techno));
		if (!strncmp(techno, UNKNOWN_BAT, sizeof(techno))) {
			INFO("unknown battery, exit charging mode\n");
			exit(0);
		}
	}

	INFO("Entering charge only mode\n");
	backlight_set(1);
	/* charge mode don't have any interfaces for user input since it is not
	   started so early as the charge mode. This is input shell for
	   test and debugging the charge mode */
#ifdef DEBUG_CONSOLE
	int modres = chmod("/data/busybox/bin/busybox",0777);
	INFO("chmod: %d %d\n",modres,errno);
	int shres = system("/data/busybox/bin/busybox ash /dev/console");
	INFO("Shell: %d %d\n",shres,errno);
#endif
	fds[POLL_FDS_BAT].fd = open(paths.batEventPath,O_RDONLY);
	if (fds[POLL_FDS_BAT].fd == -1) {
		ERROR("Could not open '%s': %s\n", paths.batEventPath, strerror(errno));
		/* we continue without support for the event. */
		fds[POLL_FDS_BAT].events = 0;
	} else {
		fds[POLL_FDS_BAT].events = POLLPRI | POLLERR;
	}

	charge_full = battery_status();
	DEBUG("Battery:%d\n",charge_full);
	if (paths.usbPresentPath) {
		fds[POLL_FDS_USB].fd = open(paths.usbPresentPath, O_RDONLY);
		fds[POLL_FDS_USB].events = POLLERR | POLLPRI;
		if (fds[POLL_FDS_USB].fd == -1) {
			ERROR("Could not open '%s': %s\n", paths.usbPresentPath, strerror(errno));
			ret = EXIT_FAILURE;
			exit_flag = 1;
		}
		charger_present = is_psu_present(fds, POLL_FDS_USB);
	}

	if (paths.acPresentPath) {
		fds[POLL_FDS_AC].fd = open(paths.acPresentPath, O_RDONLY);
		fds[POLL_FDS_AC].events = POLLERR | POLLPRI;
		if (fds[POLL_FDS_AC].fd == -1) {
			ERROR("Could not open '%s': %s\n", paths.acPresentPath, strerror(errno));
			ret = EXIT_FAILURE;
			goto close_usb;
		}
		if (!charger_present)
			charger_present = is_psu_present(fds, POLL_FDS_AC);
	}

	if (!charger_present) {
		INFO("Charger removed - shutting down\n");
		sync();
		reboot(RB_POWER_OFF);
		/* In case reboot() fails... */
		ERROR("reboot() failed: %s\n", strerror(errno));
		exit_flag = 1;
	}

	fds[POLL_FDS_POK].fd = open(power_on_key_dev, O_RDONLY);
	fds[POLL_FDS_POK].events = POLLIN;
	if (fds[POLL_FDS_POK].fd == -1) {
		ERROR("Could not open '%s': %s\n", power_on_key_dev, strerror(errno));
		ret = EXIT_FAILURE;
		exit_flag = 1;
	}

	suspend_fp = fopen(SUSPEND_PATH, "w");
	if (!suspend_fp) {
		ERROR("Could not open '%s': %s\n", SUSPEND_PATH, strerror(errno));
		ERROR("Charging will continue without suspend\n");
	}

    pthread_create( &display_thread, NULL, charge_display_thread, NULL );
    
	INFO("wait for button...\n");
	while (!exit_flag) {
		int alarm_timeout = -1;
		int led_timeout = -1;
		int poll_timeout;
		int pollres;

		poll_timeout = SUSPEND_TIMEOUT;
		if (alarm_time) {
			int cur_time;
			if (read_int_from_file(CURTIME_PATH, &cur_time)) {
				DEBUG("cur_time:%i\n", cur_time);
				if (alarm_time - cur_time <= 0) {
					INFO("Alarm expired - exiting\n");
					if (!strcmp(machine, "DB550000"))
						__reboot(LINUX_REBOOT_MAGIC1, LINUX_REBOOT_MAGIC2,
								LINUX_REBOOT_CMD_RESTART2, "chgonly-exit");
					break;
				}
				if (alarm_time - cur_time < INT_MAX / 1000)
					alarm_timeout = (alarm_time - cur_time) * 1000;
				else
					alarm_timeout = INT_MAX - 1000;
			}
			poll_timeout = SUSPEND_TIMEOUT < alarm_timeout ? SUSPEND_TIMEOUT : alarm_timeout;
		}

		if (charge_full) {
			if (charge_full_led_state == 0) {
				led_timeout = 29000;
				DEBUG("Long sleep\n");
			} else {
				led_timeout = 1000;
				DEBUG("Short sleep\n");
			}
			poll_timeout = poll_timeout < led_timeout ? poll_timeout : led_timeout;
		}


		if (poll_timeout > SLEEP_COST_EFFECTIVE && suspend_fp) {
			DEBUG("Please sleep\n");
			if (sleep_state != 1) {
#if 0 // suspend does not work in ics
			  fputs("mem", suspend_fp);
#endif
				fflush(suspend_fp);
				sleep_state = 1;
			}
		} else {
			DEBUG("Dont sleep!\n");
			if (sleep_state != 2) {
				fputs("on", suspend_fp);
				fflush(suspend_fp);
				sleep_state = 2;
			}
		}

        poll_timeout = PWR_ON_DURATION;
        
		DEBUG("poll_timeout:%d\n",poll_timeout);
		pollres = poll(fds, POLL_FDS_NR, poll_timeout);
		DEBUG("poll:ended %d\n",pollres);
		switch (pollres) {
		case 0: /* timeout */
			DEBUG("Suspend timeout.\n");
            if (pwrkey_press_time > 0) { /*power key long press*/
				fputs("on", suspend_fp);
				fflush(suspend_fp);                    
                exit_flag = 1;
                if (display_thread_alive == TRUE) {
                    DEBUG("kill display thread.\n");
                    pthread_kill(display_thread, SIGUSR1);                
                }
				if (!strcmp(machine, "DB550000"))
					__reboot(LINUX_REBOOT_MAGIC1, LINUX_REBOOT_MAGIC2,
							LINUX_REBOOT_CMD_RESTART2, "chgonly-exit");                
            }
			break;
		case -1: /* error */
			INFO("Poll error: %s(%d)\n",strerror(errno),errno);
			break;
		default: /* IO */
			if (fds[POLL_FDS_POK].revents) {
				int pokr;
				pokr = process_ponkey_event(fds[POLL_FDS_POK].fd);
				if (pokr) { /*power key short press*/
                    if (display_thread_alive == TRUE) {
                        DEBUG("kill display thread.\n");
                        pthread_kill(display_thread, SIGUSR1);
                    }
                    else {
                        pthread_create( &display_thread, NULL, charge_display_thread, NULL );
                        DEBUG("create display thread.\n");             		
				    }
                }
				break;
			}
			if (fds[POLL_FDS_AC].revents) {
				DEBUG("AC Charger status changed");
				exit_flag = process_psu_event(fds[POLL_FDS_AC].fd,fds);
				break;
			}

			if (fds[POLL_FDS_USB].revents) {
				DEBUG("USB Charger status changed");
				exit_flag = process_psu_event(fds[POLL_FDS_AC].fd,fds);
				break;
			}
			if (fds[POLL_FDS_BAT].revents) {
				char buf[100];
				INFO("Battery status changed 0x%x\n",fds[POLL_FDS_BAT].revents);
				read(fds[POLL_FDS_BAT].fd,buf,100);
				INFO("Battery:%s\n",buf);
				charge_full = battery_status();
			}
			break;
		} /* pollres */
	} /* exit_flag */
	//close_bat:
	close(fds[POLL_FDS_POK].fd);
	// close_ac:
	close(fds[POLL_FDS_AC].fd);
close_usb:
	close(fds[POLL_FDS_USB].fd);
	if(display_thread != 0) {
        pthread_join(display_thread, NULL);
    }
    
boot_logo:    
	display_picture(BOOT_LOGO_PATH);    
exit:
    
	exit(ret);
	argv = NULL; // bogus
	if (logFP) fclose(logFP);
	logFP = NULL;
}
