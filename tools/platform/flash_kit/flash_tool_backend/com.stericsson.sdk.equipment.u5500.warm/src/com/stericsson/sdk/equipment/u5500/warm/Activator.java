package com.stericsson.sdk.equipment.u5500.warm;

import java.util.Hashtable;

import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;

import com.stericsson.sdk.equipment.IPlatform;
import com.stericsson.sdk.equipment.io.port.PortDeliveryState;
import com.stericsson.sdk.equipment.u5500.warm.internal.U5500WarmPlatform;

/**
 * Activator for U5500 ME
 * 
 * @author eolabor
 * 
 */
public class Activator implements BundleActivator {

    private static BundleContext bundleContext;

    /**
     * @return Bundle context instance
     */
    public static BundleContext getBundleContext() {
        return bundleContext;
    }

    private static void setBundleContext(BundleContext context) {
        bundleContext = context;
    }

    /**
     * {@inheritDoc}
     */
    public void start(BundleContext context) throws Exception {
        Activator.setBundleContext(context);
        Hashtable<String, String> properties = new Hashtable<String, String>();
        properties.put(U5500WarmPlatform.PLATFORM_PROPERTY, U5500WarmPlatform.PLATFORM);
        properties.put(U5500WarmPlatform.STATE_PROPERTY, PortDeliveryState.WARM.name());
        U5500WarmPlatform platform = new U5500WarmPlatform();
        context.registerService(IPlatform.class.getName(), platform, properties);
    }

    /**
     * {@inheritDoc}
     */
    public void stop(BundleContext context) throws Exception {
    }

}
