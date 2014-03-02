/*
 * Copyright (C) ST-Ericsson SA 2009-2010
 * Jonas Aaberg <jonas.aberg@stericsson.com>
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

#ifndef BOOTTIME_H
#define BOOTTIME_H


/**
 * boottime_tag()
 * Add a sample point with a name now. Shall be called before function "name"
 * is executed.
 * @name: Sample point name.
 */
void boottime_tag(char *name);

/**
 * boottime_get_entry()
 *
 * Loads a boottime measure point information.
 * @i: boottime measurement point entry.
 *
 * Returns a boottime entry. NULL, if not existing.
 */
struct boottime_entry *boottime_get_entry(unsigned int i);

/**
 * boottime_idle_get()
 *
 * Returns the amount of time in us that has been spent idling.
 */
unsigned long boottime_idle_get(void);

/**
 * boottime_idle_done()
 *
 * Returns the total time since start in us.
 */
unsigned long boottime_idle_done(void);

/**
 * boottime_idle_add()
 *
 * This function shall be added to all delay() functions.
 * The delay time input to delay() shall be provided to this
 * function as well. It is used to calculate average load
 * during boot.
 * @time: time in us.
 */
void boottime_idle_add(unsigned long time);

#endif
