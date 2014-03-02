package com.stericsson.sdk.common.gdfs;

import java.io.IOException;

/**
 * Defines the methods needed for a GDFS data parser.
 */
public interface IGDDataParser {

    /**
     * Reports if there is more data to parse.
     * 
     * @return true if there is more data available.
     * @throws IOException
     *             If the parser fails to check for data availability.
     */
    boolean hasMoreData() throws IOException;

    /**
     * Reads and parses the next block of GDFS data from its source.
     * 
     * @return The parsed block of GDFS data.
     * @throws IOException
     *             If the parser fails to read and parse the data from its source.
     */
    GDData nextData() throws IOException;

    /**
     * Closes the connection to the parser's source.
     * 
     * @throws IOException
     *             if the parser fails to close.
     */
    void close() throws IOException;
}
