package com.stericsson.sdk.assembling.cli;

import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.logging.ConsoleHandler;
import java.util.logging.Formatter;
import java.util.logging.Level;
import java.util.logging.LogRecord;
import java.util.logging.Logger;

/**
 * Utility class to setup logging
 * 
 * @author xtomlju
 */
public final class CommandLineLogging {

    /**
     * 
     */
    public static void setupLogger() {

        Logger logger = Logger.getLogger("com.stericsson.sdk");
        logger.setUseParentHandlers(false);
        logger.setLevel(Level.FINEST);

        ConsoleHandler handler = new ConsoleHandler();
        handler.setLevel(Level.SEVERE);
        handler.setFormatter(new Formatter() {

            final SimpleDateFormat format = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss:SSS");

            public String format(LogRecord record) {
                StringBuffer buffer = new StringBuffer();

                buffer.append(format.format(new Date(record.getMillis())));
                buffer.append(" [" + record.getLevel() + "] " + record.getMessage());

                if (record.getLevel().intValue() < Level.INFO.intValue()) {
                    buffer.append(" (" + record.getSourceClassName() + " " + record.getSourceMethodName() + ")");
                }

                buffer.append('\n');

                return buffer.toString();
            }
        });

        handler.close();
        logger.addHandler(handler);
    }

    private CommandLineLogging() {

    }

}
