package com.stericsson.sdk.common.ui.wizards.logic;

/**
 * TODO
 * @author mielcluk
 *
 */
public final class StringTools {

    private StringTools() {
    };

    /**
     * Method that pads (fills) a string from the right with a supplied character until is reaches
     * the desired length. If the input string is longer than the desired length the method will
     * leave the string untocuhed.
     * 
     * @param strToPad
     *            The string which should be padded
     * @param padWith
     *            The character that should be padded to the string
     * @param width
     *            The minimum size that the string will have when the method is exited
     * @return The padded string
     */
    static String padRightChar(String strToPad, char padWith, int width) {
        StringBuffer strBuf = new StringBuffer(strToPad);
        int space = width - strBuf.length();
        while (space-- > 0) {
            strBuf.append(padWith);
        }
        return strBuf.toString();
    }

    /**
     * Method that pads (fills) a string from the right with spaces until is reaches the desired
     * length. If the input string is longer than the desired length the method will leave the
     * string untocuhed.
     * 
     * @param strToPad
     *            The string which should be padded
     * @param width
     *            The minimum size that the string will have when the method is exited
     * @return The padded string
     */
    static String padRight(String strToPad, int width) {
        return padRightChar(strToPad, ' ', width);
    }

    /**
     * Method that pads (fills) a string from the left with a supplied character until is reaches
     * the desired length. If the input string is longer than the desired length the method will
     * leave the string untocuhed.
     * 
     * @param strToPad
     *            The string which should be padded
     * @param padWith
     *            The character that should be padded to the string
     * @param width
     *            The minimum size that the string will have when the method is exited
     * @return The padded string
     */
    static String padLeftChar(String strToPad, char padWith, int width) {
        StringBuffer strBuf = new StringBuffer(strToPad);
        StringBuffer whiteSpace = new StringBuffer(width);
        int space = width - strBuf.length();
        while (space-- > 0) {
            whiteSpace.append(padWith);
        }
        whiteSpace.append(strBuf);
        return whiteSpace.toString();
    }

    /**
     * Method that pads (fills) a string from the left with spaces until is reaches the desired
     * length. If the input string is longer than the desired length the method will leave the
     * string untocuhed.
     * 
     * @param strToPad
     *            The string which should be padded
     * @param width
     *            The minimum size that the string will have when the method is exited
     * @return The padded string
     */
    static String padLeft(String strToPad, int width) {
        return padLeftChar(strToPad, ' ', width);
    }
}
