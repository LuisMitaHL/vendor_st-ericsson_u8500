package com.stericsson.sdk.equipment.l9540;

import java.util.Hashtable;

import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;

import com.stericsson.sdk.common.EnvironmentProperties;
import com.stericsson.sdk.equipment.IPlatform;
import com.stericsson.sdk.equipment.io.port.PortDeliveryState;
import com.stericsson.sdk.equipment.l9540.internal.L9540Platform;
import com.stericsson.sdk.loader.communication.ILoaderCommunicationListener;

/**
 * @author mbocek01
 */
public class Activator implements BundleActivator {

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
     * {@inheritDoc}
     */
    public void start(BundleContext context) throws Exception {
        Activator.setBundleContext(context);
        Hashtable<String, String> properties = new Hashtable<String, String>();
        properties.put(L9540Platform.PLATFORM_PROPERTY, L9540Platform.PLATFORM);
        properties.put(L9540Platform.STATE_PROPERTY, PortDeliveryState.SERVICE.name());
        L9540Platform platform = new L9540Platform();
        context.registerService(IPlatform.class.getName(), platform, properties);
        context.registerService(ILoaderCommunicationListener.class.getName(), platform, properties);

        if (EnvironmentProperties.isRunningInTestMode()) {
            new L9540Platform().createDummyEquipment();
        }
    }

    /**
     * {@inheritDoc}
     */
    public void stop(BundleContext context) throws Exception {
        setBundleContext(null);
    }

}
