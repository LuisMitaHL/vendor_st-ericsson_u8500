package com.stericsson.sdk.backend;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.URL;
import java.util.Hashtable;

import org.apache.log4j.Logger;
import org.eclipse.core.runtime.FileLocator;
import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceReference;
import org.osgi.util.tracker.ServiceTracker;
import org.osgi.util.tracker.ServiceTrackerCustomizer;

import com.stericsson.sdk.backend.internal.FlashToolBackendConfigurationService;
import com.stericsson.sdk.backend.internal.FlashToolBackendService;
import com.stericsson.sdk.backend.remote.io.internal.BackendServer;
import com.stericsson.sdk.common.configuration.IConfigurationRecord;
import com.stericsson.sdk.common.configuration.IConfigurationService;
import com.stericsson.sdk.common.configuration.IConfigurationServiceListener;
import com.stericsson.sdk.common.configuration.ISystemPropertiesListener;
import com.stericsson.sdk.common.configuration.system.ISystemPropertiesContributor;

/**
 * @author xtomlju
 */
public class Activator implements BundleActivator, ServiceTrackerCustomizer, ISystemPropertiesListener {

    private static final String RESOURCE_PATH = "/resources";

    private static File resourcesPath = null;

    private BackendServer backendServer;

    private ServiceTracker tracker;

    private FlashToolBackendConfigurationService backendConfigurationService;

    private FlashToolBackendService backendService;

    private static BundleContext bundleContext;

    // /** It's true if configuration mesp was bean loaded */
    // private boolean loaded = false;

    private static Logger logger = Logger.getLogger(Activator.class.getName());

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
     *
     * {@inheritDoc}
     */
    public void start(BundleContext context) throws Exception {
        Activator.setBundleContext(context);

        // this part load and print release version of backend from releasenotes.txt
        String backendVersion = getVersion();
        if (backendVersion != null) {
            logger.info(backendVersion);
        }

        tracker = new ServiceTracker(context, IConfigurationServiceListener.class.getName(), this);

        backendConfigurationService = new FlashToolBackendConfigurationService();
        Hashtable<String, String> dictionary = new Hashtable<String, String>();
        dictionary.put("type", "backend");
        dictionary.put("format", "mesp");
        context.registerService(IConfigurationService.class.getName(), backendConfigurationService, dictionary);

        backendService = new FlashToolBackendService();

        context.registerService(IBackendService.class.getName(), backendService, null);

        loadBackendConfiguration();

        tracker.open();

        backendServer = new BackendServer();
        context.registerService(IConfigurationServiceListener.class.getName(), backendServer, null);

        context.registerService(ISystemPropertiesListener.class.getName(), this, null);
    }

    /**
     * {@inheritDoc}
     */
    public void stop(BundleContext context) throws Exception {
        tracker.close();
        backendServer.setEnabled(false);
    }

    /**
     * {@inheritDoc}
     */
    public Object addingService(ServiceReference reference) {

        IConfigurationServiceListener listener =
            (IConfigurationServiceListener) getBundleContext().getService(reference);
        IConfigurationService configurationService = backendService.getConfigurationService();
        if (configurationService != null) {
            for (IConfigurationRecord record : configurationService.getAllRecords()) {
                listener.configurationChanged(configurationService, null, record);
            }
        }

        return reference;
    }

    /**
     * {@inheritDoc}
     */
    public void modifiedService(ServiceReference reference, Object service) {
    }

    /**
     * {@inheritDoc}
     */
    public void removedService(ServiceReference reference, Object service) {
    }

    private void loadBackendConfiguration() {
        try {
            backendService.loadConfiguration();
            // loaded = true;
        } catch (NullPointerException e) {
            logger.warn(e.getMessage());
        } catch (FileNotFoundException e) {
            logger.error(e.getMessage());
        } catch (IOException e) {
            logger.fatal("Failed to read configuration file", e);
        }
    }

    /**
     * {@inheritDoc}
     */
    public void configurationChanged(ISystemPropertiesContributor contributor) {
        logger.debug("System configuration changed " + contributor);
        if (backendService != null) {
            // if (!loaded) {
            loadBackendConfiguration();
            // }
        }
    }

    /**
     * This method find and load version of backend from release notes
     *
     * @return string of release version
     */
    private String getVersion() {
        String line = null;
        File releaseNotes = null;
        File dir = null;
        FileInputStream isr = null;
        BufferedReader reader = null;

        dir = new File(System.getProperty("user.dir"));
        dir = dir.getParentFile();
        if (dir == null) {
            return null;
        }
        releaseNotes = new File(dir.getAbsolutePath() + File.separator + "ReleaseNotes.txt");

        if (!releaseNotes.exists()) {
            dir = dir.getParentFile();
            if (dir == null) {
                return null;
            }
            releaseNotes = new File(dir.getAbsolutePath() + File.separator + "ReleaseNotes.txt");
            if (!releaseNotes.exists()) {
                return null;
            }
        }

        try {
            isr = new FileInputStream(releaseNotes);
            reader = new BufferedReader(new InputStreamReader(isr, "UTF-8"));
            line = reader.readLine();
            while (line != null && !line.startsWith("FlashKit")) {
                line = reader.readLine();
            }
            reader.close();
            isr.close();
        } catch (Exception e) {
            return null;
        }
        return line;
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
