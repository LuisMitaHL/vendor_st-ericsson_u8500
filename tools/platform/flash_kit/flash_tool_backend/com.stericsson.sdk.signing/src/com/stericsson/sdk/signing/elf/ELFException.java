package com.stericsson.sdk.signing.elf;

import java.io.IOException;

/**
 * @author etomjun
 */
public class ELFException extends IOException {
    /** */
    private static final long serialVersionUID = 7126911406656626516L;

    /**
     * @param message
     *            TBD
     */
    public ELFException(final String message) {
        super(message);
    }
}
