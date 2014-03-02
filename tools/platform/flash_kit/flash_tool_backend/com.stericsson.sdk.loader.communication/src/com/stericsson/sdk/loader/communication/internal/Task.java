package com.stericsson.sdk.loader.communication.internal;

import com.stericsson.sdk.equipment.io.port.ILCPort;
import com.stericsson.sdk.equipment.io.port.PortException;

/**
 * 
 * @author xdancho
 * 
 */
public abstract class Task {

    long instancePointer;

    long dataPointer;

    int length;

    byte[] data;

    private ILCPort port;

    int pos;

    boolean done;

    boolean error;

    /**
     * 
     * @param len
     *            length to read/write
     * @param dataPtr
     *            the pointer
     * @param instancePtr
     *            instance pointer
     * @param p
     *            port object
     */
    public Task(int len, long dataPtr, long instancePtr, ILCPort p) {
        done = false;
        length = len;
        dataPointer = dataPtr;
        instancePointer = instancePtr;
        port = p;
        data = new byte[len];
        pos = 0;
    }

    /**
     * get the instance pointer
     * 
     * @return the instance pointer
     */
    public long getInstancePointer() {
        return instancePointer;
    }

    /**
     * get the data pointer
     * 
     * @return the data pointer
     */
    public long getDataPointer() {
        return dataPointer;
    }

    /**
     * @return Port interface
     */
    public ILCPort getPort() {
        return port;
    }

    /**
     * get the length
     * 
     * @return the length
     */
    public int getLength() {
        return length;
    }

    /**
     * get the data
     * 
     * @return the data
     */
    public byte[] getData() {
        return data;
    }

    /**
     * set data
     * 
     * @param dataBuffer
     *            the data
     */
    public void setData(byte[] dataBuffer) {
        this.data = dataBuffer;
    }

    /**
     * @param p
     *            Position
     */
    public void setPos(int p) {
        pos = p;
    }

    /**
     * @return Position
     */

    public int getPos() {
        return pos;
    }

    /**
     * @throws PortException
     *             TBD
     */
    public abstract void process() throws PortException;

    /**
     * @return True if task is done
     */
    public boolean isDone() {
        return done;
    }

}
