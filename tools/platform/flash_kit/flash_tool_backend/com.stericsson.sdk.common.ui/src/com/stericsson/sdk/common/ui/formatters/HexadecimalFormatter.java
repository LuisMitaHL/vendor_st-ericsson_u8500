/**
 * 
 */
package com.stericsson.sdk.common.ui.formatters;

import com.stericsson.sdk.common.HexUtilities;

/**
 * Hexadecimal String formatter for interface IFormatter.
 * 
 * @author mbodan01
 * 
 */
public class HexadecimalFormatter implements IFormatter<String> {

    /**
     * Method which is used to fill string prefix by zero.
     * 
     * @param pString
     *            string which containing hexadecimal number
     * @param pSize
     *            required size of string
     * @return filled string
     */
    private String fillZero(String pString, int pSize) {
        int prefixsize = pSize - pString.length();
        if (prefixsize <= 0) {
            return pString;
        }
        StringBuilder result = new StringBuilder();
        for (int i = 0; i < prefixsize; i++) {
            result.append('0');
        }
        result.append(pString);
        return result.toString();
    }

    /**
     * Method which is used to format hexadecimal string.
     * 
     * @param pString
     *            input non formated hexadecimal value
     * @return formated string of hexadecimal value
     */
    public String format(String pString) {
        StringBuilder result = new StringBuilder(HexUtilities.HEX_PREFIX);
        if (HexUtilities.hasHexPrefix(pString)) {
            result.append(fillZero(pString.substring(2), 4).toLowerCase());
        } else {
            result.append(fillZero(pString, 4).toLowerCase());
        }
        return result.toString();
    }
}
