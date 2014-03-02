package com.stericsson.sdk.loader.communication.test;

import junit.framework.TestCase;

import com.stericsson.sdk.loader.communication.LCDefinitions;
import com.stericsson.sdk.loader.communication.internal.LCTimeouts;

/**
 * @author xdancho
 *
 */
public class LCTimeoutsTest extends TestCase {

    /**
     * test timouts
     * @Test
     */
    public void testTimeouts() {
 
        LCTimeouts timouts = LCTimeouts.getInstance();

        timouts.setTimeouts(LCDefinitions.BULK_CMD_RECEIVED, 1);
        timouts.setTimeouts(LCDefinitions.BULK_DATA_RECEIVED, 2);
        timouts.setTimeouts(LCDefinitions.BULK_SESSION_END, 3);
        timouts.setTimeouts(LCDefinitions.COMMAND_ACK, 4);
        timouts.setTimeouts(LCDefinitions.COMMAND_GR, 5);

        assertEquals(1, timouts.getTimeouts(LCDefinitions.BULK_CMD_RECEIVED));
        assertEquals(2, timouts.getTimeouts(LCDefinitions.BULK_DATA_RECEIVED));
        assertEquals(3, timouts.getTimeouts(LCDefinitions.BULK_SESSION_END));
        assertEquals(4, timouts.getTimeouts(LCDefinitions.COMMAND_ACK));
        assertEquals(5, timouts.getTimeouts(LCDefinitions.COMMAND_GR));

    }
}
