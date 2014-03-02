package com.stericsson.sdk.equipment.tasks;

import java.util.ArrayList;
import java.util.List;

import com.stericsson.sdk.equipment.model.IEquipmentFlashModel;
import com.stericsson.sdk.loader.communication.types.ListDevicesType;
import com.stericsson.sdk.loader.communication.types.subtypes.Device;

/**
 * @author xtomlju
 */
public class EquipmentFlashModel implements IEquipmentFlashModel {

    /** */
    private List<Device> devices;

    /** */
    private String flashReport;

    /**
     * Constructor
     * 
     * @param flashDevices
     *            Flash devices
     * @param report
     *            Flash report
     */
    public EquipmentFlashModel(ListDevicesType flashDevices, String report) {
        devices = flashDevices.getDevices();
        flashReport = report;
    }

    /**
     * {@inheritDoc}
     */
    public String[] getFlashDeviceNames(String filter) {

        List<String> result = new ArrayList<String>();

        for (Device device : devices) {
            if (filter != null) {
                if (filter.contains(device.getType())) {
                    result.add(device.getPath());
                }
            } else {
                result.add(device.getPath());
            }
        }

        String[] returnArray = new String[result.size()];
        for (int i = 0; i < result.size(); i++) {
            returnArray[i] = (String) result.get(i);
        }

        return returnArray;
    }

    /**
     * {@inheritDoc}
     */
    public long getLength(String deviceName) {

        long result = -1;
        Device device = findDevice(deviceName);

        if (device != null) {
            result = device.getLength();
        }

        return result;
    }

    /**
     * {@inheritDoc}
     */
    public long getStart(String deviceName) {

        long result = -1;
        Device device = findDevice(deviceName);

        if (device != null) {
            result = device.getStart();
        }

        return result;
    }

    /**
     * {@inheritDoc}
     */
    public String getType(String deviceName) {

        String result = "Unknown";
        Device device = findDevice(deviceName);

        if (device != null) {
            result = device.getType();
        }

        return result;
    }

    /**
     * {@inheritDoc}
     */
    public long getBlockSize(String deviceName) {

        long result = -1;
        Device device = findDevice(deviceName);

        if (device != null) {
            result = device.getBlockSize();
        }

        return result;
    }

    /**
     * {@inheritDoc}
     */
    public String getReport() {
        return flashReport;
    }

    /**
     * 
     * @param deviceName
     * @return
     */
    private Device findDevice(String deviceName) {

        Device result = null;

        for (Device device : devices) {
            if (device.getPath().compareToIgnoreCase(deviceName) == 0) {
                result = device;
                break;
            }
        }

        return result;
    }

}
