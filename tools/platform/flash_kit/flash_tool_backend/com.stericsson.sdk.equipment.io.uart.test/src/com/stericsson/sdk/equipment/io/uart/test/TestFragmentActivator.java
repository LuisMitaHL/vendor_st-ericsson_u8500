package com.stericsson.sdk.equipment.io.uart.test;

import java.util.ArrayList;
import java.util.Hashtable;
import java.util.List;

import junit.framework.TestSuite;

import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;
import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;

import com.stericsson.sdk.equipment.io.uart.internal.SerialPortConfigurationTest;
import com.stericsson.sdk.equipment.io.uart.test.internal.UARTTester;

/**
 * @author rohacmic
 * @author Vit Sykala <vit.sykala@tieto.com>
 */
public class TestFragmentActivator implements BundleActivator {

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
        TestFragmentActivator.setBundleContext(context);
        TestSuite testSuiteService = new TestSuite();

        testSuiteService.addTest(new UARTTester());
        testSuiteService.addTest(new SerialPortConfigurationTest());

        Hashtable<String, String> properties = new Hashtable<String, String>();
        properties.put("bundle.name",
                "com.stericsson.sdk.equipment.io.uart");
        context.registerService(TestSuite.class.getName(), testSuiteService,
                properties);
    }

    /**
     * {@inheritDoc}
     */
    public void stop(BundleContext context) throws Exception {
    }

    /**
     * @param <T>
     *            Return type for the searched service object.
     * @param serviceClass
     *            Service class object registered in framework to look for.
     * @return Service object of given serviceClass. In case of multiple service
     *         object registered with given serviceClass will be returned the
     *         one with highest rank.
     */
    @SuppressWarnings("unchecked")
    public static <T> T getServiceObject(Class<T> serviceClass) {
        ServiceReference ref = getBundleContext().getServiceReference(
                serviceClass.getName());
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
    public static <T> List<T> getServiceObjects(Class<T> serviceClass,
            String filter) throws InvalidSyntaxException {
        ServiceReference[] refs = getBundleContext().getServiceReferences(
                serviceClass.getName(), filter);
        List<T> services = new ArrayList<T>();
        if (refs != null) {
            for (ServiceReference ref : refs) {
                services.add((T) getBundleContext().getService(ref));
            }
        }
        return services;
    }

}
