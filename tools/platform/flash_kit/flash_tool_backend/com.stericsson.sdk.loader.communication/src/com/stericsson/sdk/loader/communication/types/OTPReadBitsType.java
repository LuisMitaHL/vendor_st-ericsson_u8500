package com.stericsson.sdk.loader.communication.types;

/**
 * @author xdancho
 * 
 */
public class OTPReadBitsType extends LoaderCommunicationType {

    byte[] dataBits = null;

    byte[] lockStatus = null;

    /**
     * @param result
     *            result
     * @param dBits
     *            data bits
     * @param lBits
     *            locked bits
     */
    public OTPReadBitsType(int result, byte[] dBits, byte[] lBits) {
        super(result);
        this.dataBits = dBits;
        this.lockStatus = lBits;

    }

    /**
     * @return the dataBits
     */
    public byte[] getDataBits() {
        return dataBits;
    }

    /**
     * @return the lockStatusBits
     */
    public byte[] getLockStatusBits() {
        return lockStatus;
    }

}
