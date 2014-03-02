package com.stericsson.sdk.ftp.communication.test;

import java.util.Hashtable;

import junit.framework.TestSuite;

import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;

import com.stericsson.sdk.ftp.communication.test.internal.FTPCommunicationServiceTest;

/**
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

        TestSuite testSuiteService = new TestSuite();

        testSuiteService.addTest(new FTPCommunicationServiceTest());

        Hashtable<String, String> properties = new Hashtable<String, String>();
        properties.put("bundle.name", "com.stericsson.sdk.ftp.communication");
        context.registerService(TestSuite.class.getName(), testSuiteService, properties);

    }

    /**
     * {@inheritDoc}
     */
    public void stop(BundleContext context) throws Exception {
    }

}
