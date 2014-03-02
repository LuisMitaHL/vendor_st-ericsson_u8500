package com.stericsson.sdk.loader.communication.types;

import java.io.UnsupportedEncodingException;

/**
 * @author ezaptom
 * 
 */
public class SystemCollectDataType extends LoaderCommunicationType {

    byte[] collectedData;

    /**
     * @param status
     *            status
     * @param data
     *            data
     */
    public SystemCollectDataType(int status, byte[] data) {
        super(status);
        collectedData = data;
    }

    /**
     * @return collected data
     */
    public String getCollectedData() {
        String string = "";

        try {
            string = new String(collectedData, "UTF-8");
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
        }
        return string;
    }
}
