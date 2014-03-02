package com.stericsson.sdk.equipment.io.ip.subscription;

import java.io.BufferedWriter;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.Socket;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import org.apache.log4j.Logger;

/**
 * This class represents a Subscription - notification service.
 * 
 * The flash tool backend provides a possibility for any tool to subscribe for notifications. The
 * remote protocol set_subscription command takes the IP address and port the external (remote) tool
 * want to be notified on. When a dumping ME is connected, the flash tool backend sends out a
 * notification to all subscribing tools. That's handled in this class.
 * 
 * @author qkarhed
 */
public final class SubscriptionNotificationService implements IDumpEventListener {

    static final Logger log = Logger.getLogger(SubscriptionNotificationService.class);

    private static final SubscriptionNotificationService INSTANCE = new SubscriptionNotificationService();

    private ArrayList<Subscription> subscriptionList;

    /**
     * @return Instance of this singleton.
     */
    public static SubscriptionNotificationService getInstance() {
        return INSTANCE;
    }

    /**
     * Private constructor
     */
    private SubscriptionNotificationService() {
        subscriptionList = new ArrayList<Subscription>();
    }

    /**
     * Return the entire subscription list
     * 
     * @return the subscriptionList
     */
    public ArrayList<Subscription> getSubscriptionList() {
        synchronized (this) {
            ArrayList<Subscription> subscriptionListCopy = new ArrayList<Subscription>(subscriptionList);
            return subscriptionListCopy;
        }
    }

    /**
     * Add a subscription to the subscription list.
     * 
     * @param subscription
     *            the subscription to be added.
     */
    public void addSubscription(Subscription subscription) {
        if (subscription != null) {
            synchronized (this) {
                this.subscriptionList.add(subscription);
            }
        } else {
            log.error("Trying to add a null subscription");
        }
    }

    /**
     * Remove a subscription from the list.
     * 
     * @param ip
     *            IP address of the subscription to remove.
     * @param port
     *            Port of the subscription to remove.
     * @param type
     *            Type of the subscription to remove.
     */
    public void removeSubscription(InetAddress ip, int port, Notification type) {
        Subscription s = getSubscription(ip, port, type);
        if (s != null) {
            synchronized (this) {
                subscriptionList.remove(s);
            }
        }
    }

    /**
     * Return the subscription corresponding to a given IP address, port and subscription type. If
     * no corresponding subscription is found, null is returned.
     * 
     * @param ip
     *            The IP address of the wanted subscription.
     * @param port
     *            The port of the wanted subscription.
     * @param type
     *            The subscription type of the wanted subscription.
     * @return the wanted subscription, null if not found.
     */
    public Subscription getSubscription(InetAddress ip, int port, Notification type) {
        synchronized (this) {
            for (Subscription s : subscriptionList) {
                InetAddress subscriptionIp = s.getIpAddress();
                int subscriptionPort = s.getPort();
                Notification subscriptionType = s.getType();
                if ((subscriptionIp.equals(ip)) && (subscriptionPort == port) && (subscriptionType.equals(type))) {
                    return s;
                }
            }
        }
        return null;
    }

    /**
     * Given a certain notification type, return all subscriptions in the subscription list which
     * are added with this type.
     * 
     * @param type
     *            the type to fetch all subscriptions for.
     * @return a list of all subscriptions added for the given type.
     */
    public ArrayList<Subscription> getSubscriptionsWithType(Notification type) {
        ArrayList<Subscription> subscriptions = new ArrayList<Subscription>();
        synchronized (this) {
            for (Subscription s : subscriptionList) {
                Notification subscriptionType = s.getType();
                if (subscriptionType.equals(type)) {
                    subscriptions.add(s);
                }
            }
        }
        return subscriptions;
    }

    /**
     * Goes through the list of subscriptions, and for each subscription that matches this
     * subscriptionType, tries to connect to that subscriber (IP and port provided in the
     * subscription).
     * 
     * If it's not possible to connect, the subscription corresponding to that IP and port is
     * removed from the list.
     * 
     * @param subscriptionType
     *            the type of subscription for which to notify.
     * @param info
     *            info about the notification, such as for instance an ID of the dumping device.
     * 
     */
    public void notifySubscribers(Notification subscriptionType, String info) {
        Socket socket = null;
        PrintWriter out = null;

        synchronized (this) {
            for (Iterator<Subscription> i = subscriptionList.iterator(); i.hasNext();) {
                Subscription subscription = i.next();
                if (subscription.getType() == subscriptionType) {
                    try {
                        socket = new Socket(subscription.getIpAddress(), subscription.getPort());
                        if (socket != null) {
                            out =
                                new PrintWriter(new BufferedWriter(new OutputStreamWriter(socket.getOutputStream())),
                                    true);
                            // Inform subscriber of id of dumping ME and /or location of dumps..
                            out.println(info);
                        }
                    } catch (IOException ioe) {
                        log.error("No subscriber listening for type: " + subscriptionType + " @ "
                            + subscription.getIpAddress().toString() + ":" + subscription.getPort());
                        // Could not connect, remove this subscription!
                        i.remove();
                    } finally {
                        if (socket != null) {
                            closeSocket(socket);
                        }
                    }
                }
            }
        }
    }

    /**
     * Return a list of all subscriptions a certain subscriber has.
     * 
     * @param ip
     *            The IP address of the subscriber for which to list all subscriptions.
     * @param port
     *            The port of the subscriber for which to list all subscriptions.
     * @return subscriptions A list of subscriptions.
     */
    public List<Subscription> getSubscriptionsForSubscriber(InetAddress ip, int port) {
        List<Subscription> subscriptions = new ArrayList<Subscription>();
        synchronized (this) {
            for (Subscription s : subscriptionList) {
                int subscriptionPort = s.getPort();
                InetAddress subscriptionIP = s.getIpAddress();
                if ((subscriptionPort == port) && (subscriptionIP.equals(ip))) {
                    subscriptions.add(s);
                }
            }
        }
        return subscriptions;
    }

    /**
     * {@inheritDoc}
     */
    public void dumpingMEdetected(DumpEvent e) {
        String info =
            "Notfication type: " + Notification.DETECT_DUMPING_ME + " Equipment: " + e.getEquipmentId()
                + " core dump path: " + e.getDumpPath();
        notifySubscribers(Notification.DETECT_DUMPING_ME, info);
    }

    private void closeSocket(Socket socket) {
        try {
            socket.close();
        } catch (IOException ioe) {
            log.error(ioe.getMessage());
        }
    }
}
