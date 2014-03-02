package com.stericsson.sdk.equipment.io.ip.internal;

import static com.stericsson.sdk.equipment.Activator.getServiceObject;

import org.apache.log4j.Logger;
import org.osgi.framework.BundleContext;
import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;

import com.stericsson.sdk.equipment.EquipmentState;
import com.stericsson.sdk.equipment.IEquipment;
import com.stericsson.sdk.equipment.IEquipmentTask;
import com.stericsson.sdk.equipment.io.ip.Activator;
import com.stericsson.sdk.equipment.io.port.IPort;
import com.stericsson.sdk.equipment.io.port.IPortProvider;
import com.stericsson.sdk.equipment.io.port.IPortReceiver;
import com.stericsson.sdk.equipment.io.port.PortDeliveryState;
import com.stericsson.sdk.equipment.io.port.PortException;

/**
 * Delivers port to backend. Creates the equipment and update status of equipment.
 * 
 * @author eolabor
 * 
 */
public class FtpPortProvider implements IPortProvider {

    static final Logger log = Logger.getLogger(FtpPortProvider.class);

    private boolean triggered = false;

    private boolean dumping = false;

    private String identifier;

    /**
     * Constructor for FtpPortProvider
     * 
     * @param pIdentifier
     *            port identifier (IP address)
     */
    public FtpPortProvider(String pIdentifier) {
        identifier = pIdentifier;
    }

    /**
     * {@inheritDoc}
     */
    public String getIdentifier() {
        return identifier;
    }

    /**
     * {@inheritDoc}
     */
    public boolean isTriggered() {
        return triggered;
    }

    /**
     * {@inheritDoc}
     */
    public void release() {
        try {
            IPortReceiver portReciever = getServiceObject(IPortReceiver.class);
            if (portReciever != null) {
                portReciever.closePort(new FtpPort(this));
                FtpPortHandler.getInstance().getIpDevices().remove(getIdentifier());
            }
        } catch (PortException e) {
            log.error(e.getMessage());
        }
    }

    /**
     * {@inheritDoc}
     */
    public void trigger() throws PortException {
        triggered = true;
        final FtpPortProvider tempPortProvider = this;

        IPortReceiver portReciever = null;
        portReciever = getServiceObject(IPortReceiver.class);
        IPort port = new FtpPort(tempPortProvider);
        if (portReciever != null) {
            portReciever.deliverPort(port, PortDeliveryState.WARM);
        }

    }

    /**
     * Update dumping parameter according to current DNS broadcast
     * 
     * @param isDumping
     *            boolean true if dumping otherwise false.
     */
    public void dumping(boolean isDumping) {
        dumping = isDumping;
        IEquipment equipment = getEquipment(getIdentifier());
        if (equipment != null) {
            if (isDumping()) {
                equipment.setStatus(EquipmentState.DUMPING);
            } else {
                equipment.setStatus(EquipmentState.IDLE);
            }
        } else {
            dumping = false;
        }
    }

    /**
     * Method to return equipments from context based on IP address as identifier
     * 
     * @param portIdentifier
     *            IP adress
     * @return Equipment matching filter
     */
    public IEquipment getEquipment(String portIdentifier) {
        IEquipment equipment = null;
        BundleContext context = Activator.getBundleContext();
        if (context != null) {
            String filter = "(" + IEquipmentTask.PORT_PROPERTY + "=" + portIdentifier + ")";
            try {
                ServiceReference[] serviceReferences = context.getServiceReferences(IEquipment.class.getName(), filter);
                if (serviceReferences != null) {
                    equipment = (IEquipment) context.getService(serviceReferences[0]);
                }
            } catch (InvalidSyntaxException e) {
                log.error(e.getMessage());
            }
        }
        return equipment;
    }

    /**
     * Get method for parameter dumping
     * 
     * @return boolean parameter dumping
     */
    public boolean isDumping() {
        return dumping;
    }

}
