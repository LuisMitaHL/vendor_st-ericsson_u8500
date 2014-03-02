package com.stericsson.sdk.loader.communication.types;

/**
 * @author xdancho
 * 
 */
public class VolumePropertiesType extends LoaderCommunicationType {

    String fsType = "";

    long fsSize = 0;

    long fsFree = 0;

    /**
     * @param status
     *            result
     * @param type
     *            type
     * @param typeSize
     *            length of type string
     * @param size
     *            total size
     * @param free
     *            free space
     * 
     */
    public VolumePropertiesType(int status, String type, int typeSize, long size, long free) {
        super(status);

        if(LoaderCommunicationType.checkString(type, typeSize)) {
            this.fsType = type.substring(0, typeSize);
        }
        this.fsSize = size;
        this.fsFree = free;
    }

    /**
     * @return the fsType
     */
    public String getFsType() {
        return fsType;
    }

    /**
     * @return the size
     */
    public long getSize() {
        return fsSize;
    }

    /**
     * @return the free
     */
    public long getFree() {
        return fsFree;
    }

}
