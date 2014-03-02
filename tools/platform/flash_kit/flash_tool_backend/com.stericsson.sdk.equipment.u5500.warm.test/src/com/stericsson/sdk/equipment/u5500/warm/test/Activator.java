package com.stericsson.sdk.equipment.u5500.warm.test;

import java.util.ArrayList;
import java.util.Hashtable;
import java.util.List;

import junit.framework.TestCase;
import junit.framework.TestSuite;

import org.apache.log4j.Logger;
import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;

import com.stericsson.sdk.equipment.io.AbstractFTPPort;
import com.stericsson.sdk.equipment.io.port.IPort;
import com.stericsson.sdk.equipment.io.port.PortException;

/**
 * @author eolabor
 *
 */
public class Activator implements BundleActivator {

    private static boolean throwException = false;

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

    private String[] testCasesClassesNames = new String[] {
        "com.stericsson.sdk.equipment.u5500.warm.test.U5500WarmEquipmentBootTaskTest",
        "com.stericsson.sdk.equipment.u5500.warm.test.U5500WarmEquipmentShutdownTaskTest",
        "com.stericsson.sdk.equipment.u5500.warm.test.U5500WarmEquipmentListFileTaskTest",
        "com.stericsson.sdk.equipment.u5500.warm.test.U5500WarmEquipmentDownloadDumpTaskTest",
        "com.stericsson.sdk.equipment.u5500.warm.test.U5500WarmEquipmentDeleteDumpTaskTest"};

    /**
     * Help parameter for testcases
     *
     * @param exception
     *            boolean
     */
    public static void setThrowException(boolean exception) {
        throwException = exception;
    }

    private static IPort port = new AbstractFTPPort("IpAddress") {

        boolean open = true;

        @Override
        public String getPortName() {
            // TODO Auto-generated method stub
            return super.getPortName();
        }

        @Override
        public void close() throws PortException {
            if (throwException) {
                System.out.println("Throwing Exception");
                throw new PortException("Test: Should throw exception");
            } else {
                open = false;
            }
        }

        @Override
        public boolean isOpen() {
            return open;
        }

        @Override
        public void open() throws PortException {
            // TODO Auto-generated method stub
            super.open();
        }

        @Override
        public boolean connect(String username, String password) {
            if (throwException) {
                return false;
            }
            return true;
        }

        @Override
        public List<String> listAllFiles() {
            List<String> fileList = new ArrayList<String>();
            fileList.add("CoreDump1");
            fileList.add("CoreDump2");
            return fileList;
        }

        @Override
        public boolean downloadFile(String ftpFileName, String localFilePath) {
            if (throwException) {
                System.out.println("downloadfile retunrs false");
                return false;
            }
            return true;
        }

        @Override
        public boolean deleteFile(String ftpFileName) {
            if (throwException) {
                System.out.println("deletefile retunrs false");
                return false;
            }
            return true;
        }

        @Override
        public boolean uploadFile(String localFilePath, String fileName) {
            if (throwException) {
                return false;
            }
            return true;
        }

        @Override
        public void disconnect() {
        }

        @Override
        public boolean isDumping() {
            // TODO Auto-generated method stub
            return super.isDumping();
        }

    };

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
        properties.put("bundle.name", "com.stericsson.sdk.equipment.u5500.warm");
        context.registerService(TestSuite.class.getName(), testSuiteService, properties);
    }

    /**
     * {@inheritDoc}
     */
    public void stop(BundleContext context) throws Exception {
    }

}
