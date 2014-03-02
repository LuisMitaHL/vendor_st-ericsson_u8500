/**
 *
 */
package com.stericsson.sdk.assembling.utilities;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

import com.stericsson.sdk.assembling.AssemblerException;

/**
 * Utility class for content type operations.
 *
 * @author mbodan01
 */
public enum ContentTypeUtilities {
    /**
     * The only instance.
     */
    INSTANCE;

    /**
     * Method is used to check content subtype.
     *
     * @param pContentType
     *            input content type
     * @param pSubType
     *            input content subtype
     * @return true if content type is valid and subtype is equals
     */
    public static boolean checkContentSubType(String pContentType, String pSubType) {
        String pattern = "^x-[a-zA-z]+/" + pSubType + "$";
        Pattern p = Pattern.compile(pattern);
        Matcher m = p.matcher(pContentType);
        return m.find();
    }

    /**
     * Method is used to get a prefix of content type.
     *
     * @param pContentType
     *            input content type
     * @return prefix
     */
    public static String getPrefixOfContentType(String pContentType) {
        String result = null;
        String pattern = "^(x-[a-zA-z]+)/[a-zA-z-]+$";
        Pattern p = Pattern.compile(pattern);
        Matcher m = p.matcher(pContentType);
        if (m.find()) {
            result = m.group(1);
        }
        return result;
    }

    /**
     * Method is used to make content type from prefix and sub type.
     *
     * @param pContentPrefix
     *            input prefix
     * @param pSubType
     *            input subtype
     * @return content type.
     * @throws AssemblerException
     *             common exception for assemble tool
     */
    public static String makeContentType(String pContentPrefix, String pSubType) throws AssemblerException {
        if (pSubType == null || pSubType.length() == 0) {
            throw new AssemblerException("Wrong input subtype of content prefix.");
        }
        if (pContentPrefix != null && pContentPrefix.length() > 0) {
            return pContentPrefix + "/" + pSubType;
        } else {
            throw new AssemblerException("Wrong format of content prefix.");
        }
    }
}
