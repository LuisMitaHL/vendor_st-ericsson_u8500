package com.stericsson.sdk.equipment.tasks;

/**
 * @author ezaptom
 *
 */
public class EquipmentBootException extends Exception {

    /** */
    private static final long serialVersionUID = 3533113756957640166L;

    /**
     * Constructs a new exception with the specified detail message.
     * 
     * @param msg Detail message describing root cause for this exception.
     */
    public EquipmentBootException(String msg) {
        super(msg);
    }
}
