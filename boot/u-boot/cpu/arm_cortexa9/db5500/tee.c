/*
 * Code ported from tee in ST-Ericsson Linux kernel.
 *
 * Original Linux authors:
 * Copyright (C) ST-Ericsson SA 2010
 * Authors: Shujuan Chen <shujuan.chen@stericsson.com>
 *          Martin Hovang <martin.xm.hovang@stericsson.com>
 *
 * Ported to U-boot by:
 * Copyright (C) ST-Ericsson SA 2010
 * Author: Mikael Larsson <mikael.xt.larsson@stericsson.com> for ST-Ericsson.
 *
 * License terms: GNU General Public License (GPL) version 2
 */

#include <asm/arch/sec_bridge.h>
#include <asm/arch/tee.h>

/*
 * Defines the number of available memory references in an open session or
 * invoke command operation payload.
 */
#define TEEC_CONFIG_PAYLOAD_REF_COUNT	4

/*
 * Flag constants indicating which of the memory references in an open session
 * or invoke command operation payload (TEEC_Operation) that are used. Type is
 * u32.
 */
#define TEEC_MEMREF_0_USED		0x00000001
#define TEEC_MEMREF_1_USED		0x00000002
#define TEEC_MEMREF_2_USED		0x00000004
#define TEEC_MEMREF_3_USED		0x00000008

/*
 * Flag constants indicating the data transfer direction of memory in
 * TEEC_SharedMemory and TEEC_MemoryReference. TEEC_MEM_INPUT signifies data
 * transfer direction from the client application to the TEE. TEEC_MEM_OUTPUT
 * signifies data transfer direction from the TEE to the client application.
 * Type is u32.
 */
#define TEEC_MEM_INPUT			0x00000001
#define TEEC_MEM_OUTPUT			0x00000002

#define BASS_APP_ISSWAPI_EXECUTE_TA	0x11000001
#define BASS_APP_ISSWAPI_CLOSE_TA	0x11000002

#define BASS_APP_VERIFY_START_MODEM	6

#define SEC_ROM_FORCE_CLEAN_MASK	0x0020

typedef struct {
	u32 timeLow;
	u16 timeMid;
	u16 timeHiAndVersion;
	u8  clockSeqAndNode[8];
} TEEC_UUID;

typedef struct {
	void *buffer;
	size_t size;
	u32 flags;
} TEEC_SharedMemory;

typedef struct {
	TEEC_SharedMemory memRefs[TEEC_CONFIG_PAYLOAD_REF_COUNT];
	u32 flags;
} TEEC_Operation;

int verify_start_modem(struct access_image_descr *access_image_descr)
{
	int ret = 0;
	u32 org;
	u32 id = 0;
	u32 buffer;
	TEEC_SharedMemory shm;
	TEEC_Operation operation;
	/* UUID for static TA */
	TEEC_UUID uuid = {
		0xBC765EDE,
		0x6724,
		0x11DF,
		{0x8E, 0x12, 0xEC, 0xDB, 0xDF, 0xD7, 0x20, 0x85}
	};

	if (access_image_descr == NULL) {
		ret = -1;
		goto out;
	}

	if (access_image_descr->elf_hdr == NULL) {
		ret = -1;
		goto out;
	}

	sec_bridge_init_bridge();

	shm.flags = TEEC_MEM_INPUT;

	shm.buffer = &buffer;
	shm.size = sizeof(buffer);

	*((u32 **)shm.buffer) = &access_image_descr->elf_hdr->e_entry;

	operation.memRefs[0] = shm;
	operation.flags = TEEC_MEMREF_0_USED;

	/* Call ISSW to verify and start modem */
	sec_bridge_call_secure_service(BASS_APP_ISSWAPI_EXECUTE_TA,
				       SEC_ROM_FORCE_CLEAN_MASK,
				       &id,
				       &uuid,
				       NULL,
				       BASS_APP_VERIFY_START_MODEM,
				       &operation,
				       &ret,
				       &org);

	/* Call ISSW to stop TA*/
	sec_bridge_call_secure_service(BASS_APP_ISSWAPI_CLOSE_TA,
				       SEC_ROM_FORCE_CLEAN_MASK,
				       &id,
				       &uuid,
				       NULL,
				       NULL);

out:
	return ret;
}
