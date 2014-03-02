package com.stericsson.sdk.loader.communication;

import java.io.File;
import java.io.IOException;
import java.net.URL;
import java.util.Hashtable;
import java.util.Locale;

import org.apache.log4j.Logger;
import org.eclipse.core.runtime.FileLocator;
import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;

import com.stericsson.sdk.loader.communication.internal.LoaderCommunicationCommandHandler;
import com.stericsson.sdk.loader.communication.internal.LoaderCommunicationMock;

/**
 * @author xtomlju
 */
public class Activator implements BundleActivator {

    private static final String RESOURCE_PATH = "/resources";

    private static BundleContext bundleContext;

    private static boolean runningOnWindows = false;

    private static File resourcesPath = null;

    private static Logger logger = Logger.getLogger(Activator.class.getName());

    /**
     * Reason for separate by the OS running:
     * 
     * For win the locks is not needed and the timeouts can be set relatively high and still reach
     * reasonable communication speeds
     * 
     * For linux the locks is needed to be able to boot at all. The timeouts on every read in linux
     * is set to 20 ms to reach reasonable communication speeds
     * 
     * @return true if running on windows
     */
    public static boolean isRunningOnWindows() {
        return runningOnWindows;
    }

    private static void setRunningOnWindows(boolean onWindows) {
        runningOnWindows = onWindows;
    }

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
        Hashtable<String, String> mockDictionary = new Hashtable<String, String>();
        mockDictionary.put("type", "mock");

        Hashtable<String, String> normalDictionary = new Hashtable<String, String>();
        normalDictionary.put("type", "normal");

        LoaderCommunicationCommandHandler cmdHandler = new LoaderCommunicationCommandHandler();
        context.registerService(ILoaderCommunicationService.class.getName(), cmdHandler, normalDictionary);

        LoaderCommunicationMock mock = new LoaderCommunicationMock();
        context.registerService(ILoaderCommunicationService.class.getName(), mock, mockDictionary);

        if (System.getProperty("os.name").toLowerCase(Locale.getDefault()).contains("win")) {
            Activator.setRunningOnWindows(true);
        }
    }

    /**
     * {@inheritDoc}
     */
    public void stop(BundleContext context) throws Exception {
    }

    /**
     * Gets path to resources folder of this bundle. 
     * @return path to resources folder
     */
    public static File getResourcesPath() {
        if (resourcesPath == null) {
            URL url = Activator.class.getResource(RESOURCE_PATH);

            URL find = null;

            try {
                find = FileLocator.toFileURL(url);
            } catch (IOException e) {
                // we are missing resources in jar file, this is critical, throwing runtime exception
                logger.error("Can not access resources of the bundle "+Activator.class.getPackage().getName(), e);
                throw new RuntimeException(e);
            }

            resourcesPath = new File(find.getFile());
        }

        return resourcesPath;
    }
}
