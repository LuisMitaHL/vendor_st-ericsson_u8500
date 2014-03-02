package com.stericsson.sdk.equipment.io.ip.subscription;

import java.net.InetAddress;

/**
 * This class represents a subscription. A subscription has an IP address and port, which is the IP
 * address and port of the subscriber. A subscription also has a type. Subscribers can subscribe to
 * different notifications, and what differentiates them is the type. Example of Subscription types:
 * DETECT_DUMPING_ME (represented as 0) AUTOMATIC_CORE_DUMP_DOWNLOADED (represented as 1)
 *
 * @author qkarhed
 */
public class Subscription {

    private InetAddress subscriberIP;

    private int subscriberPort;

    private Notification notificationType;

    /**
     * Constructor
     *
     * @param ipAddr
     *            IP address of the subscriber which created this Subscription.
     * @param port
     *            port on which the subscriber listens for notifications.
     * @param type
     *            Type of notification the subscribers subscribe for.
     */
    public Subscription(InetAddress ipAddr, int port, Notification type) {
        setIpAddress(ipAddr);
        setPort(port);
        setType(type);
    }

    /**
     * Returns the IP address.
     *
     * @return the IP address.
     */
    public InetAddress getIpAddress() {
        return subscriberIP;
    }

    /**
     * Sets the IP address.
     *
     * @param ipAddress
     *            the IP address of the subscriber (host) for this subscription.
     */
    public void setIpAddress(InetAddress ipAddress) {
        this.subscriberIP = ipAddress;
    }

    /**
     * Returns the port on which the subscriber of this subscription is listening on.
     *
     * @return the port on which the subscriber of this subscription is listening.
     */
    public int getPort() {
        return subscriberPort;
    }

    /**
     * Sets the port.
     *
     * @param port
     *            the port on which the subscriber of this subscription is listening.
     */
    public void setPort(int port) {
        this.subscriberPort = port;
    }

    /**
     * Returns the type of this subscription = what the subscriber wants notifications for.
     *
     * @return the type of the notifications for this subscription.
     */
    public Notification getType() {
        return notificationType;
    }

    /**
     * Set the type of notifications for this subscription.
     *
     * @param type
     *            the type
     */
    public void setType(Notification type) {
        this.notificationType = type;
    }

}
