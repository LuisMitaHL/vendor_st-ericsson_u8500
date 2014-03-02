package com.stericsson.sdk.loader.communication.internal;

import com.stericsson.sdk.equipment.io.port.ILCPort;
import com.stericsson.sdk.equipment.io.port.PortException;
import com.stericsson.sdk.loader.communication.Activator;

/**
 * @author xtomlju
 */
public class ReaderTask extends Task {

    /**
     * Constructor.
     * 
     * @param len
     *            TBD
     * @param dataPtr
     *            TBD
     * @param instancePtr
     *            TBD
     * @param p
     *            TBD
     */
    public ReaderTask(int len, long dataPtr, long instancePtr, ILCPort p) {
        super(len, dataPtr, instancePtr, p);
    }

    private static final int WIN_READ_TIMEOUT = 100000;

    private static final int LINUX_READ_TIMEOUT = 100000;

    /**
     * {@inheritDoc}
     */
    @Override
    public void process() throws PortException {
        done = false;
        while (true) {
            if (getPos() == getLength()) {
                done = true;
                break;
            }

            int read =
                getPort().read(getData(), getPos(), (int) getLength() - getPos(),
                    Activator.isRunningOnWindows() ? WIN_READ_TIMEOUT : LINUX_READ_TIMEOUT);
            if (read > 0) {
                setPos(getPos() + read);
            } else {
                break;
            }
        }
    }
}
