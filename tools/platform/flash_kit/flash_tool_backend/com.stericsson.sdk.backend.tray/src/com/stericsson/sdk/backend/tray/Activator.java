package com.stericsson.sdk.backend.tray;

import java.io.File;
import java.io.IOException;
import java.net.URL;

import org.eclipse.core.runtime.FileLocator;
import org.eclipse.swt.widgets.Display;
import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;

/**
 * @author xtomlju
 */
public class Activator implements BundleActivator {

    private SystemTrayIcon trayIcon = null;

    private Thread trayThread;

    private static BundleContext bundleContext;

    private static String resourcePath;

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
        boolean showTrayIcon = true;
        Activator.setBundleContext(context);
        setResourcePath();
        if (System.getProperty("trayicon.show") != null) {
            if ("false".equalsIgnoreCase(System.getProperty("trayicon.show"))) {
                showTrayIcon = false;
            }
        }

        if (showTrayIcon) {
            trayIcon = new SystemTrayIcon();
            trayThread = new Thread(trayIcon, "System Tray Icon Thread");
            trayThread.start();
        }
    }

    /**
     * {@inheritDoc}
     */
    public void stop(BundleContext context) throws Exception {
        Display.getDefault().asyncExec(new Runnable() {
            public void run() {
                trayIcon.dispose();
            }
        });
        trayThread.join();
        Activator.setBundleContext(null);
    }

    private static void setResourcesPath(String path) {
        resourcePath = path;
    }

    /**
     * @return return path to resources needed during testing
     */
    public static String getResourcesPath() {
        return resourcePath;
    }

    private void setResourcePath() {
        URL url = getClass().getResource("/images");

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
