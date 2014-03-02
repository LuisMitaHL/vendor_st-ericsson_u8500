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

#ifndef ASSEMBLY

/*
 * Function: mmc_fifo_read()
 *
 * Info: Reads data from an MMC (ARM PL180) FIFO
 *
 * Parameters:
 *	fifo	   - pointer to the first PL180 FIFO register
 *	buf	   - pointer to a read buffer (32-bit aligned)
 *	count	   - number of bytes to be read (32-bit aligned)
 *	status_reg - pointer to the PL180 status register
 *
 * Returns '0' if success and PL180 status on failure.
 *
 */
int mmc_fifo_read(uint32_t *fifo, uint32_t *buf, unsigned int count,
		uint32_t *status_reg);

/*
 * Function: mmc_fifo_write()
 *
 * Info: Writes data to an MMC (ARM PL180) FIFO
 *
 * Parameters:
 *	buf	   - pointer to a write buffer (32-bit aligned)
 *	fifo	   - pointer to the first PL180 FIFO register
 *	count	   - number of bytes to be written (32-bit aligned)
 *	status_reg - pointer to the PL180 status register
 *
 * Returns '0' if success and PL180 status on failure.
 *
 */
int mmc_fifo_write(uint32_t *buf, uint32_t *fifo, unsigned int count,
		uint32_t *status_reg);

#endif /* !ASSEMBLY */
