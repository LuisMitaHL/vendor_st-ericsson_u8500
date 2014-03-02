package com.stericsson.sdk.equipment;

import java.util.Enumeration;
import java.util.HashSet;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.Set;

import org.apache.log4j.Logger;
import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;

import com.stericsson.sdk.common.configuration.ConfigurationOption;
import com.stericsson.sdk.equipment.io.DeadPort;
import com.stericsson.sdk.equipment.io.port.IPort;
import com.stericsson.sdk.equipment.io.port.PortException;

/**
 * @author xtomlju
 */
public abstract class AbstractEquipment implements IEquipment {

    /** */
    private IPort equipmentPort;

    /** */
    private IEquipmentProfile equipmentProfile;

    private Hashtable<String, EquipmentProperty> properties = null;

    private Hashtable<String, EquipmentProperty> supportedSecurityProperties = null;

    private EquipmentStatus equipmentStatus;

    private Hashtable<EquipmentModel, Object> equipmentModels;

    private Logger logger = Logger.getLogger(AbstractEquipment.class.getName());

    private String information;

    /**
     * Constructor.
     * 
     * @param port
     *            Communication port
     * @param profile
     *            Equipment profile
     */
    public AbstractEquipment(IPort port, IEquipmentProfile profile) {
        equipmentPort = port;
        equipmentProfile = profile;
        equipmentStatus = new EquipmentStatus(EquipmentState.IDLE);
        properties = new Hashtable<String, EquipmentProperty>();
        equipmentModels = new Hashtable<EquipmentModel, Object>();

        information = "";

    }

    // Force inheriting classes to implement it reasonably as this is used
    // for logging in this class.
    /**
     * {@inheritDoc}
     */
    public abstract String toString();

    /**
     * override if needed {@inheritDoc}
     */
    public void updateModel(EquipmentModel model) {

    }

    /**
     * {@inheritDoc}
     */
    public IPort getPort() {
        return equipmentPort;
    }

    /**
     * {@inheritDoc}
     */
    public IEquipmentProfile getProfile() {
        return equipmentProfile;
    }

    /**
     * {@inheritDoc}
     */
    public void setProfile(IEquipmentProfile profile) {
        equipmentProfile = profile;
        notifyStateChanged();
    }

    /**
     * {@inheritDoc}
     */
    public void setStatus(EquipmentState state) {
        setStatus(state, "");
    }

    /**
     * {@inheritDoc}
     */
    public void setStatus(EquipmentState state, String stateMessage) {
        equipmentStatus.setState(state);
        equipmentStatus.setStateMessage(stateMessage);
        notifyStateChanged();
        if (state == EquipmentState.ERROR) {
            removePort();
        }
    }

    /**
     * @param info
     *            information about device
     */
    public void setInfo(String info) {
        this.information = info;
    }

    /**
     * @return information about device (Nomadik ID, ASIC Version, Chip Option, Chip Customer,
     *         Secure Mode, Public ID)
     */
    public String getInfo() {
        return information;
    }

    /**
     * Set/update model. Any existing model will be replaced by specified model
     * 
     * @param model
     *            Model enumerated type
     * @param instance
     *            Model instance
     */
    public void setModel(EquipmentModel model, Object instance) {
        equipmentModels.put(model, instance);
    }

    /**
     * {@inheritDoc}
     */
    public Object getModel(EquipmentModel model) {
        return equipmentModels.get(model);
    }

    private void removePort() {
        try {
            equipmentPort.close();
        } catch (PortException e) {
            e.getCause(); /* Dummy call to prevent checkstyle error */
        }
        equipmentPort = new DeadPort();
        setProperty(IPlatform.PORT_PROPERTY, IPlatform.PORT_PROPERTY, equipmentPort.getPortName(), false);
    }

    /**
     * {@inheritDoc}
     */
    public EquipmentStatus getStatus() {
        return equipmentStatus;
    }

    /**
     * {@inheritDoc}
     */
    public void setStatusMessage(String pEquipmentStateMessage) {
        equipmentStatus.setStateMessage(pEquipmentStateMessage);
    }

    /**
     * Notify all IEquipmentListener services that state has changed.
     */
    protected void notifyStateChanged() {
        new ServiceHelper().notifyStateChanged();
    }

    /**
     * Register equipment property.
     */
    protected void registerProperties() {
        new ServiceHelper().registerProperties();
    }

    private class ServiceHelper {

        public void notifyStateChanged() {
            ServiceReference[] references = null;

            try {
                references =
                    Activator.getBundleContext().getServiceReferences(IEquipmentListener.class.getName(), null);
                if (references != null) {
                    for (ServiceReference reference : references) {
                        IEquipmentListener listener =
                            (IEquipmentListener) Activator.getBundleContext().getService(reference);
                        try {
                            listener.equipmentStateChanged(AbstractEquipment.this, getStatus());
                        } catch (Exception e) {
                            logger.warn("Exception in state changed notification listener", e);
                        }
                        Activator.getBundleContext().ungetService(reference);
                    }
                }
            } catch (InvalidSyntaxException e) {
                logger.error("Invalid syntax in service reference filter", e);
            }
        }

        public void registerProperties() {
            ServiceReference[] references = null;
            String filter = "(" + IPlatform.PLATFORM_PROPERTY + "=" + getProperty(IPlatform.PLATFORM_PROPERTY) + ")";
            try {
                references = Activator.getBundleContext().getServiceReferences(IPlatform.class.getName(), filter);
                if (references != null) {
                    for (ServiceReference reference : references) {
                        IPlatform platform = (IPlatform) Activator.getBundleContext().getService(reference);
                        try {
                            platform.registerEquipmentProperties(AbstractEquipment.this);
                        } catch (Exception e) {
                            logger.warn("Exception while registering equipment properties", e);
                        }
                        Activator.getBundleContext().ungetService(reference);
                    }
                }
            } catch (InvalidSyntaxException e) {
                logger.error("Invalid syntax in service reference filter", e);
            }
        }
    }

    /**
     *{@inheritDoc}
     */
    public void setProperty(String id, String propertyName, String propertyValue, boolean isSecurity) {
        properties.put(id, new EquipmentProperty(id, propertyName, propertyValue, isSecurity));
        registerProperties();
    }

    /**
     * {@inheritDoc}
     */
    public void setProperty(String id, String propertyName, String propertyValue, boolean isSecurity, boolean readable,
        boolean writable) {
        properties.put(id, new EquipmentProperty(id, propertyName, propertyValue, isSecurity, readable, writable));
        registerProperties();
    }

    /**
     * {@inheritDoc}
     */
    public EquipmentProperty getProperty(String id) {
        return properties.get(id);
    }

    /**
     *{@inheritDoc}
     */
    public EquipmentProperty getPropertyViaName(String name) {
        if (!properties.isEmpty()) {
            EquipmentProperty property = null;
            Iterator<EquipmentProperty> it = properties.values().iterator();
            while (it.hasNext()) {
                property = it.next();
                if (property.getPropertyName().contentEquals(name)) {
                    return property;
                }
            }
        }
        return null;
    }

    /**
     * {@inheritDoc}
     */
    public Set<String> getPropertyNames() {
        Set<String> names = new HashSet<String>();
        Iterator<EquipmentProperty> it = properties.values().iterator();
        while (it.hasNext()) {
            names.add(it.next().getPropertyName());
        }
        return names;
    }

    /**
     * {@inheritDoc}
     */
    public Hashtable<String, EquipmentProperty> getProperties() {
        return properties;
    }

    /**
     * {@inheritDoc}
     */
    public Hashtable<String, EquipmentProperty> getSupportedSecurityProperties() {
        if (supportedSecurityProperties == null) {
            supportedSecurityProperties = getProfile().getSupportedSecurityProperties();
        }
        return supportedSecurityProperties;
    }

    /**
     * @param id
     *            TBD
     * @return TBD
     */
    public EquipmentProperty getSecurityProperty(String id) {
        return getSupportedSecurityProperties().get(id);
    }

    /**
     * @param name
     *            TBD
     * @return TBD
     */
    public EquipmentProperty getSecurityPropertyViaName(String name) {
        if (!getSupportedSecurityProperties().isEmpty()) {
            EquipmentProperty property = null;
            Iterator<EquipmentProperty> it = getSupportedSecurityProperties().values().iterator();
            while (it.hasNext()) {
                property = it.next();
                if (property.getPropertyName().contentEquals(name)) {
                    return property;
                }
            }
        }
        return null;
    }

    /**
     * searches whether property given by its name or id is a security property.
     * 
     * @param id
     *            property id
     * @param name
     *            property name
     * 
     * @return true if property is a security property
     */
    public boolean isSecurity(String id, String name) {
        if (id != null && getSupportedSecurityProperties().containsKey(id)) {
            name = getSupportedSecurityProperties().get(id).getPropertyName();
            return true;
        }
        if (name != null) {
            Enumeration<EquipmentProperty> props = getSupportedSecurityProperties().elements();
            EquipmentProperty prop;
            while (props.hasMoreElements()) {
                prop = props.nextElement();
                if (prop.getPropertyName().contentEquals(name)) {
                    id = prop.getPropertyId();
                    return true;
                }
            }
        }
        return false;
    }

    /**
     * @return Service properties
     */
    public Hashtable<String, String> getServiceProperties() {
        Hashtable<String, EquipmentProperty> allProperties = getProperties();
        Hashtable<String, String> result = new Hashtable<String, String>();

        result.put(AbstractPlatform.PLATFORM_PROPERTY, allProperties.get(AbstractPlatform.PLATFORM_PROPERTY)
            .getPropertyValue());
        result
            .put(AbstractPlatform.PORT_PROPERTY, allProperties.get(AbstractPlatform.PORT_PROPERTY).getPropertyValue());

        return result;
    }

    /**
     * Method check ME profile if LCM/LCD should be initialized.
     * 
     * @return true if ME uses loader communication
     */
    public boolean usesLoaderCommunication() {
        String value = getProfile().getProperty(ConfigurationOption.PROPERTY_LOADER_COMMUNICATION);
        if ("true".equalsIgnoreCase(value) || "yes".equalsIgnoreCase(value)) {
            return true;
        }
        return false;
    }

}
