package com.stericsson.sdk.equipment.m7x00.test;

import java.io.File;
import java.net.URL;
import java.util.Hashtable;

import junit.framework.TestCase;
import junit.framework.TestSuite;

import org.apache.log4j.Logger;
import org.eclipse.core.runtime.FileLocator;
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

    private static final String RESOURCES_PATH = "/resources";

    /** Logger instance */
    private static Logger logger = Logger.getLogger(TestFragmentActivator.class.getName());

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

    private Class<?>[] testCasesClasses =
        new Class[] {
            com.stericsson.sdk.equipment.m7x00.internal.M7X00EquipmentTest.class,
            com.stericsson.sdk.equipment.m7x00.internal.M7x00PlatformTest.class,
            com.stericsson.sdk.equipment.m7x00.tasks.M7X00EquipmentCopyFileTaskTest.class,
            com.stericsson.sdk.equipment.m7x00.tasks.M7X00EquipmentCreateDirectoryTaskTest.class,
            com.stericsson.sdk.equipment.m7x00.tasks.M7X00EquipmentDeleteFileTaskTest.class,
            com.stericsson.sdk.equipment.m7x00.tasks.M7X00EquipmentFormatVolumeTaskTest.class,
            com.stericsson.sdk.equipment.m7x00.tasks.M7X00EquipmentMoveFileTaskTest.class,
            com.stericsson.sdk.equipment.m7x00.tasks.M7X00EquipmentChangeAccessTaskTest.class,
            com.stericsson.sdk.equipment.m7x00.tasks.M7X00EquipmentListDirectoryTaskTest.class,
            com.stericsson.sdk.equipment.m7x00.tasks.M7X00EquipmentPropertiesTaskTest.class,
            com.stericsson.sdk.equipment.m7x00.tasks.M7X00EquipmentVolumePropertiesTaskTest.class,
            com.stericsson.sdk.equipment.m7x00.tasks.M7X00EquipmentListLoadModulesTaskTest.class,
            com.stericsson.sdk.equipment.m7x00.tasks.M7X00EquipmentCheckCompatibilityTaskTest.class,
            com.stericsson.sdk.equipment.m7x00.bsu.BSUUtilsTest.class};

    private static ILoaderCommunicationMock mock = null;

    /**
     * @param ilcm
     *            the mock to set
     */
    private static void setMock(ILoaderCommunicationMock ilcm) {
        TestFragmentActivator.mock = ilcm;
    }

    private static IPort port = new AbstractPort("USB2") {

        public int write(byte[] buffer, int offset, int length, int timeout) throws PortException {
            return 0;
        }

        public int read(byte[] buffer, int offset, int length, int timeout) throws PortException {
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
        TestFragmentActivator.setBundleContext(context);
        TestCase testCaseService;
        TestSuite testSuiteService = new TestSuite();

        for (Class<?> actualClazz : testCasesClasses) {

            try {
                testCaseService = (TestCase) actualClazz.newInstance();
                context.registerService(TestCase.class.getName(), testCaseService, null);
                testSuiteService.addTest(testCaseService);
            } catch (Throwable t) {
                logger.error(t.getMessage());
            }
        }

        Hashtable<String, String> properties = new Hashtable<String, String>();
        properties.put("bundle.name", "com.stericsson.sdk.equipment.m7x00");
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

    private static File resourcesPath;

    /**
     * Gets resources path of this bundle
     * 
     * @return resources path
     * @throws Exception
     *             in case resources could not be found
     */
    public static File getResourcesPath() throws Exception {
        if (resourcesPath == null) {
            URL url = TestFragmentActivator.class.getResource(RESOURCES_PATH);

            URL find = null;

            try {
                find = FileLocator.toFileURL(url);
            } finally {
                // we are missing resources in jar file
                logger.error("Can not access resources of the bundle " + TestFragmentActivator.class.getPackage().getName());
            }

            resourcesPath = new File(find.getFile());
        }

        return resourcesPath;
    }
}
