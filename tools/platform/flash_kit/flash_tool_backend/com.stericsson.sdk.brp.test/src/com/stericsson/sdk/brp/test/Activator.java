package com.stericsson.sdk.brp.test;

import java.util.Hashtable;

import junit.framework.TestCase;
import junit.framework.TestSuite;

import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;

import com.stericsson.sdk.brp.util.test.Base64Test;

/**
 * 
 * @author xolabju
 * 
 */
public class Activator implements BundleActivator {

    /**
     * {@inheritDoc}
     */
    public void start(BundleContext context) throws Exception {
        TestCase testCaseService;
        TestSuite testSuiteService = new TestSuite();

        testCaseService = new AbstractCommandExecutorTest();
        context.registerService(TestCase.class.getName(), testCaseService, null);
        testSuiteService.addTest(testCaseService);

        testCaseService = new AbstractCommandTest();
        context.registerService(TestCase.class.getName(), testCaseService, null);
        testSuiteService.addTest(testCaseService);

        testCaseService = new CommandFactoryTest();
        context.registerService(TestCase.class.getName(), testCaseService, null);
        testSuiteService.addTest(testCaseService);

        testCaseService = new CommandDescriptionTest();
        context.registerService(TestCase.class.getName(), testCaseService, null);
        testSuiteService.addTest(testCaseService);

        testCaseService = new Base64Test();
        context.registerService(TestCase.class.getName(), testCaseService, null);
        testSuiteService.addTest(testCaseService);

        Hashtable<String, String> properties = new Hashtable<String, String>();
        properties.put("bundle.name", "com.stericsson.sdk.brp");
        context.registerService(TestSuite.class.getName(), testSuiteService, properties);
    }

    /**
     * {@inheritDoc}
     */
    public void stop(BundleContext context) throws Exception {

    }

}
