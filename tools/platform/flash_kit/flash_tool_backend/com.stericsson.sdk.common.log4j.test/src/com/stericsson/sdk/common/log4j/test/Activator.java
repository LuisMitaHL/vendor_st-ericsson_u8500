package com.stericsson.sdk.common.log4j.test;

import java.util.Hashtable;

import junit.framework.TestCase;
import junit.framework.TestSuite;

import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;

/**
 * @author xtomlju
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
        TestCase testCaseService;
        TestSuite testSuiteService = new TestSuite();

        testCaseService = new LogLevelTest();
        context
                .registerService(TestCase.class.getName(), testCaseService,
                        null);
        testSuiteService.addTest(testCaseService);

        testCaseService = new LogFactoryTest();
        context
                .registerService(TestCase.class.getName(), testCaseService,
                        null);
        testSuiteService.addTest(testCaseService);

        Hashtable<String, String> properties = new Hashtable<String, String>();
        properties.put("bundle.name", "com.stericsson.sdk.common.log");
        context.registerService(TestSuite.class.getName(), testSuiteService,
                properties);
    }

    /**
     * {@inheritDoc}
     */
    public void stop(BundleContext context) throws Exception {
    }

}
