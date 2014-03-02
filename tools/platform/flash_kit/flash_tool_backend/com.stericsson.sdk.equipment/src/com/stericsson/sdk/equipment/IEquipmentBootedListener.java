package com.stericsson.sdk.equipment;

/**
 * Implementer of this interface is notified when new ME is booted 
 * 
 * @author xtomzap
 *
 */
public interface IEquipmentBootedListener {

    /**
     * New ME booted notification
     * 
     * @param equipment new ME
     */
    void newEquipmentBooted(IEquipment equipment);

}
