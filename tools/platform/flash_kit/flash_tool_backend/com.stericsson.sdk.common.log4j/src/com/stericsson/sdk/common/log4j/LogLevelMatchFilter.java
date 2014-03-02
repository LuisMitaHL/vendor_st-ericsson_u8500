package com.stericsson.sdk.common.log4j;

import org.apache.log4j.Level;
import org.apache.log4j.spi.LoggingEvent;
import org.apache.log4j.varia.LevelMatchFilter;

/**
 * @author xdancho
 * 
 */
public class LogLevelMatchFilter extends LevelMatchFilter {

    Level levelToMatch;

    /**
     * {@inheritDoc}
     * 
     * @see org.apache.log4j.varia.LevelMatchFilter#setLevelToMatch(java.lang.String)
     */
    public void setLevelToMatch(String level) {
        // levelToMatch = OptionConverter.toLevel(level, null);
        if (level == null) {
            levelToMatch = null;
            return;
        }


        int hashIndex = level.indexOf('#');
        if (hashIndex == -1) {
            if ("NULL".equalsIgnoreCase(level)) {
                levelToMatch = null;
            } else {

                levelToMatch = LogLevel.toLevel(level, null);
            }
        }
    }

    /** {@inheritDoc}
     * @see org.apache.log4j.varia.LevelMatchFilter#decide(org.apache.log4j.spi.LoggingEvent)
     */
    public int decide(LoggingEvent event) {

        return event.getLevel() == LogLevel.SYSLOG ? 1 : 0;

    }

}
