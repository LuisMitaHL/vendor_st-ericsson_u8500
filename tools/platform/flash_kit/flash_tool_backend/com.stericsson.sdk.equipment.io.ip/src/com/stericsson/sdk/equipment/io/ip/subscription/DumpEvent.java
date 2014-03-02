package com.stericsson.sdk.equipment.io.ip.subscription;

/**
 * This class represents dump events.
 * 
 * @author eolabor
 */
public class DumpEvent {

    private String equipmentID;

    private String dumpPath;

    /**
     * Constructor - creates a new dump event.
     * 
     * @param identifier
     *            Unique identifier of the ME which generated this dump event. Usually this
     *            identifier is IMEI, IP address of the ME or similar.
     * @param path
     *            Path to the dump that generated this event (mDNSBroadcast).
     */
    public DumpEvent(String identifier, String path) {
        equipmentID = identifier;
        dumpPath = path;
    }

    /**
     * Returns the equipmentID.
     * 
     * @return equpment_id A unique identifier for the ME which generated this dump event.
     */
    public String getEquipmentId() {
        return equipmentID;
    }

    /**
     * Returns the dumpPath.
     * 
     * @return the dumpPath Path to the dump that generated this event (mDNSBroadcast).
     */
    public String getDumpPath() {
        return dumpPath;
    }

}
