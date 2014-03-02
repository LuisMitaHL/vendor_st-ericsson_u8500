package com.stericsson.sdk.loader.communication.types;

/**
 * @author xdancho
 * 
 */
public class PSReadGlobalDataUnitType extends LoaderCommunicationType {

    byte[] globalData;

    /**
     * @param status
     *            result
     * @param data
     *            global data
     */
    public PSReadGlobalDataUnitType(int status, byte[] data) {
        super(status);
        this.globalData = data;
    }

    /**
     * @return the data
     */
    public byte[] getData() {
        return globalData;
    }

}
