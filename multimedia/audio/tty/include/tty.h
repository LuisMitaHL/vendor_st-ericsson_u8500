/***********************************************************************
 *    $Copyright 2010 ST-Ericsson $
 ***********************************************************************
 *
 *    The copyright to the document(s) herein is the property of
 *    ST-Ericsson AB, Sweden.
 *  
 *    The document(s) may be used and/or copied only with the
 *    written permission from ST-Ericsson AB
 *    or in accordance with the terms and conditions stipulated
 *    in the agreement or contract under which the document(s)
 *    have been supplied.
 ***********************************************************************
 *
 *    File:        tty.h
 *    Author:      ST-Ericsson
 *    Created:     2010-07-02
 *    Description: Public interface file for TTY 
 *
 ***********************************************************************/

#ifndef INCLUSION_GUARD_TTY_H
#define INCLUSION_GUARD_TTY_H


/***********************************************************************
 *
 * Types 
 *
 ***********************************************************************/
typedef struct TTY_TxState_s TTY_TxState_t;
typedef struct TTY_RxState_s TTY_RxState_t;

/**
 * The type TTY_TxSyncState_t holds state info on the synchronization of the TTY transmitter.
 * A parameter of this type can be passed as an input parameter to the TTY receiver.
 * The field 'InFigureMode' can be either 0 or 1 and tells whether the TTY transmitter is in 'figure mode' or not.
 */
/*
typedef struct
{
} TTY_TxSyncState_t;
*/
/**
 * The type TTY_RxSyncState_t holds state info on the synchronization of the TTY receiver.
 * A parameter of this type can be passed as an input parameter to the TTY transmitter.
 * The fields 'InFigureMode', 'CtmFromFarEndDetected' and 'EnquiryFromFarEndDetected' can all be either 0 or 1.
 * The value of the fields tells if the TTY receiver is in 'figure mode', if ctm from far end is detected,
 * and if enquiry from far end is detected, respectively
 */
typedef struct
{
  unsigned short  CtmFromFarEndDetected[2];   //2 == TTY_LENGTH_PCM_FRAME/TTY_LENGTH_TONE_VEC
  signed short  EnquiryFromFarEndDetected[2]; //2 == TTY_LENGTH_PCM_FRAME/TTY_LENGTH_TONE_VEC 
} TTY_RxSyncState_t;


/**
 * The type TTY_Result_t is used as return type, for the TTY execute function.
 * The return value TTY_RESULT_NO_HANDLE is used if the TTY algorithm (RX or TX) is not 'allocated',
 * that is a NULL pointer is passed to the function. Otherwise, TTY_RESULT_OK is used.
 *
 */
typedef enum
{
  TTY_RESULT_OK  = 0x0,
  TTY_RESULT_NO_HANDLE
} TTY_Result_t;

/***********************************************************************
 *
 * Functions 
 *
 ***********************************************************************/
/**
 * Function which dynamically allocate an instance of TTY_TxState_t
 *  @param None
 *  @return a pointer to the dynamically memory, NULL if the allocation failed
 */
TTY_TxState_t* TTY_TX_Construct( void );

/**
 * Function TTY_TX_Init
 * This function initiate the TTY_TxState_t with initial default values
 * @param pointer to TTY_TxState_t
 * @return none
 */
void TTY_TX_Init( TTY_TxState_t     *const TxState_p);
                  



/**
 * Function TTY_TX_Execute
 * This function execute the TTY on TX side
 *
 * @param  TxState_p: A pointer to a TTY Tx structure containing
 * static constants and variables used by the TTY transmitter.
 *
 * @param TxSyncState_p: A pointer to a TTY Tx structure containing static constants
 * and variables used by the TTY transmitter.
 *
 * @param PcmIn_p:	Pointer to a buffer which contains 80 PCM samples (baudot tone input)
 *
 * @param PcmOut_p: Pointer to a buffer which contains 80 PCM samples (CTM tone output)
 *
 * @return TTY_RESULT_NO_HANDLE, if TxState_p is NULL.
 *         TTY_RESULT_OK, otherwise
 */
TTY_Result_t TTY_TX_Execute(       TTY_TxState_t     *const TxState_p,
                                   const TTY_RxSyncState_t* const TTY_Rx_SyncState,
                             const short             *const PcmIn_p,
                                   short             *const PcmOut_p);

/**
 *  Function TTY_TX_Destruct
 * This routine frees memory used for the TTY transmitter state that was allocated by TTY_TX_Construct()
 * The behavior and constraint  are the same as for the standard c function free()
 * @param state	A pointer to a TTY Tx structure.
 * @return none
 */
void TTY_TX_Destruct( TTY_TxState_t* state );



/**
 * Function TTY_RX_Construct
 * This routine allocates memory for the TTY receiver state
 *
 * @param none
 *
 * @return TTY_RxState_t* pointer to the allocated memory for the RX state
 *
 */
TTY_RxState_t* TTY_RX_Construct( void );

/**
 * Function TTY_RX_Init
 * This routine initiates an instance of the TTY receiver and the Rx synchronization state.
 * The TTY receiver instance converts encoded CTM tones to baudot tones to be passed on in the audio chain.
 *
 * @param   A pointer to a TTY Rx structure
 *
 * @param A pointer to a Rx Sync state
 *
 */
void TTY_RX_Init( TTY_RxState_t     *const RxState_p,
                  TTY_RxSyncState_t *const RxSyncState_p);

/**
 * Function TTY_RX_Execute
 * This is the main receiver function of the TTY module.
 * This function takes speech-decoded PCM samples containing CTM tones and converts them to encoded baudot tones.
 *
 * @param RxState_p A pointer to a TTY Rx structure containing static constants and
 * variables used by the TTY receiver
 *
 * @param RxSyncState_p  Pointer to a structure containing state info of the Rx part of TTY
 *
 * @param PcmIn_p  Pointer to a buffer containing 80 PCM samples (CTM tone input)
 *
 * @param PcmOut_p Pointer to a buffer containing 80 PCM samples (baudot tone output)
 *
 * @return TTY_RESULT_NO_HANDLE, if *TxState_pp is NULL. TTY_RESULT_OK, otherwise.
 */

TTY_Result_t TTY_RX_Execute(       TTY_RxState_t     *const RxState_p,
                                   TTY_RxSyncState_t *const RxSyncState_p,
                             const short             *const PcmIn_p,
                                   short             *const PcmOut_p);

/**
 * TTY_RX_Destruct
 * This routine frees memory used for the TTY receiver state that was allocated by TTY_RX_Construct()
 * @param   state A pointer to a TTY Rx structure.
 * @return none
 *
 */
void TTY_RX_Destruct( TTY_RxState_t* state );


/**
 * TTY_Share_From_RX_To_TX
 *  This function share/ the RX sync state from RX to TX and is called
 *  before TTY_TX_Execute is called
 *  @param TTY_Tx_state pointer to TX state
 *
 *  @param TTY_Rx_SyncState RX sync state which contain the RX state needed to be shared to TX
 *
 *  @return none
 */
void TTY_Share_From_RX_To_TX(       TTY_TxState_t*           TTY_Tx_state,
                              const TTY_RxSyncState_t* const TTY_Rx_SyncState);


/**
 * TTY_Share_From_TX_To_RX
 * This function share/transfer the TX sync state to RX state and is called before
 * TTY_RX_Execute
 *
 * @param TTY_RX_state Rx state
 *
 * @param TTY_TX_SyncState TX syncstate
 */
void TTY_Share_From_TX_To_RX(void );

#endif /* #ifndef INCLUSION_GUARD_TTY_H */
