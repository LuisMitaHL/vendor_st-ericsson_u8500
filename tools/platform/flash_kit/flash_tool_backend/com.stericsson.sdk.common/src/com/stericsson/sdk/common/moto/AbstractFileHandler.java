package com.stericsson.sdk.common.moto;

import java.io.File;
import java.io.IOException;

/**
 * Common parent for various file handlers capable to load content of related files and provide
 * their payloads for signing purpose.
 * 
 * @author emicroh
 */
public abstract class AbstractFileHandler {

    /** Input file handled by this handler. */
    protected File inputFile;

    /**
     * @param input
     *            Input file to be handled by this handler.
     */
    public AbstractFileHandler(File input) {
        this.inputFile = input;
    }

    /**
     * Perform input file loading.
     * 
     * @throws IOException
     *             When I/O operation fails during input file loading.
     */
    public abstract void load() throws IOException;

    // FIXME - we cannot be 100% sure that have enough RAM to return entire payload as byte array!
    /**
     * @return Payload in desired form.
     * 
     * @throws IOException
     *             When I/O operation fails during fetching payload.
     */
    public abstract byte[] getPayload() throws IOException;

    // public abstract COPSHeaderFields getCOPSHeaderFields();
}
