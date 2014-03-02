package com.stericsson.sdk.common.memory;

import java.io.IOException;

/**
 * Exception for failed I/O operation performed on {@link VirtualMemory}.
 * 
 * @author emicroh
 */
public class MemoryIOException extends IOException {

    private static final long serialVersionUID = 2266359153593155251L;

    /**
     * Creates new instance for unspecified cause.
     */
    public MemoryIOException() {
        super("Unexpected memory IO exception!");
    }

    /**
     * Creates new instance from given cause exception.
     * 
     * @param t
     *            Exception which should be initialized as a cause.
     */
    public MemoryIOException(Throwable t) {
        this();
        initCause(t);
    }

    /**
     * Creates new instance and initializes it with given message.
     * 
     * @param msg
     *            Message describing cause of this exception.
     */
    public MemoryIOException(String msg) {
        super(msg);
    }

    /**
     * Creates new instance and initializes with given message and cause.
     * 
     * @param msg
     *            Message describing cause of this exception.
     * @param t
     *            Exception which should be initialized as a cause.
     */
    public MemoryIOException(String msg, Throwable t) {
        super(msg);
        super.initCause(t);
    }
}
