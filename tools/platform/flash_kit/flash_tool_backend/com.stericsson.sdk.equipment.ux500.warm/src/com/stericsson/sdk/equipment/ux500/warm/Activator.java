package com.stericsson.sdk.equipment.ux500.warm;

import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;

/**
 * Activator class for warm equipment
 * 
 * @author esrimpa
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
    }

    /**
     * {@inheritDoc}
     */
    public void stop(BundleContext context) throws Exception {
        setBundleContext(null);
    }

}
