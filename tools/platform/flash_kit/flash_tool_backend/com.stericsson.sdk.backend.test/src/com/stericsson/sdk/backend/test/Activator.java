package com.stericsson.sdk.backend.test;

import java.util.Hashtable;

import junit.framework.TestCase;
import junit.framework.TestSuite;

import org.apache.log4j.Logger;
import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceReference;

import com.stericsson.sdk.backend.IBackendService;

/**
 * @author ezaptom
 */
public class Activator implements BundleActivator {

    /** */
    static IBackendService backendService;

    private static BundleContext bundleContext;

    /**
     * @param bs
     *            the backendService to set
     */
    private static void setBackendService(IBackendService bs) {
        Activator.backendService = bs;
    }

    private static void setBundleContext(BundleContext context) {
        bundleContext = context;
    }

    /** Logger instance */
    private static Logger logger = Logger.getLogger(Activator.class.getName());

    private String[] testCasesClassesNames =
        new String[] {
            "com.stericsson.sdk.backend.remote.test.BackendClientHandlerTest",
            "com.stericsson.sdk.backend.remote.test.BackendServerTest",
            "com.stericsson.sdk.backend.remote.executor.backend.GetDeviceInfoExecutorTest",
            "com.stericsson.sdk.backend.remote.executor.backend.GetAvailableProfilesExecutorTest",
            "com.stericsson.sdk.backend.remote.executor.backend.SetActiveProfileExecutorTest",
            "com.stericsson.sdk.backend.remote.executor.backend.GetActiveProfileExecutorTest",
            "com.stericsson.sdk.backend.remote.executor.backend.ShutdownBackendExecutorTest",
            "com.stericsson.sdk.backend.remote.executor.backend.SetLocalSigningExecutorTest",
            "com.stericsson.sdk.backend.remote.executor.backend.TriggerUARTPortExecutorTest",
            "com.stericsson.sdk.backend.remote.executor.security.GetEquipmentPropertiesExecutorTest",
            "com.stericsson.sdk.backend.remote.executor.security.GetEquipmentAvailableSecurityPropertiesExecutorTest",
            "com.stericsson.sdk.backend.remote.executor.security.GetEquipmentPropertyExecutorTest",
            "com.stericsson.sdk.backend.remote.executor.security.SetEquipmentPropertyExecutorTest",
            "com.stericsson.sdk.backend.remote.executor.security.BindPropertiesExecutorTest",
            "com.stericsson.sdk.backend.remote.executor.filesystem.FileSystemExecutorTest",
            "com.stericsson.sdk.backend.remote.executor.flash.DumpAreaExecutorTest",
            "com.stericsson.sdk.backend.remote.executor.flash.EraseAreaExecutorTest",
            "com.stericsson.sdk.backend.remote.executor.flash.ProcessFileExecutorTest",
            "com.stericsson.sdk.backend.remote.executor.coredump.DownloadDumpExecutorTest",
            "com.stericsson.sdk.backend.remote.executor.coredump.DeleteDumpExecutorTest",
            "com.stericsson.sdk.backend.remote.executor.coredump.ListFileExecutorTest",
            "com.stericsson.sdk.backend.remote.executor.coredump.SetSubscriptionExecutorTest",
            "com.stericsson.sdk.backend.remote.executor.coredump.GetSubscriptionExecutorTest",
            "com.stericsson.sdk.backend.remote.executor.coredump.GetCoreDumpLocationExecutorTest",
            "com.stericsson.sdk.backend.remote.executor.coredump.SetCoreDumpLocationExecutorTest",
            "com.stericsson.sdk.backend.remote.executor.coredump.SetAutoDownloadExecutorTest",
            "com.stericsson.sdk.backend.remote.executor.coredump.GetAutoDownloadExecutorTest",
            "com.stericsson.sdk.backend.remote.executor.coredump.GetAutoDeleteStatusExecutorTest",
            "com.stericsson.sdk.backend.remote.executor.coredump.SetAutoDeleteExecutorTest"};

    /**
     * @return Bunde context object
     */
    public static BundleContext getBundleContext() {
        return bundleContext;
    }

    /**
     * @return TBD
     */
    public static IBackendService getBackendService() {
        return backendService;
    }

    /**
     * {@inheritDoc}
     */
    public void start(BundleContext context) throws Exception {
        Activator.setBundleContext(context);
        TestCase testCaseService;
        TestSuite testSuiteService = new TestSuite();

        testCaseService = new FlashToolBackendConfigurationServiceTest();
        context.registerService(TestCase.class.getName(), testCaseService, null);
        testSuiteService.addTest(testCaseService);

        testCaseService = new FlashToolBackendServiceTest();
        context.registerService(TestCase.class.getName(), testCaseService, null);
        testSuiteService.addTest(testCaseService);

        Hashtable<String, String> properties = new Hashtable<String, String>();
        properties.put("bundle.name", "com.stericsson.sdk.backend");
        context.registerService(TestSuite.class.getName(), testSuiteService, properties);

        ServiceReference serviceReference = context.getServiceReference(IBackendService.class.getName());
        setBackendService((IBackendService) context.getService(serviceReference));
        backendService.setAcceptClients(false);
        backendService.setAcceptEquipments(false);

        testSuiteService = new TestSuite();

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

        properties = new Hashtable<String, String>();
        properties.put("bundle.name", "com.stericsson.sdk.backend.remote");
        context.registerService(TestSuite.class.getName(), testSuiteService, properties);
    }

    /**
     * {@inheritDoc}
     */
    public void stop(BundleContext context) throws Exception {
    }
}
