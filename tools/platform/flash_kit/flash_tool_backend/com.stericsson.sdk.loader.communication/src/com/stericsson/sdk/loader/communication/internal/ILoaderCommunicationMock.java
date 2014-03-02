package com.stericsson.sdk.loader.communication.internal;

import com.stericsson.sdk.equipment.io.port.IPort;

/**
 * @author xdancho
 * 
 */
public interface ILoaderCommunicationMock {

    /**
     * @param result
     *            the result to return when sending commands
     */
    void setResult(Object result);

    /**
     * force error message
     * 
     * @param port
     *            the port
     */
    void forceError(IPort port);

    /**
     * force message
     */
    void forceMessage();

    /**
     * delay in ms before returning from command
     * 
     * @param delayInMs
     *            delay in ms
     */
    void setDelay(int delayInMs);

    /**
     * {@inheritDoc}
     */
    String getLoaderErrorDesc(int error);

}
