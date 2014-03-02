package com.stericsson.sdk.loader.communication.types;

import java.util.ArrayList;

import com.stericsson.sdk.loader.communication.types.subtypes.Entry;

/**
 * @author xdancho
 * 
 */
public class ListDirectoryType extends LoaderCommunicationType {

    int entryCount = 0;

    ArrayList<Entry> directoryList = new ArrayList<Entry>();

    /**
     * @param status
     *            result
     * @param count
     *            count
     * 
     */
    public ListDirectoryType(int status, int count) {
        super(status);
        entryCount = count;
    }

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
    public void addDirectory(String name, int nameSize, long size, int mode, int time) {
        directoryList.add(new Entry(name, nameSize, size, mode, time));

    }

    /**
     * @return list of directorys
     */
    public ArrayList<Entry> getDirectorys() {
        return directoryList;
    }

}
