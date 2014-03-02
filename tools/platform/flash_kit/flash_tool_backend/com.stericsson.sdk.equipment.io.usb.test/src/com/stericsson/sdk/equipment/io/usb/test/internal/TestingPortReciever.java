package com.stericsson.sdk.equipment.io.usb.test.internal;

import com.stericsson.sdk.equipment.io.AbstractPort;
import com.stericsson.sdk.equipment.io.port.IPort;
import com.stericsson.sdk.equipment.io.port.IPortReceiver;
import com.stericsson.sdk.equipment.io.port.PortDeliveryState;
import com.stericsson.sdk.equipment.io.port.PortException;

/**
 * @author rohacmic
 */
public class TestingPortReciever implements IPortReceiver {

    private AbstractPort deliveredPort;

    private PortDeliveryState portDeliveryState;

    /**
     * {@inheritDoc}
     */
    public void deliverPort(IPort port, PortDeliveryState state) throws PortException {
        deliveredPort = (AbstractPort)port;
        portDeliveryState = state;
    }

    /**
     * {@inheritDoc}
     */
    public void deliverPort(IPort port, PortDeliveryState state, boolean bootME) throws PortException {
        deliveredPort = (AbstractPort)port;
        portDeliveryState = state;
    }

    /**
     * @return Delivered port.
     */
    public AbstractPort getDeliveredPort() {
        return deliveredPort;
    }

    /**
     * @return Delivered port provider
     */
    public PortDeliveryState getPortDeliveryState() {
        return portDeliveryState;
    }

    /**
     * @return True if port has already been delivered, false otherwise. 
     */
    public boolean wasDelivered() {
        return (deliveredPort != null);
    }

    /**
     * {@inheritDoc}
     */
    public void closePort(IPort port) throws PortException {
    }

}
