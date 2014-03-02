/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 * @brief   Initiates the uicc state machine.
 * @return  The initial state (The start state).
 *
 */
const ste_sim_state_t * uicc_initiate_state_machine();

/**
 * @brief   Gets the current SIM card type.
 * @return  The SIM card type.
 *
 */
ste_uicc_card_type_t    uicc_get_card_type();

/**
 * @brief   Gets the type of the current selected application on the SIM card.
 * @return  The application type.
 *
 */
ste_sim_app_type_t      uicc_get_app_type();

/**
 * @brief   Gets the ID of the current selected application on the SIM card.
 * @return  The application ID.
 *
 */
int                     uicc_get_app_id();
