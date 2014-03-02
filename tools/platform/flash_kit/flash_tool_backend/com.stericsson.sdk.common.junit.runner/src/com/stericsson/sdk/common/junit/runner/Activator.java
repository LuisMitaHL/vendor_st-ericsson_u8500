package com.stericsson.sdk.common.junit.runner;

import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;

/**
 * @author xtomlju
 */
public class Activator implements BundleActivator {

    private static BundleContext bundleContext;

    private static void setBundleContext(BundleContext context) {
        bundleContext = context;
    }

    /**
     * @return TBD
     */
    public static BundleContext getBundleContext() {
        return bundleContext;
    }

    /**
     * {@inheritDoc}
     */
    public void start(BundleContext context) throws Exception {
        Activator.setBundleContext(context);
        new Runner(context).execute();
    }

    /**
     * {@inheritDoc}
     */
    public void stop(BundleContext context) throws Exception {
    }

}
