package com.stericsson.sdk.equipment.io.port;

/**
 * @author tomas
 */
public interface IPort {

    /**
     * Generator of unique IDs for IPort objects.
     * 
     * @author xadazim
     */
    enum PortIdGenerator {
        INSTANCE;
        private int nextId=0;

        /**
         * Generates new unique id.
         * @return id
         */
        public int getNextId() {
            synchronized (INSTANCE) {
                nextId++;
                return nextId;
            }
        }
    }

    /**
     * Gets port identifier.
     * Each port object should have a unique identifier
     * generated with PortIdGenerator.
     * @return port identifier
     */
    Integer getPortIdentifier();

    /**
     * Gets port name.
     * E.g. COM1, USB0.
     * @return returns port name.
     */
    String getPortName();

    /**
     * Will open port for communication.
     * 
     * @throws PortException
     *             When I/O error occurs during port opening.
     */
    void open() throws PortException;

    /**
     * Will close communication port.
     * 
     * @throws PortException
     *             When I/O error occurs during port closing.
     */
    void close() throws PortException;

    /**
     * @return True if communication port is opened, false otherwise.
     */
    boolean isOpen();


}
