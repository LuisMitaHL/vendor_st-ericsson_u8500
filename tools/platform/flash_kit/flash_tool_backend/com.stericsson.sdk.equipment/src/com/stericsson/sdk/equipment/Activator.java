package com.stericsson.sdk.equipment;

import java.io.File;
import java.io.IOException;
import java.net.URL;
import java.util.ArrayList;
import java.util.Hashtable;
import java.util.List;

import org.eclipse.core.runtime.FileLocator;
import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;
import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;
import org.osgi.util.tracker.ServiceTracker;

import com.stericsson.sdk.common.configuration.IConfigurationServiceListener;
import com.stericsson.sdk.equipment.internal.EquipmentProfileService;
import com.stericsson.sdk.equipment.internal.PlatformPortReceiver;
import com.stericsson.sdk.equipment.io.port.IPortDataListener;
import com.stericsson.sdk.equipment.io.port.IPortReceiver;

/**
 * @author xtomlju
 */
public class Activator implements BundleActivator {

    /** */
    private static ServiceTracker portDataListenerTracker;

    private BundleActivator testFragmentActivator;

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

    private static void initPortDataListenerTracker() {
        portDataListenerTracker = new ServiceTracker(getBundleContext(), IPortDataListener.class.getName(), null);
    }

    private static final String RESOURCE_PATH = "/res";

    private static String resourcePath = "";

    /**
     * {@inheritDoc}
     */
    public void start(BundleContext context) throws Exception {
        Activator.setBundleContext(context);
        setResourcePath();
        initPortDataListenerTracker();
        portDataListenerTracker.open();

        PlatformPortReceiver receiver = new PlatformPortReceiver();

        context.registerService(IPortReceiver.class.getName(), receiver, new Hashtable<String, String>());
        Hashtable<String, String> properties = new Hashtable<String, String>();
        properties.put("type", "profile");
        EquipmentProfileService service = new EquipmentProfileService();
        context.registerService(IEquipmentProfileManagerService.class.getName(), service, properties);
        context.registerService(IConfigurationServiceListener.class.getName(), service, properties);
        context.registerService(IPortDataListener.class.getName(), receiver, new Hashtable<String, String>());
        try {
            Object tmpFragmentActivator =
                Class.forName("com.stericsson.sdk.equipment.test.TestFragmentActivator").newInstance();
            if (tmpFragmentActivator instanceof BundleActivator) {
                testFragmentActivator = (BundleActivator) tmpFragmentActivator;
                testFragmentActivator.start(context);
            }
        } catch (ClassNotFoundException e) {
            testFragmentActivator = null;
        }
    }

    /**
     * {@inheritDoc}
     */
    public void stop(BundleContext context) throws Exception {
        portDataListenerTracker.close();
        if (testFragmentActivator != null) {
            testFragmentActivator.stop(context);
        }
    }

    /**
     * @return return path to resources needed during testing
     */
    public static String getResourcesPath() {
        return resourcePath;
    }

    private static void setResourcesPath(String path) {
        Activator.resourcePath = path;
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
     * @return TBD
     */
    public static ServiceTracker getPortDataListenerTracker() {
        return portDataListenerTracker;
    }

    /**
     * @param <T>
     *            Return type for the searched service object.
     * @param serviceClass
     *            Service class object registered in framework to look for.
     * @return Service object of given serviceClass. In case of multiple service object registered
     *         with given serviceClass will be returned the one with highest rank.
     */
    @SuppressWarnings("unchecked")
    public static <T> T getServiceObject(Class<T> serviceClass) {
        ServiceReference ref = getBundleContext().getServiceReference(serviceClass.getName());
        if (ref != null) {
            return (T) getBundleContext().getService(ref);
        } else {
            return null;
        }
    }

    /**
     * @param <T>
     *            TBD
     * @param serviceClass
     *            TBD
     * @param filter
     *            TBD
     * @return TBD
     * @throws InvalidSyntaxException
     *             TBD
     */
    @SuppressWarnings("unchecked")
    public static <T> List<T> getServiceObjects(Class<T> serviceClass, String filter) throws InvalidSyntaxException {
        ServiceReference[] refs = getBundleContext().getServiceReferences(serviceClass.getName(), filter);
        List<T> services = new ArrayList<T>();
        if (refs != null) {
            for (ServiceReference ref : refs) {
                services.add((T) getBundleContext().getService(ref));
            }
        }
        return services;
    }
}
