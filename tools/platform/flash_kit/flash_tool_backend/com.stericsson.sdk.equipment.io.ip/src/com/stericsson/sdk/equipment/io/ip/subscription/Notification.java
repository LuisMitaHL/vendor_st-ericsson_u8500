package com.stericsson.sdk.equipment.io.ip.subscription;

/**
 * This class represents a notification. There are (so far) two types of notification: - An ME is
 * dumping or - A core dump has been automatically downloaded.
 *
 * @author qkarhed
 */
public enum Notification {

    /**
     * Notification representing that an Mobile Equipment (ME) is dumping.
     */
    DETECT_DUMPING_ME,

    /**
     * Notification representing that a core dump has automatically finished being downloaded.
     */
    AUTOMATIC_COREDUMP_DOWNLOADED;

}
