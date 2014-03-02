package com.stericsson.sdk.equipment.io.uart;

import java.util.ArrayList;
import java.util.Dictionary;
import java.util.Hashtable;
import java.util.List;

import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceRegistration;

import com.stericsson.sdk.common.EnvironmentProperties;
import com.stericsson.sdk.common.configuration.IConfigurationServiceListener;
import com.stericsson.sdk.equipment.io.port.IPortProviderTester;
import com.stericsson.sdk.equipment.io.uart.internal.SerialPort;
import com.stericsson.sdk.equipment.io.uart.internal.SerialPortHandler;

/**
 * @author rohacmic
 * 
 */
public class Activator implements BundleActivator {

    private final List<ServiceRegistration> registrations = new ArrayList<ServiceRegistration>();

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

    /**
     * {@inheritDoc}
     */
    public void start(BundleContext ctx) throws Exception {
        Activator.setBundleContext(ctx);
        boolean testingMode = EnvironmentProperties.isRunningInTestMode();

        for (SerialPort port : SerialPortHandler.getInstance().getSerialPorts()) {
            registerService(IConfigurationServiceListener.class, port, null);
            if (testingMode) {
                Dictionary<String, String> settings = new Hashtable<String, String>();
                settings.put("port.provider.tester", "UART");
                registerService(IPortProviderTester.class, port, settings);
            }
        }

        try {
            Object tmpFragmentActivator =
                Class.forName("com.stericsson.sdk.equipment.io.uart.test.TestFragmentActivator").newInstance();
            if (tmpFragmentActivator instanceof BundleActivator) {
                testFragmentActivator = (BundleActivator) tmpFragmentActivator;
                testFragmentActivator.start(ctx);
            }
        } catch (ClassNotFoundException e) {
            testFragmentActivator = null;
        }

    }

    /**
     * {@inheritDoc}
     */
    public void stop(BundleContext ctx) throws Exception {
        for (ServiceRegistration regObj : registrations) {
            regObj.unregister();
        }
        if (testFragmentActivator != null) {
            testFragmentActivator.stop(ctx);
        }
    }

    private void registerService(Class<?> clazz, Object service, Dictionary<String, String> pSetting) {
        registrations.add(getBundleContext().registerService(clazz.getName(), service, pSetting));
    }
}
