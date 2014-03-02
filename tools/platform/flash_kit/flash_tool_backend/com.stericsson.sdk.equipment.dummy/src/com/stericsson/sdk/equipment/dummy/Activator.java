package com.stericsson.sdk.equipment.dummy;

import java.util.Hashtable;

import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;

import com.stericsson.sdk.equipment.dummy.internal.DummyPlatform;
import com.stericsson.sdk.equipment.dummy.internal.DummyPort;
import com.stericsson.sdk.equipment.dummy.internal.DummyProfile;
import com.stericsson.sdk.equipment.io.port.IPort;
import com.stericsson.sdk.equipment.io.port.PortDeliveryState;

/**
 * 
 * @author xolabju
 * 
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
        // create platform
        Hashtable<String, String> properties = new Hashtable<String, String>();
        properties.put(DummyPlatform.PLATFORM_PROPERTY, DummyPlatform.PLATFORM);
        properties.put(DummyPlatform.STATE_PROPERTY, PortDeliveryState.SERVICE.name());
        DummyPlatform platform = new DummyPlatform();

        // create equipment
        IPort dummyPortA = new DummyPort("PortA");
        IPort dummyPortB = new DummyPort("PortB");
        platform.createEquipment(dummyPortA, new DummyProfile(dummyPortA), true);
        platform.createEquipment(dummyPortB, new DummyProfile(dummyPortB), true);
    }

    /**
     * {@inheritDoc}
     */
    public void stop(BundleContext context) throws Exception {
    }

}
