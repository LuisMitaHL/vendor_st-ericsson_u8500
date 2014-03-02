package com.stericsson.sdk.loader.communication.internal;

import com.stericsson.sdk.equipment.io.port.ILCPort;
import com.stericsson.sdk.equipment.io.port.PortException;

/**
 * @author xtomlju
 */
public class WriterTask extends Task {

    /**
     * Constructor.
     * @param len TBD
     * @param dataPtr TBD
     * @param instancePtr TBD
     * @param p TBD
     */
    public WriterTask(int len, long dataPtr, long instancePtr, ILCPort p) {
        super(len, dataPtr, instancePtr, p);
        // TODO Auto-generated constructor stub
    }

    /**
     * {@inheritDoc}
     */
    public void process() throws PortException {
        // TODO Auto-generated method stub
        int written = getPort().write(getData(), 50000);
        if (written < 0) {
            if ((written != -116) && (written != -110)) {
                throw new PortException("Unable to write data to port: " + written);
            }
        }

        if (written == getLength()) {
            done = true;
        }

    }

}
