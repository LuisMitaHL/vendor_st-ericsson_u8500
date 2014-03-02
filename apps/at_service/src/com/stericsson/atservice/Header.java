/**
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

package com.stericsson.atservice;

/**
 * Class representing a header. A header describes the response that will be
 * sent to at_core.
 */
public class Header {

    /**
     * The type text used in the header.
     */
    protected static final String HEADER_TYPE = "TYPE";

    /**
     * The size text used in the header.
     */
    protected static final String HEADER_SIZE = "SIZE";

    /**
     * The CME text used in the header.
     */
    protected static final String HEADER_CME = "CME";

    /**
     * The header string.
     */
    protected String header;

    /**
     * Creates a header from a AtCommandResponse.
     *
     * @param response the AtCommandResponse
     */
    public Header(AtCommandResponse response) {
        StringBuilder sb = new StringBuilder();
        sb.append(HEADER_TYPE);
        sb.append(response.getResultCode());
        sb.append(HEADER_SIZE);
        setNumberThreeDigits(sb, response.getResponseText().length());
        sb.append(HEADER_CME);
        setNumberThreeDigits(sb, response.getCmeErrorCode());
        header = sb.toString();

    }

    /**
     * Returns the header as a string.
     *
     * @return a string
     */
    public String toString() {
        return header;
    }

    /**
     * Returns the length of the header
     *
     * @return the length
     */
    public int getHeaderLength() {
        return header.length();
    }

    /**
     * Returns the header as a byte array.
     *
     * @return a byte array
     */
    public byte[] getBytes() {
        return header.getBytes();
    }

    /**
     * Sets a number in a string builder using three digits.
     * <p>
     * Example: nbr = 2 -> 002
     *
     * @param header the header to set the number in
     * @param nbr the number to set
     */
    private void setNumberThreeDigits(StringBuilder header, int nbr) {
        if (nbr < 0 || nbr > 999) { // faulty values - handle as no value
            header.append("000");
            return;
        }

        if (nbr < 10) {
            header.append("00");

        } else if (nbr < 100) {
            header.append("0");
        }

        header.append(nbr);
    }

}
