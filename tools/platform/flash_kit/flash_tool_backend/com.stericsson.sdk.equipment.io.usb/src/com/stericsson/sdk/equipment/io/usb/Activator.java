package com.stericsson.sdk.equipment.io.usb;

import java.util.ArrayList;
import java.util.Dictionary;
import java.util.Hashtable;
import java.util.List;
import java.util.Locale;

import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceRegistration;

import com.stericsson.sdk.common.EnvironmentProperties;
import com.stericsson.sdk.common.configuration.IConfigurationServiceListener;
import com.stericsson.sdk.equipment.io.port.IPortProviderTester;
import com.stericsson.sdk.equipment.io.usb.internal.USBNativeHandlerServices;

/**
 * @author emicroh
 * 
 */
public class Activator implements BundleActivator {

    private final List<ServiceRegistration> registrations = new ArrayList<ServiceRegistration>();

    private static BundleContext bundleContext;

    private static boolean runningOnWindows = false;

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
    public void start(BundleContext ctx) throws Exception {
        Activator.setBundleContext(ctx);

        // create object providing all desired services
        USBNativeHandlerServices serviceObject = USBNativeHandlerServices.getInstance();

        registerService(IConfigurationServiceListener.class, serviceObject, null);

        if (EnvironmentProperties.isRunningInTestMode()) {
            Dictionary<String, String> settings = new Hashtable<String, String>();
            settings.put("port.provider.tester", "USB");
            registerService(IPortProviderTester.class, serviceObject, settings);
        }

        if (System.getProperty("os.name").toLowerCase(Locale.getDefault()).contains("win")) {
            Activator.setRunningOnWindows(true);
        }
    }

    /**
     * {@inheritDoc}
     */
    public void stop(BundleContext ctx) throws Exception {
        if (USBNativeHandlerServices.getInstance().isTriggered()) {
            USBNativeHandlerServices.getInstance().release();
        }
        for (ServiceRegistration regObj : registrations) {
            regObj.unregister();
        }
    }

    private void registerService(Class<?> clazz, Object service, Dictionary<String, String> pSetting) {
        registrations.add(getBundleContext().registerService(clazz.getName(), service, pSetting));
    }
}
