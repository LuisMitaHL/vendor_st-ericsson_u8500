package com.stericsson.sdk.equipment.m7x00;

import java.io.File;
import java.net.URI;
import java.net.URL;
import java.util.Hashtable;

import org.apache.log4j.Logger;
import org.eclipse.core.runtime.FileLocator;
import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;

import com.stericsson.sdk.common.EnvironmentProperties;
import com.stericsson.sdk.equipment.IPlatform;
import com.stericsson.sdk.equipment.io.port.PortDeliveryState;
import com.stericsson.sdk.equipment.m7x00.internal.M7x00Platform;
import com.stericsson.sdk.loader.communication.ILoaderCommunicationListener;

/**
 * @author xtomlju
 */
public class Activator implements BundleActivator {

    private static BundleContext bundleContext;

    private static final Logger log = Logger.getLogger(BundleActivator.class);

    private static File bsuExeFile;

    private static final String BSU_EXE_NAME = "BSUHostManager.exe";

    /**
     * @return Bundle context instance
     */
    public static BundleContext getBundleContext() {
        return bundleContext;
    }

    /**
     * Gets file (path) to BSUHostManager.exe
     * 
     * @return path to BSUHostManager.exe
     */
    public static File getBSUExeFile() {
        return bsuExeFile;
    }

    private static void setBundleContext(BundleContext context) {
        bundleContext = context;
    }

    private static void setBsuExeFile(File pBsuExeFile) {
        bsuExeFile = pBsuExeFile;
    }

    private BundleActivator testFragmentActivator;

    /**
     * {@inheritDoc}
     */
    public void start(BundleContext context) throws Exception {
        Activator.setBundleContext(context);
        Hashtable<String, String> properties = new Hashtable<String, String>();
        properties.put(M7x00Platform.PLATFORM_PROPERTY, M7x00Platform.PLATFORM);
        properties.put(M7x00Platform.STATE_PROPERTY, PortDeliveryState.SERVICE.name());
        M7x00Platform platform = new M7x00Platform();
        context.registerService(IPlatform.class.getName(), platform, properties);
        context.registerService(ILoaderCommunicationListener.class.getName(), platform, properties);
        testFragmentActivatorStart(context);

        // find BSUHostManager.exe
        URL entryUrl = context.getBundle().getEntry(BSU_EXE_NAME);
        if (entryUrl != null) {
            URL url = FileLocator.toFileURL(entryUrl);
            URI uri = new URI(url.toString().replaceAll(" ", "%20"));
            Activator.setBsuExeFile(new File(uri));
            log.info(BSU_EXE_NAME + " path is " + bsuExeFile.getAbsolutePath());
        } else {
            throw new Exception("Can not locate " + BSU_EXE_NAME + " in the bundle jar");
        }

        if (EnvironmentProperties.isRunningInTestMode()) {
            new M7x00Platform().createDummyEquipment();
        }
    }

    /**
     * {@inheritDoc}
     */
    public void stop(BundleContext context) throws Exception {
        setBundleContext(null);
        testFragmentActivatorStop(context);
    }

    /**
     * Start Fragment bundle for test if it is accessible. Should be called at and of start method
     * of Host bundle.
     * 
     * @param context
     *            BundleContext of host bundle
     * @throws Exception .
     */
    private void testFragmentActivatorStart(BundleContext context) throws Exception {
        try {
            Object tmpFragmentActivator =
                Class.forName("com.stericsson.sdk.equipment.m7x00.test.TestFragmentActivator").newInstance();
            if (tmpFragmentActivator instanceof BundleActivator) {
                testFragmentActivator = (BundleActivator) tmpFragmentActivator;
                testFragmentActivator.start(context);
            }
        } catch (ClassNotFoundException e) {
            testFragmentActivator = null;
        }
    }

    /**
     * stop Fragment bundle for test if it is accessible. Should be called at and of stop method of
     * Host bundle.
     * 
     * @param context
     *            BundleContext of host bundle
     * @throws Exception .
     */
    private void testFragmentActivatorStop(BundleContext context) throws Exception {
        if (testFragmentActivator != null) {
            testFragmentActivator.stop(context);
        }
    }

}
