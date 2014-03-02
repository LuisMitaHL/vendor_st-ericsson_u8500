package com.stericsson.sdk.equipment.bootrom;

import java.io.File;
import java.util.Map;

import org.apache.log4j.Logger;

import com.stericsson.sdk.common.HexUtilities;
import com.stericsson.sdk.common.LittleEndianByteConverter;
import com.stericsson.sdk.equipment.Activator;

/**
 * 
 * @author xolabju
 * 
 */
public final class BootRomTokens {

    private BootRomTokens() {
    }

    private static Map<Long, String> tokenDescriptions;

    private static final String TOKEN_XML = "bootrom.xml";

    private static Logger log = Logger.getLogger(BootRomTokens.class.getName());

    /**
     * Returns a description based on the given token
     * 
     * @param token
     *            the boot rom token
     * @return a description based on the given token
     */
    public static String getTokenDescription(byte[] token) {
        if (tokenDescriptions == null) {
            log.debug("Mapping token descriptions");
            String res = Activator.getResourcesPath();
            if (res != null) {
                File xmlFile = new File(res + File.separator + TOKEN_XML);
                try {
                    tokenDescriptions = BootRomTokenParser.parseXMLErrorCodes(xmlFile);
                } catch (Exception e) {
                    log.warn("Failed to parse token descriptions", e);
                }
            }
        }
        long byteArrayToInt = LittleEndianByteConverter.byteArrayToLong(token);
        String descr = null;
        if (tokenDescriptions != null) {
            descr = tokenDescriptions.get(byteArrayToInt);
            if (descr == null || descr.trim().length() == 0) {
                descr = "Unknown token";
            }
        }
        return HexUtilities.toHexString(byteArrayToInt) + ": " + descr;
    }
}
