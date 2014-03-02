/**
 * Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.Exception;

import java.io.IOException;

/**
 * @author Thomas Palmqvist
 * 
 */
public class DirectoryNotFoundException extends IOException {

    /** The version of this class with consideration of serialization. */

    private static final long serialVersionUID = 1L;

    /**
     * Constructs an <code>DirectoryNotFoundException</code> with
     * <code>null</code> as its error detail message.
     */
    public DirectoryNotFoundException() {
        super();
    }

    /**
     * Constructs an <code>DirectoryNotFoundException</code> with the specified
     * detail message. The error message string <code>s</code> can later be
     * retrieved by the <code>{@link java.lang.Throwable#getMessage}</code>
     * method of class <code>java.lang.Throwable</code>.
     * 
     * @param message
     *            the detail message.
     */
    public DirectoryNotFoundException(String message) {
        super(message);
    }
}
