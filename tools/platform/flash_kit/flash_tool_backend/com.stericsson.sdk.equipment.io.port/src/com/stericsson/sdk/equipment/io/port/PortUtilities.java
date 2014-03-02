package com.stericsson.sdk.equipment.io.port;

/**
 * Contains utility methods for ports.
 * 
 * @author pkutac01
 * 
 */
public final class PortUtilities {

    private PortUtilities() {

    }

    /**
     * Checks whether the provided port is a USB port.
     * 
     * @param port
     *            Port to be checked.
     * @return True if the provided port is a USB port, false otherwise.
     */
    public static boolean isUSBPort(IPort port) {
        if (port == null) {
            throw new NullPointerException();
        }

        if (port.getPortName().contains("USB") && !port.getPortName().contains("ttyUSB")) {
            return true;
        }

        return false;
    }

}
