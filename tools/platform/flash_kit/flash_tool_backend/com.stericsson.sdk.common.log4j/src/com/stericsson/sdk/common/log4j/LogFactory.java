package com.stericsson.sdk.common.log4j;

import org.apache.log4j.Logger;

/**
 * Log factory receives a message and creates a new formatted syslog message
 * 
 */
public final class LogFactory {

    private static final Logger log = Logger.getLogger(LogFactory.class.getName());

    private static final String TYPE = "17"; // Quality of service

    private static final String VERSION = "1.0";

    private static final int RANGE_MIN = 7000;

    private static final int RANGE_MAX = 7499;

    private LogFactory() {

    }



    /**
     * Creates a formatted syslog and log it to the syslog server
     * 
     * @param applicationID
     *            id
       */
    public static void sysLogCreator(int applicationID) {
         // [Type][Application][Version]<Content>
        String id = "";

        id = Integer.toString(applicationID);

        if (applicationID < RANGE_MIN || applicationID > RANGE_MAX) {
            log.error("Application ID must be " + RANGE_MIN + " <= x <= " + RANGE_MAX);
            return;
        }

        StringBuilder sysLogMsg = new StringBuilder();
        sysLogMsg.append("[");
        sysLogMsg.append(TYPE);
        sysLogMsg.append("]");
        sysLogMsg.append("[");
        sysLogMsg.append(id);
        sysLogMsg.append("]");
        sysLogMsg.append("[");
        sysLogMsg.append(VERSION);
        sysLogMsg.append("]");
        sysLogMsg.append(":");
//        if (content != null) {
//
//            sysLogMsg.append(fixSyslogPrint(content.toString()));
//        }

        log.log(LogLevel.SYSLOG, sysLogMsg);
    }

//    private static String fixSyslogPrint(String msg) {
//
//        // remove the record name + space
//        StringBuilder output = new StringBuilder(msg);
//        output.delete(0, msg.indexOf(" ") + 1);
//
//        // remove '"' and add ','
//
//        return output.toString().replaceAll("\"", "").replaceAll(" ", ", ");
//
//    }

}
