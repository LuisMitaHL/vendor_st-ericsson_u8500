package com.stericsson.sdk.loader.communication.types.subtypes;

import com.stericsson.sdk.loader.communication.types.LoaderCommunicationType;

/**
 * 
 * @author xdancho
 * 
 */
public class Device {

    String devicePath = "";

    String deviceType = "";

    long deviceBlockSize = 0;

    long deviceStart = 0;

    long deviceLength = 0;

    /**
     * 
     * @param path
     *            path of device
     * @param pathSize
     *            size of path string
     * @param type
     *            type of device
     * @param typeSize
     *            size of type string
     * @param blockSize
     *            blocksize of device
     * @param start
     *            start of device
     * @param length
     *            length of device
     * 
     */
    public Device(String path, int pathSize, String type, int typeSize,
            long blockSize, long start, long length) {

        if (LoaderCommunicationType.checkString(path, pathSize)) {
            this.devicePath = path.substring(0, pathSize);
        }

        if (LoaderCommunicationType.checkString(type, typeSize)) {
            this.deviceType = type.substring(0, typeSize);
        }
        this.deviceBlockSize = blockSize;
        this.deviceStart = start;
        this.deviceLength = length;

    }

    /**
     * 
     * @return device path
     */
    public String getPath() {
        return devicePath;
    }

    /**
     * 
     * @return type of device
     */
    public String getType() {
        return deviceType;
    }

    /**
     * 
     * @return block size of device
     */
    public long getBlockSize() {
        return deviceBlockSize;
    }

    /**
     * 
     * @return start of device
     */
    public long getStart() {
        return deviceStart;
    }

    /**
     * 
     * @return size of device
     */
    public long getLength() {
        return deviceLength;
    }

    /**
     * {@inheritDoc}
     */
    public String toString() {
        return devicePath + ";" + deviceType + ";" + deviceStart + ";" + deviceLength + ";" + deviceBlockSize + ";";
    }
}
