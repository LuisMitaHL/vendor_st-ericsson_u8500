package com.stericsson.sdk.equipment.model;

/**
 * @author xtomlju
 */
public interface IEquipmentFlashModel {

    /**
     * Get an array of flash device names
     * 
     * @param filter
     *            Device filter. Each device will be queried for type. If type is a sub sequence of
     *            filter it will be matched and part of the returned array.
     * @return Flash device name array
     */
    String[] getFlashDeviceNames(String filter);

    /**
     * Get flash device type for specified device name
     * 
     * @param deviceName
     *            Name of flash device
     * @return Flash device type
     */
    String getType(String deviceName);

    /**
     * Get flash device offset for specified device name
     * 
     * @param deviceName
     *            Name of flash device
     * @return Flash device offset
     */
    long getStart(String deviceName);

    /**
     * Get flash device length (size) for specified device name
     * 
     * @param deviceName
     *            Name of flash device
     * @return Flash device length
     */
    long getLength(String deviceName);

    /**
     * Get block size for specified device name
     * 
     * @param deviceName
     *            Name of flash device
     * @return Block size
     */
    long getBlockSize(String deviceName);

    /**
     * @return A flash report
     */
    String getReport();
}
