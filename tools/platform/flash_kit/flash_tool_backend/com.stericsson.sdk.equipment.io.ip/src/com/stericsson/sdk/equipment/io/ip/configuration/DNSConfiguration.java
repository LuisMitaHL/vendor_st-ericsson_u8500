/**
 *
 */
package com.stericsson.sdk.equipment.io.ip.configuration;

/**
 * Configuration class for mDNS Broadcast listener.
 *
 * @author eolabor
 *
 */
public final class DNSConfiguration {

    /**
     * Broadcast name when me is connected
     */
    public static final String AVAILABLE_ME = "READY";

    /**
     * Broadcast name when me is dumping
     */
    public static final String DUMPING_ME = "DUMPME";

    /**
     * Type of Broadcast to listen for
     */
    public static final String SERVICE_TYPE = "_mobile._tcp.local.";

    /**
     * NetworkInterface poll interval
     */
    public static final int POLL_INTERVAL = 10000;

    /**
     * USB network interface search string Windows
     */
    public static final String USB_ETH_NAME_WINDOWS = "ndis";

    /**
     * USB network interface search string Linux
     */
    public static final String USB_ETH_NAME_LINUX = "usb0";

    /**
     * Constructor.
     */
    private DNSConfiguration() {
    }

}
