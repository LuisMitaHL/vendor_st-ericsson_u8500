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
#include <asm/arch/cpu.h>

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


#define BASS_APP_GET_PRODUCT_ID		9

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

int get_productid(u32 *const product_id)
{
	int ret = 0;
	u32 org;
	u32 id = 0;
	TEEC_Operation operation;

	/* UUID for static TA */
	TEEC_UUID uuid = {
		0xBC765EDE,
		0x6724,
		0x11DF,
		{0x8E, 0x12, 0xEC, 0xDB, 0xDF, 0xD7, 0x20, 0x85}
	};

	/*
	 * This is used by some CPU detection code but doesn't currently work
	 * for a9540 CPUs. Therefore if the CPU is a u9540 we don't bother
	 * checking any further.
	 */
	if (cpu_is_u9540v10())
		return -1;

	sec_bridge_init_bridge();

	memset((void *)(&operation), 0, sizeof(TEEC_Operation));

	*product_id = 0;

	operation.memRefs[0].buffer = product_id;
	operation.memRefs[0].size = sizeof(product_id);
	operation.memRefs[0].flags = TEEC_MEM_OUTPUT;

	operation.flags = TEEC_MEMREF_0_USED;

	/* Call ISSW to verify and start modem */
	sec_bridge_call_secure_service(BASS_APP_ISSWAPI_EXECUTE_TA,
				       SEC_ROM_FORCE_CLEAN_MASK,
				       &id,
				       &uuid,
				       NULL,
				       BASS_APP_GET_PRODUCT_ID,
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

	return ret;
}
