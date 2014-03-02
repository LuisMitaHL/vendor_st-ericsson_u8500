package com.stericsson.sdk.backend.remote.io;

import java.io.IOException;

/**
 * Interface for a backend client
 * 
 * @author xolabju
 * 
 */
public interface IBackendServerConnection {

    /**
     * Reads a line of text from the client
     * 
     * @return the read line
     * @throws IOException
     *             if the reading fails
     */
    String readLine() throws IOException;

    /**
     * Writes a line of text to the client
     * 
     * @param line
     *            the line to write
     * @throws IOException
     *             if the writing fails
     */
    void write(String line) throws IOException;

    /**
     * 
     * @return the client address
     */
    String getAddress();

    /**
     * Closes the client connection and it's associated streams
     */
    void close();
}
