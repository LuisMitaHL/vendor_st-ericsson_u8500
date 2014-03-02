package com.stericsson.sdk.equipment.io.ip.internal;

import com.stericsson.sdk.equipment.io.AbstractFTPPort;
import com.stericsson.sdk.equipment.io.port.PortException;

/**
 * Communication port for FTP (network over USB)
 * 
 * @author eolabor
 * 
 */
public class FtpPort extends AbstractFTPPort {

    private FtpPortProvider portprovider;

    /**
     * Initializes a newly created FtpPort with given port identifier.
     * 
     * @param pPort
     *            FtpPortProvider
     */
    public FtpPort(FtpPortProvider pPort) {
        super(pPort.getIdentifier());
        portprovider = pPort;
    }

    /**
     * {@inheritDoc}
     * 
     * @see com.stericsson.sdk.equipment.io.AbstractFTPPort#isDumping()
     */
    @Override
    public boolean isDumping() {
        return portprovider.isDumping();
    }

    /**
     * {@inheritDoc}
     * 
     * @see com.stericsson.sdk.equipment.io.AbstractFTPPort#close()
     */
    @Override
    public void close() throws PortException {
        portprovider.release();
    }

}
