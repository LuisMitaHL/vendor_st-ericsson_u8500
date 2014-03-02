package com.stericsson.sdk.common.ui.validators;

import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

import com.stericsson.sdk.common.HexUtilities;

/**
 * This class validates a String which aren't in a selected Collection.
 * 
 * @author mbodan01
 */
public class ShortInputContainValidator extends ShortInputValidator {

    private Collection<String> list;

    /**
     * Basic constructor
     */
    public ShortInputContainValidator() {
        list = new ArrayList<String>(0);
    }

    /**
     * @param plist
     *            Collection of String which describe contain validated component.
     */
    public ShortInputContainValidator(Collection<String> plist) {
        list = plist;
    }

    /**
     * @param plist
     *            Collection of String which describe contain validated component.
     */
    public void setList(Collection<String> plist) {
        list = plist;
    }

    /**
     * @param plist
     *            Collection of String which contain hexadecimal numbers.
     * @return collection of short numbers
     */
    private List<Short> parseCollection(Collection<String> plist) {
        List<Short> result = new ArrayList<Short>();
        for (String hex : plist) {
            try {
                result.add(new Short((short) HexUtilities.parseHexDigit(hex)));
            } catch (NumberFormatException e) {
                continue;
            }
        }
        return result;
    }

    /**
     * @param text
     *            The String to validate.
     * @return String Is null for no error, or an error message.
     */
    @Override
    public String isValid(String text) {
        if (text.compareTo("") != 0) {
            try {
                Short number = new Short((short) HexUtilities.parseHexDigit(text));
                List<Short> hexs = parseCollection(list);
                if (hexs.contains(number)) {
                    return "The input is alredy in the list.";
                }
            } catch (NumberFormatException e) {
                return "The input isn't hexadecimal number.";
            }
        }
        return super.isValid(text);
    }
}
