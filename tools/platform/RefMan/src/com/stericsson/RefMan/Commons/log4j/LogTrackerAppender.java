/**
 * Copyright ST-Ericsson 2010. All rights reserved.
 */

package com.stericsson.RefMan.Commons.log4j;

import java.util.Vector;

import org.apache.log4j.AppenderSkeleton;
import org.apache.log4j.spi.Filter;
import org.apache.log4j.spi.LocationInfo;
import org.apache.log4j.spi.LoggingEvent;

/**
 * A log4j Appender which can be used to snoop logged events, suitable for
 * testing.
 *
 * @author qfrelun
 *
 */
public class LogTrackerAppender extends AppenderSkeleton {
    /**
     * All events logged since last getEvents()
     */
    private static Vector<LoggingEvent> events;

    /**
     * set to true to debug
     */
    private boolean debugOutput = false;

    /**
     * Constructor.
     */
    public LogTrackerAppender() {
        init();
    }

    /**
     * Constructor.
     *
     * @param isActive
     */
    public LogTrackerAppender(boolean isActive) {
        super(isActive);
        init();
    }

    /**
     * Initialize the event list and setup filter.
     *
     */
    private void init() {
        createEvents();

        addFilter(new Filter() {

            @Override
            public int decide(LoggingEvent arg0) {
                return Filter.ACCEPT;
            }
        });
    }

    /**
     * Ensure the eventlist is created.
     */
    static private synchronized Vector<LoggingEvent> createEvents() {
        if (events == null) {
            events = new Vector<LoggingEvent>();
        }

        return events;
    }

    /*
     * (non-Javadoc)
     *
     * @see
     * org.apache.log4j.AppenderSkeleton#append(org.apache.log4j.spi.LoggingEvent
     * )
     */
    @Override
    protected void append(LoggingEvent arg0) {
        debug(arg0);
        synchronized (events) {
            events.add(arg0);
        }

    }

    /**
     * Output every message to stdout
     *
     * @param arg0
     *            The event.
     */
    private void debug(LoggingEvent arg0) {
        if (debugOutput) {
            org.apache.log4j.Level l = arg0.getLevel();
            String message = (String) arg0.getMessage();
            LocationInfo loc = arg0.getLocationInformation();
            String callingClass = loc.getClassName();
            System.out.println("LogTrackerAppender: got loggingEvent " + arg0
                    + " level " + l + " message '" + message + "'" + " class "
                    + callingClass);
        }
    }

    /**
     * Get the list of events logged since the previous call to getEvent().
     * Since this method does not consider which threads that does the logging,
     * log events from all threads are returned.
     *
     * @return The events logged so far.
     */
    public synchronized static Vector<LoggingEvent> getEvents() {
        createEvents();

        Vector<LoggingEvent> toReturn;
        synchronized (events) {
            toReturn = events;
            events = new Vector<LoggingEvent>();
        }
        return toReturn;
    }

    /*
     * (non-Javadoc)
     *
     * @see org.apache.log4j.Appender#close()
     */
    public void close() {
        synchronized (events) {
            events = new Vector<LoggingEvent>();
        }
    }

    /*
     * (non-Javadoc)
     *
     * @see org.apache.log4j.Appender#requiresLayout()
     */
    public boolean requiresLayout() {
        return false;
    }

}
