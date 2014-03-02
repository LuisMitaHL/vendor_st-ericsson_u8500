package com.stericsson.sdk.equipment;

/**
 * @author xtomlju
 */
public interface IEquipmentListener {

    /**
     * @param equipment
     *            TBD
     * @param newState
     *            TBD
     */
    void equipmentStateChanged(IEquipment equipment, EquipmentStatus newState);

    /**
     * Called when the equipment is disconnected
     * 
     * @param equipment
     *            the disconnected equipment
     */
    void equipmentDisconnected(IEquipment equipment);

}
