package com.stericsson.sdk.loader.communication.test;

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

/**
 * @author xdancho
 */
public class Activator implements BundleActivator {

    private static String resourcePath = "";

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
     * @param path
     *            the resourcePath to set
     */
    private static void setResourcesPath(String path) {
        Activator.resourcePath = path;
    }

    private static final String RESOURCE_PATH = "/resources";

    /** Logger instance */
    private static Logger logger = Logger.getLogger(Activator.class.getName());

    private String[] testCasesClassesNames =
        new String[] {
            "com.stericsson.sdk.loader.communication.test.DataTypesTest",
            "com.stericsson.sdk.loader.communication.test.DataTypesTest2",
            "com.stericsson.sdk.loader.communication.test.ParserTest",
            "com.stericsson.sdk.loader.communication.test.CommandsTest",
            "com.stericsson.sdk.loader.communication.test.LCCommandMappingsTest",
            "com.stericsson.sdk.loader.communication.test.LCTimeoutsTest",
            "com.stericsson.sdk.loader.communication.test.TaskTest",
            "com.stericsson.sdk.loader.communication.test.LCContextTest",
            "com.stericsson.sdk.loader.communication.test.ReaderWriterTest"};

    /**
     * get the path to the resource files needed for tests
     * 
     * @return the resource path
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

    /**
     * {@inheritDoc}
     */
    public void start(BundleContext context) throws Exception {
        Activator.setBundleContext(context);
        setResourcePath();

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
        properties.put("bundle.name", "com.stericsson.sdk.loader.communication");
        context.registerService(TestSuite.class.getName(), testSuiteService, properties);

    }

    /**
     * {@inheritDoc}
     */
    public void stop(BundleContext context) throws Exception {
    }

}
