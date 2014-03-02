package com.stericsson.sdk.equipment.l9540.test;

import java.util.Hashtable;

import junit.framework.TestCase;
import junit.framework.TestSuite;

import org.apache.log4j.Logger;
import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceReference;

import com.stericsson.sdk.equipment.io.AbstractPort;
import com.stericsson.sdk.equipment.io.port.IPort;
import com.stericsson.sdk.equipment.io.port.PortException;
import com.stericsson.sdk.loader.communication.ILoaderCommunicationService;
import com.stericsson.sdk.loader.communication.internal.ILoaderCommunicationMock;

/**
 * @author mbocek01
 */
public class Activator implements BundleActivator {

    /** Logger instance */
    private static Logger logger = Logger.getLogger(Activator.class.getName());

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

    private String[] testCasesClassesNames =
        new String[] {
            "com.stericsson.sdk.equipment.l9540.test.L9540EquipmentTest",
            "com.stericsson.sdk.equipment.l9540.test.L9540EquipmentWriteRpmbTaskTest",
            "com.stericsson.sdk.equipment.l9540.test.L9540EquipmentSelectLoaderOptionsTaskTest"};

    private static ILoaderCommunicationMock mock = null;

    /**
     * @param ilcm
     *            the mock to set
     */
    private static void setMock(ILoaderCommunicationMock ilcm) {
        Activator.mock = ilcm;
    }

    private static IPort port = new AbstractPort("USB2") {

        public int write(byte[] buffer, int offset, int length, int timeout) throws PortException {
            // TODO Auto-generated method stub
            return 0;
        }

        public int read(byte[] buffer, int offset, int length, int timeout) throws PortException {
            // TODO Auto-generated method stub
            return 0;
        }
    };

    /**
     * @return return the mock interface for use in test cases
     */
    public static ILoaderCommunicationMock getLoaderCommunicationMock() {
        return mock;
    }

    /**
     * @return return a dummy port for use in test cases
     */
    public static IPort getPort() {
        return port;

    }

    /**
     * {@inheritDoc}
     */
    public void start(BundleContext context) throws Exception {
        Activator.setBundleContext(context);
        TestCase testCaseService;
        TestSuite testSuiteService = new TestSuite();

        for (String actualClassName : testCasesClassesNames) {
            Class<?> actualClazz;
            try {
                actualClazz = Class.forName(actualClassName);
                testCaseService = (TestCase) actualClazz.newInstance();
                context.registerService(TestCase.class.getName(), testCaseService, null);
                testSuiteService.addTest(testCaseService);
            } catch (Throwable t) {
                logger.error(t.getMessage());
            }
        }
        Hashtable<String, String> properties = new Hashtable<String, String>();
        properties.put("bundle.name", "com.stericsson.sdk.equipment.l9540");
        context.registerService(TestSuite.class.getName(), testSuiteService, properties);

        ServiceReference[] references =
            Activator.getBundleContext().getServiceReferences(ILoaderCommunicationService.class.getName(),
                "(type=mock)");
        Object service = Activator.getBundleContext().getService(references[0]);
        setMock((ILoaderCommunicationMock) service);
    }

    /**
     * {@inheritDoc}
     */
    public void stop(BundleContext context) throws Exception {
    }

}
