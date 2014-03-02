package com.stericsson.sdk.equipment.io.uart.internal;

import java.io.IOException;


/**
 * @author rohacmic
 *
 */
public class UARTException extends IOException {

    /** */
    private static final long serialVersionUID = 1L;

    /**
     * Constructs a new exception with the specified detail message.
     * 
     * @param msg Detail message describing root cause for this exception.
     */
    public UARTException(String msg) {
        super(msg);
    }
}
