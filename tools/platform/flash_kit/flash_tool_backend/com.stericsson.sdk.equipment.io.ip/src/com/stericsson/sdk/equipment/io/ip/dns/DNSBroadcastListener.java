package com.stericsson.sdk.equipment.io.ip.dns;

import java.util.regex.PatternSyntaxException;

import javax.jmdns.ServiceEvent;
import javax.jmdns.ServiceListener;

import org.apache.log4j.Logger;

import com.stericsson.sdk.equipment.io.ip.Activator;
import com.stericsson.sdk.equipment.io.ip.configuration.DNSConfiguration;
import com.stericsson.sdk.equipment.io.ip.internal.FtpPortHandler;
import com.stericsson.sdk.equipment.io.ip.subscription.DumpEvent;
import com.stericsson.sdk.equipment.io.ip.subscription.IDumpEventListener;

/**
 * Listens to MDNS broadcast messages.
 *
 * @author eolabor
 */
public class DNSBroadcastListener implements ServiceListener {

    static final Logger log = Logger.getLogger(DNSBroadcastListener.class);

    private FtpPortHandler portHandler = FtpPortHandler.getInstance();

    private static final String BROADCAST_DELIMITER = " ";

    private String targetIP;

    /**
     * {@inheritDoc}
     */
    public void serviceAdded(ServiceEvent event) {
        String broadcastName = getDnsBroadcastName(event.getName());
        if (broadcastName.equalsIgnoreCase(DNSConfiguration.AVAILABLE_ME)
            || broadcastName.equalsIgnoreCase(DNSConfiguration.DUMPING_ME)) {
            log.info("Service added   : " + event.getName() + "." + event.getType());
        }
    }

    /**
     * {@inheritDoc}
     */
    public void serviceRemoved(ServiceEvent event) {
        String broadcastName = getDnsBroadcastName(event.getName());
        if (broadcastName.equalsIgnoreCase(DNSConfiguration.AVAILABLE_ME)) {
            portHandler.removeDevice(event);
            log.info("Service removed : " + event.getName() + "." + event.getType());
        }
    }

    /**
     * {@inheritDoc}
     */
    public void serviceResolved(ServiceEvent event) {
        String broadcastName = getDnsBroadcastName(event.getName());
        targetIP = event.getInfo().getHostAddress();
        if (broadcastName.equalsIgnoreCase(DNSConfiguration.AVAILABLE_ME)) {
            log.info("Service resolved: " + event.getInfo());
            portHandler.addDevice(targetIP);
        } else if (broadcastName.equalsIgnoreCase(DNSConfiguration.DUMPING_ME)) {
            log.info("Service resolved: " + event.getInfo());
            portHandler.dumpingDevice(targetIP);
            int startIndex = event.getInfo().getNiceTextString().indexOf("=") + 1;
            String dumpPath = event.getInfo().getNiceTextString().substring(startIndex);
            fireDumpEvent(new DumpEvent(event.getInfo().getHostAddress(), dumpPath));
        }

    }

    /**
     * @return the targetIP
     */
    public String getTargetIP() {
        return targetIP;
    }

    private String getDnsBroadcastName(String name) {
        String[] mDnsBroadcastName;
        try {
            mDnsBroadcastName = name.split(BROADCAST_DELIMITER);
        } catch (PatternSyntaxException e) {
            return name;
        }
        return mDnsBroadcastName[0];
    }

    /**
     * Notify all IDumpEventListeners that a dump is available.
     *
     * @param e
     *            The dump event
     */
    public void fireDumpEvent(DumpEvent e) {
        if (Activator.getDumpEventListenerTracker() == null) {
            return;
        }
        Object[] listeners = Activator.getDumpEventListenerTracker().getServices();
        if (listeners == null) {
            return;
        }
        for (Object l : listeners) {
            ((IDumpEventListener) l).dumpingMEdetected(e);
        }
    }

}
