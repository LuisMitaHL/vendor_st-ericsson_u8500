package com.stericsson.sdk.equipment;

import java.util.HashMap;

import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;
import org.osgi.framework.ServiceRegistration;

/**
 * @author xtomlju
 */
public abstract class AbstractPlatform implements IPlatform {

    /** */
    private HashMap<IEquipment, ServiceRegistration> registeredEquipments;

    /**
     * Constructor
     */
    public AbstractPlatform() {
        registeredEquipments = new HashMap<IEquipment, ServiceRegistration>();
    }

    /**
     * {@inheritDoc}
     */
    public void destroyEquipment(IEquipment equipment) {
        ServiceRegistration registration = registeredEquipments.get(equipment);
        if (registration != null) {
            // equipment.getProperties().put(PORT_PROPERTY, PORT_PROPERTY, "N/A",false);
            // registration.setProperties(equipment.getProperties());
            registration.unregister();
            registeredEquipments.remove(equipment);
            // equipment.setStatus(EquipmentState.ERROR);
            notifyKilledEquipment(equipment);
        }
    }

    private void notifyKilledEquipment(IEquipment equipment) {
        ServiceReference[] references = null;

        try {
            references = Activator.getBundleContext().getServiceReferences(IEquipmentListener.class.getName(), null);
            if (references != null) {
                for (ServiceReference reference : references) {
                    IEquipmentListener listener =
                        (IEquipmentListener) Activator.getBundleContext().getService(reference);
                    if (listener != null) {
                        try {
                            listener.equipmentDisconnected(equipment);
                        } catch (Exception e) {
                            e.printStackTrace();
                        }
                    }
                    Activator.getBundleContext().ungetService(reference);
                }
            }
        } catch (InvalidSyntaxException e) {
            e.printStackTrace();
        }

    }

    /**
     * Add equipment service
     * 
     * @param equipment
     *            TBD
     * @param serviceRegistration
     *            TBD
     * @throws Exception
     *             in case another device is already connected to this port
     */
    protected void registerEquipment(IEquipment equipment, ServiceRegistration serviceRegistration) throws Exception {
        // we can not have two devices connected to the same port, if we are trying to do this, 
        // something went wrong...
        //for (IEquipment e : registeredEquipments.keySet()) {
        //    if (e.getPort().getIdentifier().equals(equipment.getPort().getIdentifier())) {
        //        throw new Exception("Another device is already connected to the same port "
        //            + equipment.getPort().getIdentifier());
        //    }
        //}
        registeredEquipments.put(equipment, serviceRegistration);
    }

    /**
     * {@inheritDoc}
     */
    public void registerEquipmentProperties(IEquipment equipment) {
        ServiceRegistration serviceRegistration = registeredEquipments.get(equipment);
        if (serviceRegistration != null) {
            serviceRegistration.setProperties(equipment.getServiceProperties());
        }
    }

}
