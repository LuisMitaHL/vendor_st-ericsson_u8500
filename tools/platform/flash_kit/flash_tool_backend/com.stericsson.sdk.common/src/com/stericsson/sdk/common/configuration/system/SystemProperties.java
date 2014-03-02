package com.stericsson.sdk.common.configuration.system;

import java.util.HashSet;
import java.util.Set;

import org.osgi.framework.Bundle;
import org.osgi.framework.BundleContext;
import org.osgi.framework.FrameworkUtil;
import org.osgi.framework.ServiceReference;
import org.osgi.util.tracker.ServiceTracker;
import org.osgi.util.tracker.ServiceTrackerCustomizer;

import com.stericsson.sdk.common.configuration.ISystemPropertiesListener;

/**
 * This class contains string constants of keys of all the system properties that are used in flash
 * tool backend.<BR>
 * <BR>
 * 
 * System.getProperty should not be used directly, the getProperty method from this class should be
 * used instead. This should help tracking of which properties are in use.<BR>
 * <BR>
 * 
 * This class is tracking ISystemPropertiesContributor services which serve as additional source of
 * properties.<BR>
 * <BR>
 * 
 * The precedence is the following:<BR>
 * 1. property defined as system property<BR>
 * 2. property defined by a contributor.<BR>
 * It means that if a property is defined both in system and by contributor, the system value is
 * used.<BR>
 * <BR>
 * 
 * Two different contributors are not allowed to contribute the same property (this is checked and
 * exception is thrown in such case).<BR>
 * <BR>
 * 
 * @author xadazim
 * 
 */
public enum SystemProperties {
    /**
     * The only instance.
     */
    INSTANCE;

    private final Set<ISystemPropertiesContributor> contributors = new HashSet<ISystemPropertiesContributor>();

    private ServiceTracker listenerTracker;

    private SystemProperties() {
        Bundle bundle = FrameworkUtil.getBundle(this.getClass());
        if (bundle == null) {
            throw new NullPointerException("Unable resolve bundle.");
        }
        BundleContext bundleContext = bundle.getBundleContext();
        if (bundleContext != null) {
            ServiceTracker tracker =
                new ServiceTracker(bundleContext, ISystemPropertiesContributor.class.getName(),
                    new PropertyServiceTrackerCustomizer());
            tracker.open();

            listenerTracker = new ServiceTracker(bundleContext, ISystemPropertiesListener.class.getName(), null);
            listenerTracker.open();
        } else {
            throw new NullPointerException("Unable resolve context bundle.");
        }

    }

    /**
     * Value associated with this key determines if core dump gets downloaded automatically after a
     * dumping ME is connected (used by dump collector classes).
     */
    public static final String BACKEND_AUTO_DOWNLOAD_ENABLED = "coredump.auto.download";

    /**
     * Value associated with this key defines where downloaded core dump will be placed (used by
     * dump collector classes).
     */
    public static final String BACKEND_DOWNLOAD_PATH = "coredump.download.path";

    /**
     * Value associated with this key determines if core dump should be removed from ME after it
     * gets downloaded (used by dump collector classes).
     */
    public static final String BACKEND_AUTO_DELETE_ENABLED = "coredump.auto.delete";

    /**
     * Value associated with this key defines where to look for backend.mesp file.
     */
    public static final String BACKEND_CONFIGURATIONS_ROOT = "configurations.root";

    /**
     * Value associated with this key defines where to look for profile files.
     */
    public static final String BACKEND_PROFILES_ROOT = "profiles.root";

    /**
     * Value associated with this key defines where to look for loader files.
     */
    public static final String BACKEND_LOADER_ROOT = "loaders.root";

    /**
     * Value associated with this key defines where to find lcd dynamic library.
     */
    public static final String BACKEND_LCD_FILE = "com.stericsson.sdk.loader.communication.lcd";

    /**
     * Value associated with this key defines where to find lcm dynamic library.
     */
    public static final String BACKEND_LCM_FILE = "com.stericsson.sdk.loader.communication.lcm";

    /**
     * Value associated with this key defines port where brp server will be listening.
     */
    public static final String BACKEND_BRP_PORT = "brp.port";

    /**
     * Value associated with this key defines where to find dynamic libraries. It's default key
     * which should be always set.
     */
    public static final String BACKEND_NATIVE_PATH = "native.root";

    /**
     * Value associated with this key defines level of debug logs which is used to initialize jni.
     */
    public static final String JNI_LOG_LEVEL = "jni.log.level";

    /**
     * Value associated with this key defines level of debug logs which is used to initialize
     * libusb.
     */
    public static final String LIBUSB_LOG_LEVEL = "libusb.log.level";

    /**
     * Returns a property value.<BR>
     * <BR>
     * 
     * The precedence is the following:<BR>
     * 1. property defined as system property<BR>
     * 2. property defined by a contributor.<BR>
     * It means that if a property is defined both in system and by contributor, the system value is
     * used.<BR>
     * <BR>
     * Throws RuntimeExcpetion when two or more contributors define value of the same key
     * 
     * @param key
     *            property key
     * @return property value
     * 
     */
    public static final String getProperty(String key) {
        if (System.getProperty(key) != null) {
            return System.getProperty(key);
        }

        String result = null;
        synchronized (INSTANCE.contributors) {
            for (ISystemPropertiesContributor c : INSTANCE.contributors) {
                if (c.getProperty(key) != null) {
                    if (result != null) {
                        throw new RuntimeException("Two or more contributors define the same variable");
                    }
                    result = c.getProperty(key);
                }
            }
        }

        return result;
    }

    /**
     * Checks if the property is defined as a system property or if it is provided by a contributor.
     * 
     * @param key
     *            property key
     * @return true if the property is defined by the system
     */
    public static final boolean isSystemDefined(String key) {
        return (System.getProperty(key) != null);
    }

    private class PropertyServiceTrackerCustomizer implements ServiceTrackerCustomizer {

        public Object addingService(ServiceReference pReference) {
            Bundle b = FrameworkUtil.getBundle(this.getClass());
            if (b == null) {
                throw new NullPointerException("Unable resolve bundle.");
            }

            BundleContext bc = b.getBundleContext();
            if (bc != null) {
                ISystemPropertiesContributor service = (ISystemPropertiesContributor) bc.getService(pReference);
                synchronized (INSTANCE.contributors) {
                    contributors.add(service);
                }
                notifyAllListeners(bc, service);
                return service;
            } else {
                throw new NullPointerException("Unable resolve context bundle.");
            }

        }

        public void modifiedService(ServiceReference pReference, Object pService) {
            // ignore
        }

        public void removedService(ServiceReference pReference, Object pService) {
            synchronized (INSTANCE.contributors) {
                contributors.remove(pService);
            }
        }

        private void notifyAllListeners(BundleContext bc, ISystemPropertiesContributor contributor) {
            ISystemPropertiesListener listener;
            ServiceReference[] refs = listenerTracker.getServiceReferences();
            if (refs != null) {
                for (ServiceReference ref : refs) {
                    listener = (ISystemPropertiesListener) bc.getService(ref);
                    listener.configurationChanged(contributor);
                }
            }
        }
    }
}
