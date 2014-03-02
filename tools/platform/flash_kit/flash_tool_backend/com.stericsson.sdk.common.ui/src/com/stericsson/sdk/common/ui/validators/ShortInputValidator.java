package com.stericsson.sdk.common.ui.validators;

import org.eclipse.jface.dialogs.IInputValidator;

import com.stericsson.sdk.common.HexUtilities;

/**
 * This class validates a String. It makes sure that the String is a valid unsigned short in both
 * hex and decimal format
 * 
 */
public class ShortInputValidator implements IInputValidator {

    /**
     * Validates the String.
     * 
     * @param text
     *            The String to validate.
     * @return String Is null for no error, or an error message.
     */
    public String isValid(String text) {

        String input = text;

        if (HexUtilities.hasHexPrefix(text)) {
            input = HexUtilities.removeHexPrefix(text);
        }

        if (!HexUtilities.isHexDigit(input)) {
            return "The input is not a valid hex value.";
        }
        if (input.length() > 4 || input.length() == 0) {
            return "The input is not a valid hex value between 0x0 and 0xFFFF.";
        }

        return null;
    }
}
