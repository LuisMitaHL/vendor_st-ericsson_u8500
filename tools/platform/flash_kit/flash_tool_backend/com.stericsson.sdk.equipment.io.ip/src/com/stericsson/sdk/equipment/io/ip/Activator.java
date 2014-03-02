package com.stericsson.sdk.equipment.io.ip;

import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;
import org.osgi.util.tracker.ServiceTracker;

import com.stericsson.sdk.common.configuration.IConfigurationServiceListener;
import com.stericsson.sdk.equipment.io.ip.dns.DNSBroadcastHandler;
import com.stericsson.sdk.equipment.io.ip.subscription.AutomaticTaskHandler;
import com.stericsson.sdk.equipment.io.ip.subscription.IDumpEventListener;
import com.stericsson.sdk.equipment.io.ip.subscription.SubscriptionNotificationService;

/**
 * @author eolabor
 * 
 */
public class Activator implements BundleActivator {

    private DNSBroadcastHandler broadcastHandler;

    private static BundleContext bundleContext;

    private static ServiceTracker dumpEventListenerTracker;

    /**
     * @return Bundle context instance
     */
    public static BundleContext getBundleContext() {
        return bundleContext;
    }

    private static void setBundleContext(BundleContext context) {
        bundleContext = context;
    }

    private static void setDumpEventListenerTracker(BundleContext context) {
        dumpEventListenerTracker = new ServiceTracker(context, IDumpEventListener.class.getName(), null);
    }

    /**
     * {@inheritDoc}
     */
    public void start(BundleContext context) throws Exception {
        Activator.setBundleContext(context);
        setDumpEventListenerTracker(context);
        dumpEventListenerTracker.open();
        context.registerService(IDumpEventListener.class.getName(), AutomaticTaskHandler.getInstance(), null);
        context
            .registerService(IDumpEventListener.class.getName(), SubscriptionNotificationService.getInstance(), null);
        broadcastHandler = new DNSBroadcastHandler();
        broadcastHandler.startInterfacePoll();
        context.registerService(IConfigurationServiceListener.class.getName(), broadcastHandler, null);
    }

    /**
     * Returns the ServiceTracker for the IDumpEventListener. It's needed to be able to locate the
     * listener when it's time to notify it of a dump event.
     * 
     * @return the IDumpEventListener tracker.
     */
    public static ServiceTracker getDumpEventListenerTracker() {
        return dumpEventListenerTracker;
    }

    /**
     * {@inheritDoc}
     * 
     * @see org.osgi.framework.BundleActivator#stop(org.osgi.framework.BundleContext)
     */
    public void stop(BundleContext context) throws Exception {
        broadcastHandler.close();
    }
}
