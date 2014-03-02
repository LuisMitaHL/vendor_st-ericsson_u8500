package com.stericsson.sdk.equipment.io.usb.internal;

import java.io.IOException;

/**
 * @author emicroh
 *
 */
//FIXME - should extends non runtime exception and appropriate handling around closing port, deregistering device have to be taken
public class IOTimeoutException extends IOException { 

    /** */
    private static final long serialVersionUID = -6113955953994692875L;

    /**
     * Constructs a new exception with the specified detail message.
     * 
     * @param msg Detail message describing root cause for this exception.
     */
    public IOTimeoutException(String msg) {
        super(msg);
    }
}
