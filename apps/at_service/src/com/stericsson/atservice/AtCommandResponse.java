/**
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

package com.stericsson.atservice;


/**
 * This class represents the response to a single AT command. Consists of an
 * information response and a result code.
 * <p>
 * It can represent both the final response to an AT command line and the
 * intermediate responses to a single command within a chained AT command line.
 * In both cases a result code is defined. At core must find the final result
 * code to a chained command line by combining the codes from the respective
 * commands.
 */
public class AtCommandResponse {

    /**
     * Result code for answer OK.
     */
    public static final int OK = 0;

    /**
     * Result code for answer ERROR.
     */
    public static final int ERROR = 1;

    /**
     * The CME error result code.
     */
    public static final int CME_ERROR = 2;

    /**
     * A string with CRLF.
     */
    public static final String CRLF = "\r\n";

    /**
     * The result code.
     */
    private int mResultCode;

    /**
     * The CME ERROR code
     */
    private int cmeErrorCode;

    /**
     * The information response.
     * <p>
     * Format example: <CR><LF>+CMD2:3,0,15,"GSM"<CR><LF>
     */
    private String infoResponse;

    /**
     * Constructor for an AtCommandResult with only a result code. There is no
     * information response.
     *
     * @param resultCode the result code, either OK or ERROR or CME_ERROR
     */
    public AtCommandResponse(int resultCode) {
        mResultCode = resultCode;
        cmeErrorCode = 0;
        infoResponse = "";
    }

    /**
     * Constructor for an AtCommandResult with only a result code and a CME
     * error code. There is no information response.
     *
     * @param resultCode the result code, either OK or ERROR or CME_ERROR
     * @param cmeErrorCode the CME error code
     */
    public AtCommandResponse(int resultCode, int cmeErrorCode) {
        this(resultCode);
        this.cmeErrorCode = cmeErrorCode;
    }

    /**
     * Constructor for an AtCommandResult with specified result code and
     * information response.
     *
     * @param resultCode the result code
     * @param response the information response
     */
    public AtCommandResponse(int resultCode, String response) {
        this(resultCode);

        if (response != null) {
            infoResponse = getCrlfFormat(response);
        }
    }

    /**
     * Returns the result code of the AtCommandResponse.
     *
     * @return the result code
     */
    public int getResultCode() {
        return mResultCode;
    }

    /**
     * Returns the CME error code.
     *
     * @return the error code
     */
    public int getCmeErrorCode() {
        return cmeErrorCode;
    }

    /**
     * Returns the information response as a string.
     *
     * @return the information response
     */
    public String getResponseText() {
        return infoResponse;
    }

    /**
     * Returns a value indicating if the AtCommandResponse has a response text.
     *
     * @return true if it has and false otherwise
     */
    public boolean hasResponseText() {
        return infoResponse.length() != 0;
    }

    /**
     * Returns a header for sending the information in this AtCommandResponse to
     * at_core.
     *
     * @return a header describing the information in this AtCommandResponse
     */
    public Header getHeader() {
        return new Header(this);
    }

    /**
     * Add CRLF to the back and front of a string.
     * <p>
     * Example: str -> CRLFstrCRFL
     *
     * @param str the string
     * @return the string with CRLF added
     */
    private String getCrlfFormat(String str) {
        return CRLF + str + CRLF; // slow but few parts so ok
    }

}
