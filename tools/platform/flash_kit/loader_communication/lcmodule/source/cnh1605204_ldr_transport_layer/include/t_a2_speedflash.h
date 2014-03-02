/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef _INCLUSION_GUARD_T_A2_SPEEDFLASH_H_
#define _INCLUSION_GUARD_T_A2_SPEEDFLASH_H_
/**
 * @addtogroup ldr_communication_serv
 * @{
 *    @addtogroup A2_family
 *    @{
 *
 */

/*******************************************************************************
 * Types, constants
 ******************************************************************************/
#define A2_SPEEDFLASH_REQ_SIZE 2

typedef enum {
    A2_SPEEDFLASH_START,
    A2_SPEEDFLASH_ACTIVE,
    A2_SPEEDFLASH_INACTIVE
} A2_SpeedflashState_t;

typedef enum {
    A2_SPEEDFLASH_READ_REQ,
    A2_SPEEDFLASH_WAIT_READ_REQ,
    A2_SPEEDFLASH_WRITE_BLOCK,
    A2_SPEEDFLASH_WAIT_WRITE_BLOCK
} A2_SpeedflashInboundState_t;

/**
 * A2 speedflash context.
 * Context structures for A2 speedflash protocol.
 */
typedef struct {
    A2_SpeedflashState_t        State;
    boolean                     LastBlock;
    void                       *Outbound_p;
    uint32                      OutboundSize;
    uint8                       Scratch[A2_SPEEDFLASH_REQ_SIZE];
    A2_SpeedflashInboundState_t InboundState;
} A2_SpeedflashContext_t;


/** @} */
/** @} */
#endif // _INCLUSION_GUARD_T_A2_SPEEDFLASH_H_

