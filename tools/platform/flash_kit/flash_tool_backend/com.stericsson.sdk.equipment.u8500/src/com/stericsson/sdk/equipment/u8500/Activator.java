package com.stericsson.sdk.equipment.u8500;

import java.util.Hashtable;

import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;

import com.stericsson.sdk.common.EnvironmentProperties;
import com.stericsson.sdk.equipment.IPlatform;
import com.stericsson.sdk.equipment.io.port.PortDeliveryState;
import com.stericsson.sdk.equipment.u8500.internal.U8500Platform;
import com.stericsson.sdk.loader.communication.ILoaderCommunicationListener;

/**
 * @author xtomlju
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
        properties.put(U8500Platform.PLATFORM_PROPERTY, U8500Platform.PLATFORM);
        properties.put(U8500Platform.STATE_PROPERTY, PortDeliveryState.SERVICE.name());
        U8500Platform platform = new U8500Platform();
        context.registerService(IPlatform.class.getName(), platform, properties);
        context.registerService(ILoaderCommunicationListener.class.getName(), platform, properties);

        if(EnvironmentProperties.isRunningInTestMode()) {
            new U8500Platform().createDummyEquipment();
        }
    }

    /**
     * {@inheritDoc}
     */
    public void stop(BundleContext context) throws Exception {
        setBundleContext(null);
    }

}
