package com.stericsson.sdk.equipment.io.port;

/**
 * Port Receiver interface
 *
 * @author xtomlju
 */
public interface IPortReceiver {

    /**
     * @param state
     *            state of the port delivery; either warm or service
     * @param port
     *            port to be delivered
     * @throws PortException
     *             When I/O error on port occurs (profile is null).
     */
    void deliverPort(IPort port, PortDeliveryState state) throws PortException;

    /**
     * @param state
     *            state of the port delivery; either warm or service
     * @param port
     *            port to be delivered
     * @param bootME
     *            defines if boot procedure should be executed on ME
     * @throws PortException
     *             When I/O error on port occurs (profile is null).
     */
    void deliverPort(IPort port, PortDeliveryState state, boolean bootME) throws PortException;

    /**
     * @param port
     *            port to be closed
     * @throws PortException
     *             When I/O error on port occurs.
     */
    void closePort(IPort port) throws PortException;
}
