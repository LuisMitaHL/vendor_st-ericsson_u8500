package com.stericsson.sdk.equipment.io.ip.test.subscription;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketTimeoutException;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;

import junit.framework.TestCase;

import org.apache.log4j.Logger;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import com.stericsson.sdk.equipment.io.ip.subscription.DumpEvent;
import com.stericsson.sdk.equipment.io.ip.subscription.Notification;
import com.stericsson.sdk.equipment.io.ip.subscription.Subscription;
import com.stericsson.sdk.equipment.io.ip.subscription.SubscriptionNotificationService;

/**
 *
 * Tests for the SubscriptionNotificationService.
 *
 * @author qkarhed
 *
 */
public class SubscriptionNotificationServiceTest extends TestCase {

    static final Logger log = Logger.getLogger(SubscriptionNotificationServiceTest.class);

    private InetAddress dummySubscriberIp;

    private InetAddress dummySubscriberIp2;

    private int dummySubscriberPort;

    private Notification dummySubscriberType;

    private SubscriptionNotificationService mClassUnderTest;

    // private CountDownLatch subscriberGotNotification;

    private String notificationInfo = "CORE_DUMP:DETECT_DUMPING_ME:<id>";

    /**
     * @see junit.framework.TestCase#setUp() Method which is run before every testcase.
     */
    @Before
    public void setUp() {
        try {
            dummySubscriberIp = InetAddress.getByName("127.0.0.1");
            dummySubscriberIp2 = InetAddress.getByName("6.6.6.6");
        } catch (Exception e) {
            log.error(e.getMessage());
        }
        dummySubscriberPort = 6666;
        dummySubscriberType = Notification.DETECT_DUMPING_ME;

        mClassUnderTest = SubscriptionNotificationService.getInstance();

        Subscription dummySubscription = new Subscription(dummySubscriberIp, dummySubscriberPort, dummySubscriberType);
        mClassUnderTest.addSubscription(dummySubscription);
    }

    /**
     * Make sure a subscription is added to the list of subscriptions.
     */
    @Test
    public void testSetSubscription() {
        Subscription addedSubscription =
            mClassUnderTest.getSubscription(dummySubscriberIp, dummySubscriberPort, dummySubscriberType);

        if (addedSubscription != null) {
            assert (addedSubscription.getIpAddress().equals(dummySubscriberIp));
            assert (addedSubscription.getPort() == dummySubscriberPort);
            assert (addedSubscription.getType() == dummySubscriberType);
        } else {
            fail("Subscription was not added to the list of subscriptions.");
        }
    }

    /**
     * Test to remove a subscription.
     */
    @Test
    public void testRemoveSubscription() {
        mClassUnderTest.removeSubscription(dummySubscriberIp, dummySubscriberPort, dummySubscriberType);
        Subscription removedSubscription =
            mClassUnderTest.getSubscription(dummySubscriberIp, dummySubscriberPort, dummySubscriberType);
        assertNull(removedSubscription);
    }

    /**
     * Set up a dummy Subscriber listening for notifications of the added subscription, and make
     * sure the subscriber gets the notification.
     */
    @Test
    public void testNotifySubscribersSubscriberExist() {
        CountDownLatch subscriberGotNotification = new CountDownLatch(1);
        new DummySubscriber(subscriberGotNotification);
        mClassUnderTest.notifySubscribers(dummySubscriberType, notificationInfo);
        try {
            assertTrue(subscriberGotNotification.await(5, TimeUnit.SECONDS));
        } catch (InterruptedException e) {
            log.error(e.getMessage());
        }
    }

    /**
     * Extra subscriptions with other subscription types are added, just to have more items to
     * iterate over in the list of subscriptions. Make sure the subscriber of the right notification
     * type is the only one getting a notification.
     */
    @Test
    public void testNotifyOneOutOfSeveralSubscribers() {
        try {
            mClassUnderTest.addSubscription(new Subscription(InetAddress.getByName("0.0.0.0"), 0000,
                Notification.AUTOMATIC_COREDUMP_DOWNLOADED));
            mClassUnderTest.addSubscription(new Subscription(InetAddress.getByName("1.1.1.1"), 1111,
                Notification.AUTOMATIC_COREDUMP_DOWNLOADED));
        } catch (Exception e) {
            log.error(e.getMessage());
        }
        CountDownLatch subscriberGotNotification = new CountDownLatch(1);
        new DummySubscriber(subscriberGotNotification);
        mClassUnderTest.notifySubscribers(dummySubscriberType, notificationInfo);
        try {
            assertTrue(subscriberGotNotification.await(3, TimeUnit.SECONDS));
        } catch (InterruptedException e) {
            log.error(e.getMessage());
        }
    }

    /**
     * Test that when a notification is sent, which there is no longer any subscribers listening
     * for, the subscription corresponding to that notification is removed from the list of
     * subscriptions.
     *
     * This happens if, for instance, a Subscriber stops listening on the specific port, so that the
     * SubscriptionNotificationService can't connect to it. If subscriber, for any reason, stops
     * listening, its subscription will be removed, and the subscriber will then have to set up a
     * new subscription if subscriber want it to continue.
     */
    @Test
    public void testNotifyWhenSubscriberNotListening() {
        // We know there are no subscribers listening for this subscription. (Since we haven't set
        // up any).
        mClassUnderTest.addSubscription(new Subscription(dummySubscriberIp2, 6668, Notification.DETECT_DUMPING_ME));

        // However, do set up a subscriber for another subscription.
        CountDownLatch subscriberGotNotification = new CountDownLatch(1);
        new DummySubscriber(subscriberGotNotification);
        mClassUnderTest.notifySubscribers(dummySubscriberType, notificationInfo);

        // And verify that subscriber gets its notification.
        try {
            assertTrue(subscriberGotNotification.await(3, TimeUnit.SECONDS));
        } catch (InterruptedException e) {
            log.error(e.getMessage());
        }

        // Verify that the subscription without a subscriber is removed.
        ArrayList<Subscription> subscriptions = mClassUnderTest.getSubscriptionsWithType(dummySubscriberType);
        for (Subscription s : subscriptions) {
            if (s.getIpAddress().equals(dummySubscriberIp2)) {
                fail(dummySubscriberIp2.toString()
                    + " is not listening any longer, but its subscription was not removed");
            }
        }
    }

    /**
     * Test iterating through the whole list and not finding any subscribers for the specific
     * notification.
     */
    @Test
    public void testNotifyWhenNoSubscribers() {
        mClassUnderTest.notifySubscribers(Notification.AUTOMATIC_COREDUMP_DOWNLOADED, "Test");
    }

    /**
     * Test get a list of all subscriptions of a specified type.
     */
    @Test
    public void testGetSubscriptionsOfCertainType() {
        try {
            mClassUnderTest.addSubscription(new Subscription(InetAddress.getByName("0.0.0.0"), 0000,
                Notification.AUTOMATIC_COREDUMP_DOWNLOADED));
            mClassUnderTest.addSubscription(new Subscription(InetAddress.getByName("1.1.1.1"), 1111,
                Notification.AUTOMATIC_COREDUMP_DOWNLOADED));
        } catch (Exception e) {
            log.error(e.getMessage());
        }
        ArrayList<Subscription> subscriptions =
            mClassUnderTest.getSubscriptionsWithType(Notification.AUTOMATIC_COREDUMP_DOWNLOADED);
        assertTrue(subscriptions.size() == 2);
    }

    /**
     * Test get subscriptions for a given subscriber
     */
    @Test
    public void testGetSubscriptions() {
        InetAddress subscriberIP = null;
        int port = 7777;
        try {
            subscriberIP = InetAddress.getByName("7.7.7.7");
        } catch (UnknownHostException e) {
            log.error(e.getMessage());
        }
        mClassUnderTest.addSubscription(new Subscription(subscriberIP, port, Notification.DETECT_DUMPING_ME));
        mClassUnderTest
            .addSubscription(new Subscription(subscriberIP, port, Notification.AUTOMATIC_COREDUMP_DOWNLOADED));
        List<Subscription> subscriptions = mClassUnderTest.getSubscriptionsForSubscriber(subscriberIP, port);

        assertNotNull(subscriptions);
        assertEquals(subscriptions.size(), 2);
        assertEquals(subscriptions.get(0).getPort(), port);
    }

    /**
     * Test add null subscription.
     *
     * @throws IOException
     *             read error
     */
    @Test
    public void testAddNullSubscription() throws IOException {
        int startSize = mClassUnderTest.getSubscriptionList().size();
        mClassUnderTest.addSubscription(null);
        int endSize = mClassUnderTest.getSubscriptionList().size();
        assertEquals(startSize, endSize);
    }

    /**
     * Test dump event received.
     */
    @Test
    public void testDumpingMEdetected() {
        CountDownLatch subscriberGotNotification = new CountDownLatch(1);
        new DummySubscriber(subscriberGotNotification);
        DumpEvent de = new DumpEvent("info", "dummyPath");
        mClassUnderTest.dumpingMEdetected(de);
        try {
            assertTrue(subscriberGotNotification.await(3, TimeUnit.SECONDS));
        } catch (InterruptedException e) {
            log.error(e.getMessage());
        }

    }

    /**
     * (non-Javadoc)
     *
     * @see junit.framework.TestCase#tearDown() Method called after each testcase is run.
     *
     */
    @After
    public void tearDown() {
        mClassUnderTest.removeSubscription(dummySubscriberIp, dummySubscriberPort, dummySubscriberType);
        dummySubscriberIp = null;
        mClassUnderTest = null;
    }

    /**
     * Help-class DummySubscriber.
     */
    public class DummySubscriber {

        private CountDownLatch countDownLatch;

        /**
         * Constructor
         *
         * @param cdl
         *            CountDownLatch
         */
        public DummySubscriber(CountDownLatch cdl) {
            countDownLatch = cdl;
            setUpDummySubscriber();
            try {
                Thread.sleep(500);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        /**
         * Set up a dummySubscriber in a new thread to listen for connection on the
         * dummySubscriberPort.
         */
        public void setUpDummySubscriber() {
            new Thread(new Runnable() {
                public void run() {
                    boolean connected = false;
                    ServerSocket dummySubscriber = null;
                    Socket subscriberSocket = null;
                    BufferedReader in = null;
                    String info = null;
                    dummySubscriber = createServerSocket();
                    while (!connected && dummySubscriber != null) {
                        try {
                            subscriberSocket = dummySubscriber.accept();
                        } catch (SocketTimeoutException ste) {
                            log.error("DummySubscriber timed out, not listening any longer");
                        } catch (IOException e) {
                            log.error("DummySubscriber accept failed");
                        }

                        if (subscriberSocket != null && subscriberSocket.isConnected()) {
                            // Should receive info-string.
                            connected = true;
                            try {
                                in =
                                    new BufferedReader(
                                        new InputStreamReader(subscriberSocket.getInputStream(), "UTF-8"));
                                info = in.readLine();
                            } catch (IOException e) {
                                log.error(e.getMessage());
                            }
                            if (info != null) {
                                log.info("Subscriber got: " + info);
                                countDownLatch.countDown();
                            } else {
                                log.error("Subscriber could not read info from notification.");
                            }
                        }
                    }
                    try {
                        if (subscriberSocket != null) {
                            subscriberSocket.close();
                        }
                        if (dummySubscriber != null) {
                            dummySubscriber.close();
                        }
                        dummySubscriber = null;
                    } catch (IOException e) {
                        log.error("Exception when closing dummySubscriber and/or socket: " + e.getMessage());
                    }
                }
            }, "dummySubscriber").start();
        }

        private ServerSocket createServerSocket() {
            ServerSocket dummySubscriber = null;
            try {
                dummySubscriber = new ServerSocket(dummySubscriberPort);
            } catch (IOException ioe) {
                log.error("Failed to listen on port: " + dummySubscriberPort);
            }
            return dummySubscriber;
        }
    }
}
