/**
 * Copyright ST-Ericsson 2010. All rights reserved.
 */

package com.stericsson.RefMan.Commons.log4j;

import java.util.Vector;

import org.apache.log4j.Level;
import org.apache.log4j.spi.LoggingEvent;

/**
 * A class that tracks log entries written with the log4j framework. It can be
 * used during testing, to check if a certain log entry has been logged or not.
 *
 * Create an instance of this class in a testcase, execute some code which logs
 * using the log4j framework, then call the assert methods to check for the
 * presence of various log entries.
 *
 * A recommended pattern is to ensure the instance of the LogTracker is declared
 * as a field and instantiated in the setUp() method of the jUnit test class.
 * Also ensure the instance is close()d in the tearDown() method.
 *
 * This class does not take into account if logging is done from different
 * threads. Only one instance of the LogTracker should be running at any time.
 *
 */
public class LogTracker {

    /**
     * This holds a copy of the logging events that has been retrieved from the
     * LogTrackerAppender. The list is retrieved from the LogTrackerAppender
     * upon the first invocation of any of the assertXXX methods.
     */
    private Vector<LoggingEvent> events;

    /**
     * Constructor
     */
    public LogTracker() {
        LogTrackerAppender.getEvents(); // resets the Appender list

    }

    /**
     * Assert that at least one log entry with the desired Level has been
     * logged.
     *
     * @param l
     *            The desired Level
     */
    public void assertEvent(Level l) {
        obtainEvents();
        for (LoggingEvent e : events) {
            if (e.getLevel().equals(l)) {
                return;
            }
        }
        throw new LogEventNotLoggedException("level=" + l.toString());
    }

    /**
     * Assert that no log entry with the desired Level has been logged.
     *
     * @param l
     *            The desired Level
     */
    public void assertNotEvent(Level l) {
        obtainEvents();
        for (LoggingEvent e : events) {
            if (e.getLevel().equals(l)) {
                throw new LogEventWasLoggedException("level=" + l.toString(), e);
            }
        }

        return;
    }

    /**
     * Assert that at least one log entry with the desired template message has
     * been logged.
     *
     * @param templateMessage
     *            The desired template message to validate against. If the
     *            template contains {} at any point, then the logged message may
     *            contain any sequence of characters at that location.
     */
    public void assertEvent(String templateMessage) {
        obtainEvents();
        for (LoggingEvent e : events) {
            if (matches((String) e.getMessage(), templateMessage)) {
                return;
            }
        }
        throw new LogEventNotLoggedException("message=" + templateMessage);
    }

    /**
     * Assert that no log entry with the desired template message has been
     * logged.
     *
     * @param templateMessage
     *            The desired template message to validate against. If the
     *            template contains {} at any point, then the logged message may
     *            contain any sequence of characters at that location.
     */
    public void assertNotEvent(String templateMessage) {
        obtainEvents();
        for (LoggingEvent e : events) {
            if (matches((String) e.getMessage(), templateMessage)) {
                throw new LogEventWasLoggedException("message="
                        + templateMessage, e);
            }
        }

        return;
    }

    /**
     * Assert that at least one log entry from the desired class has been
     * logged.
     *
     * @param clazz
     *            The desired class
     */
    public void assertEvent(Class<?> clazz) {
        obtainEvents();
        for (LoggingEvent e : events) {
            String eventClass = e.getLoggerName();
            if (eventClass.equals(clazz.getName())) {
                return;
            }
        }
        throw new LogEventNotLoggedException("class=" + clazz.getName());
    }

    /**
     * Assert that no log entry from the desired class has been logged.
     *
     * @param clazz
     *            The desired class
     */
    public void assertNotEvent(Class<?> clazz) {
        obtainEvents();
        for (LoggingEvent e : events) {
            String eventClass = e.getLoggerName();
            if (eventClass.equals(clazz.getName())) {
                throw new LogEventWasLoggedException(
                        "class=" + clazz.getName(), e);
            }
        }
        return;
    }

    /**
     * Ensures that the events field is initialized once.
     */
    private void obtainEvents() {
        if (events == null) {
            events = LogTrackerAppender.getEvents();
        }
    }

    /**
     * Checks if a message matches a template message.
     *
     * @param message
     *            The message
     * @param template
     *            The template message. If the template contains {} at any
     *            point, then the logged message may contain any sequence of
     *            characters at that location.
     * @return
     */
    boolean matches(String message, String template) {
        String regexp = template.replace("{}", "[^\\s].+[^\\s]");
        return message.matches(regexp);
    }

    /**
     * Reset the LogTracker. This allows new code to be executed and a new set
     * of log entries to be checked.
     */
    public void continueTracking() {
        events = null;
    }

    /**
     * Close the LogTracker.
     */
    public void close() {
        // Does nothing at this stage.
        events = null;
    }

}
