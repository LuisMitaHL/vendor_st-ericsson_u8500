package com.stericsson.sdk.common;

import java.io.File;
import java.io.OutputStream;
import java.io.IOException;
import java.util.List;

import com.stericsson.sdk.common.memory.IPayloadBlock;

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

    /**
     * @return Return software payload blocks for the software image loaded into memory.
     */
    public abstract List<IPayloadBlock> getPayloadBlocks();

    /**
     * Method serves to write dump file with software payload blocks in format known in Platform
     * Assistant.
     * 
     * @param os
     *            OutputStream which should be used for writing software payload blocks dump.
     * 
     * @throws IOException
     *             When I/O operation fails during dump.
     */
    public void writeDumpFile(OutputStream os) throws IOException {
        try {
            for (IPayloadBlock block : getPayloadBlocks()) {
                os.write(block.getPositionInfo());
                os.write(block.getData());
            }
        } finally {
            os.close();
        }
    }

}
