package com.stericsson.sdk.equipment;

/**
 * @author ezaptom
 *
 */
public class EquipmentStatus {

    private EquipmentState equipmentState;

    private String equipmentStateMessage = "";

    /**
     * Constructor
     * @param state of equipment
     * @param message state message
     */
    public EquipmentStatus(EquipmentState state, String message) {
        this.equipmentState = state;
        this.equipmentStateMessage = message;
    }

    /**
     * Constructor
     * @param state of equipment
     */
    public EquipmentStatus(EquipmentState state) {
        this.equipmentState = state;
        this.equipmentStateMessage = "";
    }

    /**
     * @param pState equipment state
     */
    public void setState(EquipmentState pState) {
        equipmentState = pState;
    }

    /**
     * @param message equipment state message
     */
    public void setStateMessage(String message) {
        equipmentStateMessage = message;
    }

    /**
     * @return equipment state
     */
    public EquipmentState getState() {
        return equipmentState;
    }

    /**
     * @return equipment state message
     */
    public String getStateMessage() {
        return equipmentStateMessage;
    }
}
