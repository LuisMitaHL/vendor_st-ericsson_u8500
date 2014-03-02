package com.stericsson.sdk.equipment.io.usb.test;

import java.util.Hashtable;

import junit.framework.TestSuite;

import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;

import com.stericsson.sdk.equipment.io.usb.test.internal.PortProviderTest;

/**
 * @author rohacmic
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
        TestSuite testSuiteService = new TestSuite();

        testSuiteService.addTest(new PortProviderTest());

        Hashtable<String, String> properties = new Hashtable<String, String>();
        properties.put("bundle.name",
                "com.stericsson.sdk.equipment.io.usb");
        context.registerService(TestSuite.class.getName(), testSuiteService,
                properties);
    }

    /**
     * {@inheritDoc}
     */
    public void stop(BundleContext context) throws Exception {
    }

}
