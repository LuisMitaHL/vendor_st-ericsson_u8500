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
#include <sys/types.h>
#include "abx500.h"
#include "abx500_vibrator.h"
#include "timer.h"
#include <kernel/thread.h>

#define RESONANCE_FREQ_150_HZ 3333

struct vibrator_parameters {
    uint32_t time_ms;
    uint32_t intense;
};

static struct vibrator_parameters vib_data;

int vibrate(void *arg)
{
    const struct vibrator_parameters *data =
        (const struct vibrator_parameters *) arg;
    int reg;
    uint32_t vibrator_time = 0;
    bool toogle = true;

    /* Setup AB8500_REGU_MISC1_REG to enable VTVoutEna */
    reg = ab8500_reg_read(AB8500_REGU_MISC1_REG);
    ab8500_reg_write(AB8500_REGU_MISC1_REG, reg | VTVOUTENA);
    /* disable audio registers reset */
    ab8500_reg_write(AB8500_CTRL3_REG, RESETAUDN | THSDENA);
    /* audio clock enable */
    reg = ab8500_reg_read(AB8500_SYSULPCLK_CTRL1_REG);
    ab8500_reg_write(AB8500_SYSULPCLK_CTRL1_REG, reg | AUDIOCLKENA);
    /* GPIO Direction control - 31, 29, 27 are in direction is
     * output - 31 (Dmic56clk), 29(Dmic34clk), 27(Dmic12clk) */
    ab8500_reg_write(AB8500_GPIO_DIR4_REG,
                     (GPIO31_OUTPUT | GPIO29_OUTPUT | GPIO27_OUTPUT));
    /* Normal operation */
    ab8500_reg_write(AB8500_AUDIO_SOFTWARE_RESET, 0x00);
    /* enable audio supply */
    ab8500_reg_write(AB8500_REGU_VAUDIO_SUPPLY_REG, VAUDIOENA);

    /*
     *  Audio related registers - Vibrator is controled using PWM
     */
    /* power up audio feature */
    ab8500_reg_write(AB8500_AUDIO_POWER_UP,
                     (AUDIOFEATUREPOWERUP | AUDIOANALOGPOWERUP));
    /* Vib1p ball is forced to GndVib voltage, Duty cycle is 0% */
    ab8500_reg_write(AB8500_AUDIO_PWM_GEN_CONF3, data->intense);
    ab8500_reg_write(AB8500_AUDIO_PWM_GEN_CONF2, 0x00);
    /* general vibra control */
    ab8500_reg_write(AB8500_AUDIO_PWM_GEN_CONF1, 0xFF);
    /* EnVib1/2 powered up */
    ab8500_reg_write(AB8500_AUDIO_ANA_CONF4, VIB1POWERUP );

    while (vibrator_time < (1000 * data->time_ms)) {
        udelay(RESONANCE_FREQ_150_HZ);
        vibrator_time += RESONANCE_FREQ_150_HZ;
        if (toogle) {
            ab8500_reg_write(AB8500_AUDIO_PWM_GEN_CONF3, 0x00);
            ab8500_reg_write(AB8500_AUDIO_PWM_GEN_CONF2, data->intense);
            toogle = false;
        } else {
            ab8500_reg_write(AB8500_AUDIO_PWM_GEN_CONF3, data->intense);
            ab8500_reg_write(AB8500_AUDIO_PWM_GEN_CONF2, 0x00);
            toogle = true;
        }
    }

    /* audio clock disable */
    reg = ab8500_reg_read(AB8500_SYSULPCLK_CTRL1_REG);
    ab8500_reg_write(AB8500_SYSULPCLK_CTRL1_REG, reg & ~AUDIOCLKENA);
    /* power down audio feature */
    ab8500_reg_write(AB8500_AUDIO_POWER_UP, 0);
    /* disable audio supply */
    ab8500_reg_write(AB8500_REGU_VAUDIO_SUPPLY_REG, 0);
    /* Setup AB8500_REGU_MISC1_REG to disable VTVoutEna */
    reg = ab8500_reg_read(AB8500_REGU_MISC1_REG);
    ab8500_reg_write(AB8500_REGU_MISC1_REG, reg & ~VTVOUTENA);

    return 0;
}

/*
 * run_vibrator - Runs the vibrator for a specified time
 * @time_ms : The time in ms to run the vibrator
 * @intense : The intensity of the vibrator
 * Returns:  Nothing
 */
void run_vibrator(uint32_t time_ms, uint32_t intense)
{
    dprintf(INFO, "run_vibrator for %d with intense %d\n", time_ms, intense);
    vib_data.time_ms = time_ms;
    vib_data.intense = intense;

    thread_resume(thread_create("VIBRATE", &vibrate, (void *)&vib_data,
        HIGH_PRIORITY, 8 * 1024));
}

