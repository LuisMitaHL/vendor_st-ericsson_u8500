package com.stericsson.sdk.equipment.test;

import java.util.Hashtable;

import junit.framework.TestCase;
import junit.framework.TestSuite;

import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceReference;

import com.stericsson.sdk.equipment.io.AbstractPort;
import com.stericsson.sdk.equipment.io.port.IPort;
import com.stericsson.sdk.equipment.io.port.PortException;
import com.stericsson.sdk.loader.communication.ILoaderCommunicationService;
import com.stericsson.sdk.loader.communication.internal.ILoaderCommunicationMock;

/**
 * @author xtomlju
 */
public class TestFragmentActivator implements BundleActivator {

    private static BundleContext bundleContext;

    private String[] testCasesClassesNames =
        new String[] {
            "com.stericsson.sdk.equipment.io.test.AbstractPortTest",
            "com.stericsson.sdk.equipment.io.test.DeadPortTest",
            "com.stericsson.sdk.equipment.io.test.PortChannelTest",
            "com.stericsson.sdk.equipment.io.test.PortExceptionTest",
            "com.stericsson.sdk.equipment.io.test.AbstractFTPPortTest",
            "com.stericsson.sdk.equipment.io.test.PortUtilitiesTest",
            "com.stericsson.sdk.equipment.bootrom.BootRomTokensTest",
            "com.stericsson.sdk.equipment.bootrom.BootRomTokenParserTest",
            "com.stericsson.sdk.equipment.tasks.EquipmentBootingDoneTaskTest",
            "com.stericsson.sdk.equipment.tasks.EquipmentBootTaskTest",
            "com.stericsson.sdk.equipment.tasks.EquipmentBurnOTPTaskTest",
            "com.stericsson.sdk.equipment.tasks.EquipmentBurnOTPDataTaskTest",
            "com.stericsson.sdk.equipment.tasks.EquipmentReadOTPTaskTest",
            "com.stericsson.sdk.equipment.tasks.EquipmentReadOTPDataTaskTest"
            };

    private static ILoaderCommunicationMock mock = null;

    private static IPort port = new AbstractPort("USB2") {

        public int write(byte[] buffer, int offset, int length, int timeout) throws PortException {
            return 0;
        }

        public int read(byte[] buffer, int offset, int length, int timeout) throws PortException {
            return 0;
        }
    };

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
        TestFragmentActivator.setBundleContext(context);
        TestCase testCaseService;
        TestSuite testSuiteService = new TestSuite();

        testCaseService = new PlatformPortReceiverTest();
        context.registerService(TestCase.class.getName(), testCaseService, null);
        testSuiteService.addTest(testCaseService);

        testCaseService = new EquipmentExceptionTest();
        context.registerService(TestCase.class.getName(), testCaseService, null);
        testSuiteService.addTest(testCaseService);

        testCaseService = new EquipmentProfileTest();
        context.registerService(TestCase.class.getName(), testCaseService, null);
        testSuiteService.addTest(testCaseService);

        testCaseService = new EquipmentProfileServiceTest();
        context.registerService(TestCase.class.getName(), testCaseService, null);
        testSuiteService.addTest(testCaseService);

        Hashtable<String, String> properties = new Hashtable<String, String>();
        properties.put("bundle.name", "com.stericsson.sdk.equipment");
        context.registerService(TestSuite.class.getName(), testSuiteService, properties);

        testSuiteService = new TestSuite();

        for (String actualClassName : testCasesClassesNames) {

            Class<?> actualClazz;
            try {
                actualClazz = Class.forName(actualClassName);
                testCaseService = (TestCase) actualClazz.newInstance();
                context.registerService(TestCase.class.getName(), testCaseService, null);
                testSuiteService.addTest(testCaseService);
            } catch (Throwable t) {
                System.out.println(t.getMessage());
            }
        }

        properties = new Hashtable<String, String>();
        properties.put("bundle.name", "com.stericsson.sdk.equipment.io");
        context.registerService(TestSuite.class.getName(), testSuiteService, properties);

        ServiceReference[] references =
            TestFragmentActivator.getBundleContext().getServiceReferences(ILoaderCommunicationService.class.getName(),
                "(type=mock)");
        Object service = TestFragmentActivator.getBundleContext().getService(references[0]);
        setMock((ILoaderCommunicationMock) service);
    }

    /**
     * {@inheritDoc}
     */
    public void stop(BundleContext context) throws Exception {
    }

    /**
     * @param ilcm
     *            the mock to set
     */
    private static void setMock(ILoaderCommunicationMock ilcm) {
        TestFragmentActivator.mock = ilcm;
    }

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
}
