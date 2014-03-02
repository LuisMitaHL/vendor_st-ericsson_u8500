package com.stericsson.sdk.loader.communication.types;

/**
 * @author xdancho
 * 
 */
public class SecurityPropertiesType extends LoaderCommunicationType {

    byte[] propertiesBuffer;

    /**
     * @param status
     *            result
     * @param buffer
     *            properties buffer
     */
    public SecurityPropertiesType(int status, byte[] buffer) {

        super(status);
        this.propertiesBuffer = buffer;
    }

    /**
     * get the properties buffer
     * @return
     *      the buffer
     */
    public byte[] getBuffer() {
        return propertiesBuffer;
    }
}
