package com.stericsson.sdk.equipment;

import com.stericsson.sdk.equipment.io.port.IPort;

/**
 * @author xtomlju
 */
public interface IPlatform {

    /** Property for platform */
    String PLATFORM_PROPERTY = "platform";

    /** Property for port */
    String PORT_PROPERTY = "port";

    /** property for state */
    String STATE_PROPERTY = "state";

    /**
     * @param port
     *            TBD
     * @param profile
     *            TBD
     * @param bootME should be ME booted ?
     */
    void createEquipment(IPort port, IEquipmentProfile profile, boolean bootME);

    /**
     * 
     * @param equipment
     *            TBD
     */
    void destroyEquipment(IEquipment equipment);

    /**
     * Updates the registered properties for an equipment
     * 
     * @param equipment
     *            the equipment
     */
    void registerEquipmentProperties(IEquipment equipment);
}
