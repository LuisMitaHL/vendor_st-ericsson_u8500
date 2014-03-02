package com.stericsson.sdk.equipment.io.uart.test.internal;

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

    /**
     * {@inheritDoc}
     */
    public void deliverPort(IPort port, PortDeliveryState state) throws PortException {
        deliveredPort = (AbstractPort)port;
    }

    /**
     * {@inheritDoc}
     */
    public void deliverPort(IPort port, PortDeliveryState state, boolean bootME) throws PortException {
        deliveredPort = (AbstractPort)port;
    }

    /**
     * @return Delivered port.
     */
    public AbstractPort getDeliveredPort() {
        return deliveredPort;
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
