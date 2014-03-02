package com.stericsson.sdk.equipment.io.port;

/**
 * @author emicroh
 */
public class NativeException extends Exception {

    /** */
    private static final long serialVersionUID = 3533113756957640166L;

    /**
     * Constructs a new exception with the specified detail message.
     * 
     * @param msg Detail message describing root cause for this exception.
     */
    public NativeException(String msg) {
        super(msg);
    }
}
