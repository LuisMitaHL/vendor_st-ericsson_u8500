package com.stericsson.sdk.assembling.utilities;

import java.io.Closeable;
import java.io.IOException;

/**
 * Utility class for closing streams
 * 
 * @author xolabju
 * 
 */
public final class StreamCloser {

    private StreamCloser() {
    }

    /**
     * 
     * @param closables
     *            the closables to close
     */
    public static void close(Closeable... closables) {
        if (closables != null) {
            for (Closeable c : closables) {
                if (c != null) {
                    try {
                        c.close();
                    } catch (IOException e) {
                        System.err.print("\nException technical details:\n");
                        e.printStackTrace();
                    }
                }
            }
        }
    }

}
