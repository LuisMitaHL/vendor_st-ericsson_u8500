package com.stericsson.sdk.equipment.u8500.test;

import java.io.File;
import java.io.IOException;
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
            "com.stericsson.sdk.equipment.u8500.test.U8500EquipmentBootTaskTest",
            "com.stericsson.sdk.equipment.u8500.test.U8500BinaryImageTest",
            "com.stericsson.sdk.equipment.u8500.test.AbstractByteSequenceTest",
            "com.stericsson.sdk.equipment.u8500.test.U8500EquipmentShutdownTaskTest",
            "com.stericsson.sdk.equipment.u8500.test.U8500EquipmentDumpAreaTaskTest",
            "com.stericsson.sdk.equipment.u8500.test.U8500EquipmentEraseAreaTaskTest",
            "com.stericsson.sdk.equipment.u8500.test.U8500EquipmentProcessFileTaskTest",
            "com.stericsson.sdk.equipment.u8500.test.U8500EquipmentSendRawDataTaskTest",
            "com.stericsson.sdk.equipment.u8500.test.U8500EquipmentReadGlobalDataSetTaskTest",
            "com.stericsson.sdk.equipment.u8500.test.U8500EquipmentWriteGlobalDataSetTaskTest",
            "com.stericsson.sdk.equipment.u8500.test.U8500EquipmentEraseGlobalDataSetTaskTest",
            "com.stericsson.sdk.equipment.u8500.test.U8500EquipmentTest",
            "com.stericsson.sdk.equipment.u8500.test.U8500EquipmentReadGlobalDataUnitTaskTest",
            "com.stericsson.sdk.equipment.u8500.test.U8500EquipmentWriteGlobalDataUnitTaskTest",
            "com.stericsson.sdk.equipment.u8500.test.U8500EquipmentRebootTaskTest",
            "com.stericsson.sdk.equipment.u8500.test.U8500EquipmentPropertiesTaskTest",
            "com.stericsson.sdk.equipment.u8500.test.U8500EquipmentCollectDataTaskTest",
            "com.stericsson.sdk.equipment.u8500.test.U8500EquipmentListDevicesTaskTest",
            "com.stericsson.sdk.equipment.u8500.test.U8500EquipmentAuthenticateCertificateTaskTest",
            "com.stericsson.sdk.equipment.u8500.test.U8500EquipmentDomainTaskTest",
            "com.stericsson.sdk.equipment.u8500.test.EquipmentLoaderCommunicationInitTaskTest",
            "com.stericsson.sdk.equipment.u8500.test.EquipmentChangeBaudRateTest",
            "com.stericsson.sdk.equipment.u8500.test.U8500EquipmentAuthenticateControlKeysTaskTest"};

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

        setResourcePath();

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
        properties.put("bundle.name", "com.stericsson.sdk.equipment.u8500");
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

    private static String resourcePath = "";

    private static final String RESOURCE_PATH = "/res";

    private static void setResourcesPath(String path) {
        Activator.resourcePath = path;
    }

    /**
     * @return return path to resources needed during testing
     */
    public static String getResourcesPath() {
        return resourcePath;
    }

    private void setResourcePath() {
        URL url = getClass().getResource(RESOURCE_PATH);

        URL find = null;

        try {

            find = FileLocator.toFileURL(url);

        } catch (IOException e) {
            e.printStackTrace();

        }
        try {
            File file = new File(find.getFile());

            setResourcesPath(file.getAbsolutePath());
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }
    }

}
