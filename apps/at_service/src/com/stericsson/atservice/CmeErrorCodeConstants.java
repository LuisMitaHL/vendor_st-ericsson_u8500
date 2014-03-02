/**
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

package com.stericsson.atservice;

/**
 * The final result code +CME ERROR: <err> is an indication of an error relating
 * to the functionality of the ME. The constants in this class are the possible
 * values of <err> supported by ST_Ericsson.
 */
public final class CmeErrorCodeConstants {

    /**
     * CME error code for phone failure.
     */
    public static final int PHONE_FAILURE = 0;

    /**
     * CME error code for operation not allowed.
     */
    public static final int OPERATION_NOT_ALLOWED = 3;

    /**
     * CME error code for operation not supported.
     */
    public static final int OPERATION_NOT_SUPPORTED = 4;

    /**
     * CME error code for PH-SIM PIN required.
     */
    public static final int PHSIM_PIN_REQUIRED = 5;

    /**
     * CME error code for SIM not inserted.
     */
    public static final int SIM_NOT_INSERTED = 10;

    /**
     * CME error code for SIM PIN required.
     */
    public static final int SIM_PIN_REQUIRED = 11;

    /**
     * CME error code for SIM PUK required.
     */
    public static final int SIM_PUK_REQUIRED = 12;

    /**
     * CME error code for SIM failure.
     */
    public static final int SIM_FAILURE = 13;

    /**
     * CME error code for incorrect password.
     */
    public static final int INCORRECT_PASSWORD = 16;

    /**
     * CME error code for SIM PIN2 required.
     */
    public static final int SIM_PIN2_REQUIRED = 17;

    /**
     * CME error code for SIM PUK2 required.
     */
    public static final int SIM_PUK2_REQUIRED = 18;

    /**
     * CME error code for memory full.
     */
    public static final int MEMORY_FULL = 20;

    /**
     * CME error code for invalid index.
     */
    public static final int INVALID_INDEX = 21;

    /**
     * CME error code for not found.
     */
    public static final int NOT_FOUND = 22;

    /**
     * CME error code for memory failure
     */
    public static final int MEMORY_FAILURE = 23;

    /**
     * CME error code for text string to long.
     */
    public static final int TEXT_STRING_TO_LONG = 24;

    /**
     * CME error code for invalid characters in text string.
     */
    public static final int INVALID_CHARACTERS_IN_TEXT_STRING = 25;

    /**
     * CME error code for dial string to long.
     */
    public static final int DIAL_STRING_TO_LONG = 26;

    /**
     * CME error code for invalid characters in dial string.
     */
    public static final int INVALID_CHARACTERS_IN_DIAL_STRING = 27;

    /**
     * CME error code for no network service.
     */
    public static final int NO_NETWORK_SERVICE = 30;

    /**
     * CME error code for network timeout.
     */
    public static final int NETWORK_TIMEOUT = 31;

    /**
     * CME error code for network personalized PIN required.
     */
    public static final int NETWORK_PERSONALIZED_PIN_REQUIRED = 40;

    /**
     * CME error code for network subset personalized PIN required.
     */
    public static final int NETWORK_SUBSET_PERSONALIZED_PIN_REQUIRED = 42;

    /**
     * CME error code for network provider personalized PIN required.
     */
    public static final int NETWORK_PROVIDER_PERSONALIZED_PIN_REQUIRED = 44;

    /**
     * CME error code for corporate personalized PIN required.
     */
    public static final int CORPORATE_PERSONALIZED_PIN_REQUIRED = 46;

    /**
     * CME error code for corporate personalized PUK required.
     */
    public static final int CORPORATE_PERSONALIZED_PUK_REQUIRED = 47;

    /**
     * The CME error code unknown.
     */
    public static final int UNKNOWN = 100;

    /**
     * The CME error for service option not supported.
     */
    public static final int SERVICE_OPTION_NOT_SUPPORTED = 132;

    /**
     * The CME error for requested service option not subscribed.
     */
    public static final int REQUESTED_SERVICE_OPTION_NOT_SUBSCRIBED = 133;

    /**
     * The CME error for service option temporarily out of order.
     */
    public static final int SERVICE_OPTION_TEMPORARILY_OUT_OF_ORDER = 134;

    /**
     * The CME error for unspecified GPRS error.
     */
    public static final int UNSPECIFIED_GPRS_ERROR = 148;

    /**
     * The CME error for invalid mobile class.
     */
    public static final int INVALID_MOBILE_CLASS = 150;

}
