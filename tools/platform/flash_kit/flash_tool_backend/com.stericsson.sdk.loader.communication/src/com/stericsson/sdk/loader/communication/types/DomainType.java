package com.stericsson.sdk.loader.communication.types;

/**
 * 
 * @author xdancho
 * 
 */
public class DomainType extends LoaderCommunicationType {

    int writtenDomain = 0;

    /**
     * @param status
     *            status of command
     * 
     * @param wDomain
     *            the written domain
     */
    public DomainType(int status, int wDomain) {
        super(status);

        this.writtenDomain = wDomain;
    }

    /**
     * 
     * @return the written domain
     */
    public int getWrittenDomain() {
        return writtenDomain;
    }

}
