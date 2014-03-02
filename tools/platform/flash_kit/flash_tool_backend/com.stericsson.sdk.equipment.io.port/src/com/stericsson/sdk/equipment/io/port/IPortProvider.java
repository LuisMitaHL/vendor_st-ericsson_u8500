package com.stericsson.sdk.equipment.io.port;

/**
 * @author xtomlju
 */
public interface IPortProvider {

    /**
     * Port provider implementations registers service with property PORT_PROVIDER_NAME
     * set to the name of the port.
     */
    String PORT_PROVIDER_NAME = "port.provider.name";

    /**
     * @return TBD
     */
    String getIdentifier();

    /**
     * @throws PortException
     *             TBD
     */
    void trigger() throws PortException;

    /**
     * 
     */
    void release();

    /**
     * @return TBD
     */
    boolean isTriggered();

}
