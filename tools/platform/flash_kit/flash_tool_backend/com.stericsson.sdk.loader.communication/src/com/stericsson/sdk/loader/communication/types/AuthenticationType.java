package com.stericsson.sdk.loader.communication.types;

/**
 * @author xdancho
 */
public class AuthenticationType extends LoaderCommunicationType {

    private byte[] data;

    /**
     * Constructor.
     * 
     * @param result
     *            TBD
     */
    public AuthenticationType(int result) {
        super(result);
    }

    /**
     * Constructor.
     * 
     * @param result
     *            TBD
     * @param size
     *            TBD
     * @param resData
     *            TBD
     */
    public AuthenticationType(int result, int size, byte[] resData) {
        super(result);
        data = new byte[size];
        data = resData;
    }

    /**
     * @return TBD
     */
    public byte[] getChallengeData() {
        return data;
    }

}
