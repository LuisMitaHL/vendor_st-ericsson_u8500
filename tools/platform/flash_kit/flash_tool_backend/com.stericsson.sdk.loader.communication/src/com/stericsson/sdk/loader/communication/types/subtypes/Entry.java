package com.stericsson.sdk.loader.communication.types.subtypes;

import java.io.Serializable;

import org.apache.log4j.Logger;

import com.stericsson.sdk.loader.communication.types.LoaderCommunicationType;
import com.stericsson.sdk.loader.communication.types.ModeType;

/**
 * @author xdancho
 * 
 */
public class Entry implements Serializable {

    private static final long serialVersionUID = -1033684669635677034L;

    private static Logger logger = Logger.getLogger(Entry.class);

    String entryName = "";

    long entrySize = 0;

    ModeType entryMode = null;

    int entryTime = 0;

    /**
     * @param name
     *            name
     * @param nameSize
     *            size of name string
     * @param size
     *            size
     * @param mode
     *            mode
     * @param time
     *            time
     */
    public Entry(String name, int nameSize, long size, int mode, int time) {

        if (LoaderCommunicationType.checkString(name, nameSize)) {
            this.entryName = name.substring(0, nameSize);
        }
        this.entrySize = size;
        try {
            this.entryMode = new ModeType(mode);
        } catch (Exception e) {
            logger.debug("failed to get modeType");
        }
        this.entryTime = time;
    }

    /**
     * @return the name
     */
    public String getName() {
        return entryName;
    }

    /**
     * @return the size
     */
    public long getSize() {
        return entrySize;
    }

    /**
     * @return the mode
     */
    public ModeType getMode() {
        return entryMode;
    }

    /**
     * @return the time
     */
    public int getTime() {
        return entryTime;
    }

}
