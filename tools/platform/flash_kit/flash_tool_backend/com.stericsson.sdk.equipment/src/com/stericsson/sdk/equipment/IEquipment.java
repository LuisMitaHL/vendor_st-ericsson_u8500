package com.stericsson.sdk.equipment;

import java.util.Hashtable;
import java.util.Set;

import com.stericsson.sdk.equipment.io.port.IPort;

/**
 * @author xtomlju
 */
public interface IEquipment {

    /**
     *
     * @return true if this is an ME in non-flash mode
     */
    boolean isInWarmMode();

    /**
     * Get associated port.
     *
     * @return Port interface
     */
    IPort getPort();

    /**
     * Get associated profile.
     *
     * @return Profile interface
     */
    IEquipmentProfile getProfile();

    /**
     * Set equipment state.
     *
     * @param state
     *            New state
     */
    void setStatus(EquipmentState state);

    /**
     * Set equipment state.
     *
     * @param state
     *            New state
     *
     * @param stateMessage
     *            State Message
     */
    void setStatus(EquipmentState state, String stateMessage);

    /**
     * Get equipment state.
     *
     * @return The current state of equipment
     */
    EquipmentStatus getStatus();

    /**
     * Set equipment state message.
     *
     * @param stateMessage
     *            New state
     */
    void setStatusMessage(String stateMessage);

    /**
     * @param arguments
     *            TBD
     * @return TBD
     */
    IEquipmentTask createTask(String[] arguments);

    /**
     * sets new property and adds it into properties map
     *
     * @param id
     *            property Id
     * @param propertyName
     *            name of property
     * @param propertyValue
     *            value of property
     * @param isSecurity
     *            true if property is a security property
     */
    void setProperty(String id, String propertyName, String propertyValue, boolean isSecurity);

    /**
     * sets new security property and adds it into properties map
     *
     * @param id
     *            property Id
     * @param propertyName
     *            name of property
     * @param propertyValue
     *            value of property
     * @param isSecurity
     *            true if property is a security property
     * @param readable
     *            true if property is readable
     * @param writable
     *            true if property is writable
     */
    void setProperty(String id, String propertyName, String propertyValue, boolean isSecurity, boolean readable,
        boolean writable);

    /**
     * returns property from properties map according to given property id
     *
     * @param id
     *            property Id
     * @return equipment property
     */
    EquipmentProperty getProperty(String id);

    /**
     * returns property from properties map according to given property name
     *
     * @param name
     *            property name
     * @return equipment property
     */
    EquipmentProperty getPropertyViaName(String name);

    /**
     * returns set of property names available in properties map
     *
     * @return set or property names
     */
    Set<String> getPropertyNames();

    /**
     * returns registered properties
     *
     * @return map of properties
     */
    Hashtable<String, EquipmentProperty> getProperties();

    /**
     * returns supported security properties. List of properties is read from profile
     *
     * @return map of supported security properties
     */
    Hashtable<String, EquipmentProperty> getSupportedSecurityProperties();

    /**
     * Sets the profile for the specific equipment
     *
     * @param profile
     *            the equipment profile to set
     */
    void setProfile(IEquipmentProfile profile);

    /**
     * Get registered model for specified model enumerated type
     *
     * @param model
     *            Model enumerated type
     * @return Registered model or null if no model is available
     */
    Object getModel(EquipmentModel model);

    /**
     *
     * @param model
     *            Model enumerated type
     */
    void updateModel(EquipmentModel model);

    /**
     * returns registered properties
     *
     * @return map of properties
     */
    Hashtable<String, String> getServiceProperties();


    /**
     * @return equipment type
     */
    EquipmentType getEquipmentType();
}
