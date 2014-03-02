package com.stericsson.sdk.equipment.u8500.warm;

import java.util.Hashtable;

import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;

import com.stericsson.sdk.equipment.IPlatform;
import com.stericsson.sdk.equipment.io.port.PortDeliveryState;
import com.stericsson.sdk.equipment.u8500.warm.internal.U8500WarmPlatform;

/**
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
        properties.put(U8500WarmPlatform.PLATFORM_PROPERTY, U8500WarmPlatform.PLATFORM);
        properties.put(U8500WarmPlatform.STATE_PROPERTY, PortDeliveryState.WARM.name());
        U8500WarmPlatform platform = new U8500WarmPlatform();
        context.registerService(IPlatform.class.getName(), platform, properties);
    }

    /**
     * {@inheritDoc}
     */
    public void stop(BundleContext context) throws Exception {
    }

}
