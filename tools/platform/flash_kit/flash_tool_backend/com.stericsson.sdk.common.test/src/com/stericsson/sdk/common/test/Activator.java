package com.stericsson.sdk.common.test;

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
 * @author xtomlju
 */
public class Activator implements BundleActivator {
    /** Logger instance */
    private static Logger logger = Logger.getLogger(Activator.class.getName());

    private String[] testCasesClassesNames =
        new String[] {
            "com.stericsson.sdk.common.moto.test.SRecordTest",
            "com.stericsson.sdk.common.test.AbstractFileHandlerTest",
            "com.stericsson.sdk.common.test.HexUtilitiesTest",
            "com.stericsson.sdk.common.test.EnvironmentPropertiesTest",
            "com.stericsson.sdk.common.test.LittleEndianByteConverterTest",
            "com.stericsson.sdk.common.test.VersionParserTest",
            "com.stericsson.sdk.common.test.FileUtilsTest",
            "com.stericsson.sdk.common.configuration.test.MESPConfigurationExceptionTest",
            "com.stericsson.sdk.common.configuration.test.MESPConfigurationRecordTest",
            "com.stericsson.sdk.common.configuration.test.MESPConfigurationTest",
            "com.stericsson.sdk.common.configuration.test.MESPConfigurationTokenizerTest",
            "com.stericsson.sdk.common.memory.test.VirtualMemoryTest",
            "com.stericsson.sdk.common.moto.test.A01FileHandlerTest",
            "com.stericsson.sdk.common.security.ImeiConverterTest",
            "com.stericsson.sdk.common.moto.test.AsciiConverterTest"
            };

    /**
     * {@inheritDoc}
     */
    public void start(BundleContext context) throws Exception {

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
        properties.put("bundle.name", "com.stericsson.sdk.common");
        context.registerService(TestSuite.class.getName(), testSuiteService, properties);

        testCaseService = new HexUtilitiesTest();
        context.registerService(TestCase.class.getName(), testCaseService, null);
        testSuiteService.addTest(testCaseService);

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
