package com.stericsson.sdk.loader.communication.types;

import java.util.ArrayList;
import java.util.List;

import com.stericsson.sdk.loader.communication.types.subtypes.Device;

/**
 * 
 * @author xdancho
 * 
 */
public class ListDevicesType extends LoaderCommunicationType {

    /** */
    int deviceCount = 0;

    /** */
    ArrayList<Device> deviceList = new ArrayList<Device>();

    /**
     * 
     * @param result
     *            status of the command
     * @param count
     *            the device count
     */
    public ListDevicesType(int result, int count) {
        super(result);
        this.deviceCount = count;
    }

    /**
     * 
     * @param path
     *            the absolute path for the device
     * @param pathSize
     *            size of path string
     * @param type
     *            type of device
     * @param typeSize
     *            size of type string
     * @param blockSize
     *            size of block
     * @param start
     *            start of device
     * @param length
     *            size of device
     */
    public void addDevice(String path, int pathSize, String type, int typeSize, long blockSize, long start, long length) {
        deviceList.add(new Device(path, pathSize, type, typeSize, blockSize, start, length));

    }

    /**
     * Add device at a specific index
     * 
     * @param index
     *            index to add the device
     * @param path
     *            the absolute path for the device
     * @param pathSize
     *            size of path string
     * @param type
     *            type of device
     * @param typeSize
     *            size of type string
     * @param blockSize
     *            size of block
     * @param start
     *            start of device
     * @param length
     *            size of device
     */
    public void addDevice(int index, String path, int pathSize, String type, int typeSize, long blockSize, long start,
        long length) {
        deviceList.add(index, new Device(path, pathSize, type, typeSize, blockSize, start, length));
    }

    /**
     * 
     * @return list of devices
     */
    public List<Device> getDevices() {
        return deviceList;
    }

    /**
     * @return device description
     */
    public String getDevicesDescription() {
        StringBuffer buffer = new StringBuffer();
        for (Device device : deviceList) {
            buffer.append(device.toString());
        }
        return buffer.toString();
    }
}
