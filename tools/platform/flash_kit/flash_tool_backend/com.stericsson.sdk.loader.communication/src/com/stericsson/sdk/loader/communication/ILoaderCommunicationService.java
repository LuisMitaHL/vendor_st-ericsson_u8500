package com.stericsson.sdk.loader.communication;

import java.util.List;

import com.stericsson.sdk.equipment.io.port.IPort;
import com.stericsson.sdk.loader.communication.types.subtypes.SupportedCommand;

/**
 * @author xdancho
 */
public interface ILoaderCommunicationService {

    /**
     * Send a command to the loader communication service.
     * 
     * @param instanceId
     *            Instance identifier.
     * @param methodName
     *            Name of method (command) to use
     * @param args
     *            Command arguments
     * @param listener
     *            Loader communication listener interface
     * @return Result object
     */
    Object sendCommand(long instanceId, String methodName, Object[] args, ILoaderCommunicationListener listener);

    /**
     * Initialize loader communication for specified port.
     * 
     * @param port
     *            Port interface
     * @return Instance identifier
     * @throws Exception
     *             exception
     */
    long initialize(IPort port) throws Exception;

    /**
     * Clean-up loader communication for specified instance identifier.
     * 
     * @param instanceId
     *            Instance identifier
     * @return True if finalization was successful
     */
    boolean cleanUp(long instanceId);

    /**
     * Send raw data to specified instance identifier.
     * 
     * @param instanceId
     *            Instance identifier
     * @param rawData
     *            Data to send
     */
    void sendRawData(long instanceId, byte[] rawData);

    /**
     * Set timeout values used by loader communication.
     * 
     * @param type
     *            Type of timeout
     * @param value
     *            Timeout value
     */
    void setTimeouts(String type, long value);

    /**
     * Get timeout value for specified time out type
     * 
     * @param type
     *            Timeou type
     * @return Timeout value
     */
    long getTimeouts(String type);

    /**
     * get the error description from Loader Communication Module
     * 
     * @param error
     *            .
     * @return description .
     */
    String getLoaderErrorDesc(int error);

    /**
     * Retrieve a list of supported commands for specified instance identifier.
     * 
     * @param instanceId
     *            Instance identifier
     * @return List of supported commands
     */
    List<SupportedCommand> getSupportedCommands(long instanceId);
}
