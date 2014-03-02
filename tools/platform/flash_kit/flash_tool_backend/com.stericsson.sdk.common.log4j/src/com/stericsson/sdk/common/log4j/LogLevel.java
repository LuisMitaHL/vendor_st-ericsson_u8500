package com.stericsson.sdk.common.log4j;

import java.util.Locale;

import org.apache.log4j.Level;

/**
 * Defines a custom log level
 * 
 */
public class LogLevel extends Level {

    /**
     * 
     */
    private static final long serialVersionUID = 5680626549658023725L;

    /**
     * SYSLOG level integer value
     */
    public static final int SYSLOG_INT = Level.FATAL_INT - 1;

    private static String syslogStr = "SYSLOG";

    /**
     * SYSLOG log level
     */
    public static final LogLevel SYSLOG = new LogLevel(SYSLOG_INT, syslogStr, 7);

    /**
     * @param level
     *            - level
     * @param strLevel
     *            - String level
     * @param syslogEquiv
     *            - syslogEquiv
     */
    protected LogLevel(int level, String strLevel, int syslogEquiv) {

        super(level, strLevel, syslogEquiv);
    }

    /**
     * Convert the String argument to a level. If the conversion fails then this method returns
     * {@link #SYSLOG}.
     */
    /**
     * @param sArg
     *            - string arg
     * @return - Level
     */
    public static Level toLevel(String sArg) {
        return (Level) toLevel(sArg, LogLevel.SYSLOG);
    }

    /**
     * Convert the String argument to a level. If the conversion fails, return the level specified
     * by the second argument, i.e. defaultValue.
     */
    /**
     * @param sArg
     *            - string arg
     * @param defaultValue
     *            - default value
     * @return Level
     */
    public static Level toLevel(String sArg, Level defaultValue) {
        if (sArg == null) {
            return defaultValue;
        }
        String stringVal = sArg.toUpperCase(Locale.getDefault());
        if (stringVal.equals(syslogStr)) {
            return LogLevel.SYSLOG;
        }
        return Level.toLevel(sArg, (Level) defaultValue);
    }

    /**
     * Convert an integer passed as argument to a level. If the conversion fails, then this method
     * returns {@link #DEBUG}.
     * */
    /**
     * @param i
     *            - integer
     * @return Level
     * @throws IllegalArgumentException
     *             - when exeption
     */
    public static Level toLevel(int i) throws IllegalArgumentException {
        if (i == SYSLOG_INT) {
            return LogLevel.SYSLOG;
        } else {
            return Level.toLevel(i);
        }
    }

}
