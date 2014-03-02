package com.stericsson.sdk.equipment.io.port;

import java.io.IOException;

/**
 * @author xtomlju
 */
public class PortException extends IOException {

    /** */
    private static final long serialVersionUID = -2527538149382731683L;

    /**
     * @param message
     *            Exception message
     */
    public PortException(String message) {
        super(message);
    }

    /**
     * @param t
     *            Cause exception.
     */
    public PortException(Throwable t) {
        super();
        initCause(t);
    }

    /**
     * @param message
     *            Exception message.
     * @param t
     *            Cause exception.
     */
    public PortException(String message, Throwable t) {
        super(message);
        initCause(t);
    }
}
