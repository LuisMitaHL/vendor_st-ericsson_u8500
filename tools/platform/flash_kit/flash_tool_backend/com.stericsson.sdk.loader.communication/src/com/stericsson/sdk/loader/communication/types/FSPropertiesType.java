package com.stericsson.sdk.loader.communication.types;

import org.apache.log4j.Logger;

/**
 * @author xdancho
 * 
 */
public class FSPropertiesType extends LoaderCommunicationType {

    private static Logger logger = Logger.getLogger(FSPropertiesType.class);

    ModeType propertyMode = null;

    long propertySize = 0;

    long propertyModificationTime;

    long propertyAccessTime;

    long propertyCreateTime;

    /**
     * @param status
     *            status
     * @param mode
     *            mode
     * @param size
     *            size
     * @param modificationTime
     *            modification time
     * @param accessTime
     *            access time
     * @param createTime
     *            creation time
     */
    public FSPropertiesType(int status, int mode, long size, int modificationTime, int accessTime, int createTime) {
        super(status);
        try {
            this.propertyMode = new ModeType(mode);
        } catch (Exception e) {

            logger.debug("failed to get modeType");
        }
        this.propertySize = size;
        this.propertyModificationTime = modificationTime;
        this.propertyAccessTime = accessTime;
        this.propertyCreateTime = createTime;
    }

    /**
     * @return the type
     */
    public ModeType getMode() {
        return propertyMode;
    }

    /**
     * @return the size
     */
    public long getSize() {
        return propertySize;
    }

    /**
     * @return the modificationTime
     */
    public long getModificationTime() {
        return propertyModificationTime;
    }

    /**
     * @return the accessTime
     */
    public long getAccessTime() {
        return propertyAccessTime;
    }

    /**
     * @return the createTime
     */
    public long getCreateTime() {
        return propertyCreateTime;
    }

}
