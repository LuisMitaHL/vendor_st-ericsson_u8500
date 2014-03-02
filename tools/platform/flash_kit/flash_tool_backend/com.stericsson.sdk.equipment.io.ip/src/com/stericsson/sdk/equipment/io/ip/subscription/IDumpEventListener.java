package com.stericsson.sdk.equipment.io.ip.subscription;

import java.util.EventListener;

/**
 * Listener for dump events.
 * 
 * @author eolabor
 */
public interface IDumpEventListener extends EventListener {

    /**
     * Called when a broadcast of a dumping ME is received.
     * 
     * @param e
     *            the dump event.
     */
    void dumpingMEdetected(DumpEvent e);

}
