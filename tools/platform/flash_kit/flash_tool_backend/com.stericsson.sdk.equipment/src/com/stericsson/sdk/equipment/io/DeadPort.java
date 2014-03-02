package com.stericsson.sdk.equipment.io;

import com.stericsson.sdk.equipment.io.port.PortException;

/**
 * Dead port
 * 
 * @author xolabju
 * 
 */
public class DeadPort extends AbstractPort {

    /**
     * port id
     */
    public static final String PORT_ID = "N/A";

    /**
     * Constructor
     */
    public DeadPort() {
        super(PORT_ID);
        try {
            close();
        } catch (PortException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }

    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void open() throws PortException {

    }

    /**
     * {@inheritDoc}
     */
    public int read(byte[] buffer, int offset, int length, int timeout) throws PortException {
        return 0;
    }

    /**
     * {@inheritDoc}
     */
    public int write(byte[] buffer, int offset, int length, int timeout) throws PortException {
        return 0;
    }

}
