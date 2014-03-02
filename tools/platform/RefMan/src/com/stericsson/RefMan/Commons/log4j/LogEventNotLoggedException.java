/**
 * Copyright ST-Ericsson 2010. All rights reserved.
 */

package com.stericsson.RefMan.Commons.log4j;

/**
 * @author qfrelun
 *
 */
public class LogEventNotLoggedException extends Error {
    /**
     *
     */
    private static final long serialVersionUID = 8475958861107298372L;

    /**
     *
     */
    protected LogEventNotLoggedException() {
    }

    /**
     * @param arg0
     */
    public LogEventNotLoggedException(String arg0) {
        super("A required log event of type '" + arg0 + "' was never logged.");
    }

    /**
     * @param arg0
     */
    protected LogEventNotLoggedException(Throwable arg0) {
        super(arg0);
    }

    /**
     * @param arg0
     * @param arg1
     */
    protected LogEventNotLoggedException(String arg0, Throwable arg1) {
        super(arg0, arg1);
    }

}
