package com.stericsson.sdk.equipment;

import com.stericsson.sdk.equipment.io.port.IPort;
import com.stericsson.sdk.ftp.communication.IFTPCommunicationService;

/**
 * Abstract for the Ux500 warm equipment
 * 
 * @author esrimpa
 * 
 */
public abstract class AbstractUX500WarmEquipment extends AbstractEquipment {

    /** ftp service */
    protected IFTPCommunicationService ftpService = null;

    /**
     * @param port
     *            Equipment port
     * @param profile
     *            Profile
     */
    public AbstractUX500WarmEquipment(IPort port, IEquipmentProfile profile) {
        super(port, profile);
    }

    /**
     * Method to return communication service
     * 
     * @return ftpservice of type IFTPCommunicationService
     */
    public IFTPCommunicationService getFtpService() {
        return ftpService;
    }

    /**
     * {@inheritDoc}
     */
    public boolean isInWarmMode() {
        return true;
    }
}
