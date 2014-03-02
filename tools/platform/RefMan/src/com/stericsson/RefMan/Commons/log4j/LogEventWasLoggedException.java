/**
 * Copyright ST-Ericsson 2010. All rights reserved.
 */

package com.stericsson.RefMan.Commons.log4j;

import org.apache.log4j.spi.LoggingEvent;

/**
 * @author qfrelun
 *
 */
public class LogEventWasLoggedException extends Error {
    /**
     *
     */
    private static final long serialVersionUID = -2056565099598244952L;

    /**
     *
     */
    protected LogEventWasLoggedException() {
    }

    /**
     * @param arg0
     */
    protected LogEventWasLoggedException(String arg0) {
        super(arg0);
    }

    /**
     * @param arg0
     */
    protected LogEventWasLoggedException(Throwable arg0) {
        super(arg0);
    }

    /**
     * @param arg0
     * @param arg1
     */
    protected LogEventWasLoggedException(String arg0, Throwable arg1) {
        super(arg0, arg1);
    }

    public LogEventWasLoggedException(String string, LoggingEvent e) {
        super("An unexpected log event of type '" + string + "' was logged as "
                + e.getRenderedMessage() + "\nat "
                + e.getLocationInformation().getClassName() + "("
                + e.getLocationInformation().getFileName() + ":"
                + e.getLocationInformation().getLineNumber() + ")");
    }

}
