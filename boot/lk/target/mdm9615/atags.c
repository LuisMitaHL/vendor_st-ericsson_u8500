/* Copyright (c) 2009-2011, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Code Aurora nor
 *       the names of its contributors may be used to endorse or promote
 *       products derived from this software without specific prior written
 *       permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <reg.h>
#include <debug.h>
#include <smem.h>
#include <stdint.h>

#define SIZE_1M     (1024 * 1024)
#define SIZE_8M     (8 * SIZE_1M)
#define SIZE_15M    (15 * SIZE_1M)
#define SIZE_23M    (23 * SIZE_1M)
#define SIZE_88M    (11 * SIZE_8M)
#define SIZE_128M   (128 * SIZE_1M)

unsigned* target_atag_mem(unsigned* ptr)
{
	struct smem_ram_ptable ram_ptable;
	uint8_t i = 0;

	if (smem_ram_ptable_init(&ram_ptable))
	{
		for (i = 0; i < ram_ptable.len; i++)
		{
			if (ram_ptable.parts[i].category == SDRAM &&
					ram_ptable.parts[i].type == SYS_MEMORY &&
					ram_ptable.parts[i].start == 0x40000000)
			{
				ASSERT(ram_ptable.parts[i].size >= SIZE_15M);

				*ptr++ = 4;
				*ptr++ = 0x54410002;
				*ptr++ = SIZE_15M;
				*ptr++ = ram_ptable.parts[i].start + SIZE_8M;

				*ptr++ = 4;
				*ptr++ = 0x54410002;
				*ptr++ = SIZE_23M;
				*ptr++ = ram_ptable.parts[i].start + SIZE_88M;
			}
		}
	}
	else
	{
		dprintf(CRITICAL, "ERROR: Unable to read RAM partition\n");
		ASSERT(0);
	}

	return ptr;
}

unsigned target_get_max_flash_size(void)
{
	return (SIZE_128M);
}
