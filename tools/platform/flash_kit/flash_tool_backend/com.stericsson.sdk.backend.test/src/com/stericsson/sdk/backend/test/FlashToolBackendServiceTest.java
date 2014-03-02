package com.stericsson.sdk.backend.test;

import java.io.File;
import java.io.IOException;

import junit.framework.TestCase;

import org.junit.Test;
import org.osgi.framework.ServiceReference;

import com.stericsson.sdk.backend.IBackendService;
import com.stericsson.sdk.common.configuration.system.SystemProperties;

/**
 * @author xtomlju
 */
public class FlashToolBackendServiceTest extends TestCase {

    /**
     * 
     */
    @Test
    public void testIsAcceptingClients() {
        ServiceReference reference = Activator.getBundleContext().getServiceReference(IBackendService.class.getName());
        assertNotNull(reference);

        IBackendService service = (IBackendService) Activator.getBundleContext().getService(reference);
        assertNotNull(reference);

        assertFalse(service.isAcceptingClients());

        service.setAcceptClients(true);

        assertTrue(service.isAcceptingClients());

        Activator.getBundleContext().ungetService(reference);
    }

    /**
     * 
     */
    @Test
    public void testIsAcceptingEquipments() {
        ServiceReference reference = Activator.getBundleContext().getServiceReference(IBackendService.class.getName());
        assertNotNull(reference);

        IBackendService service = (IBackendService) Activator.getBundleContext().getService(reference);
        assertNotNull(reference);

        assertFalse(service.isAcceptingEquipments());

        service.setAcceptEquipments(true);

        assertTrue(service.isAcceptingEquipments());
        Activator.getBundleContext().ungetService(reference);
    }

    /**
     * 
     */
    @Test
    public void testLoadConfiguration() {
        ServiceReference reference = Activator.getBundleContext().getServiceReference(IBackendService.class.getName());
        assertNotNull(reference);

        IBackendService service = (IBackendService) Activator.getBundleContext().getService(reference);
        assertNotNull(service);

        String oldValue = System.getProperty(SystemProperties.BACKEND_CONFIGURATIONS_ROOT);

        System.setProperty(SystemProperties.BACKEND_CONFIGURATIONS_ROOT, "Q:\\asdf");
        boolean ee = false;
        try {
            service.loadConfiguration();
        } catch (IOException e) {
            ee = true;
        }
        assertTrue(ee);

        System.setProperty(SystemProperties.BACKEND_CONFIGURATIONS_ROOT, oldValue);

        try {
            service.loadConfiguration();
        } catch (IOException e) {
            fail(e.getMessage());
        }

        Activator.getBundleContext().ungetService(reference);
    }

    /**
     * @throws Exception
     *             TBD
     */
    @Test
    public void testSaveConfiguration() throws Exception {
        ServiceReference reference = Activator.getBundleContext().getServiceReference(IBackendService.class.getName());
        assertNotNull(reference);

        IBackendService service = (IBackendService) Activator.getBundleContext().getService(reference);
        assertNotNull(reference);
        File configFile = null;

        try {
            configFile = File.createTempFile("test", ".mesp");
        } catch (IOException e) {
            throw new Exception(e.getMessage());
        }
        String oldValue = System.getProperty(SystemProperties.BACKEND_CONFIGURATIONS_ROOT);

        System.setProperty(SystemProperties.BACKEND_CONFIGURATIONS_ROOT, configFile.getParent());
        service.saveConfiguration();

        System.setProperty(SystemProperties.BACKEND_CONFIGURATIONS_ROOT, oldValue);

        assertTrue(configFile.exists());

        assertTrue(configFile.delete());

        Activator.getBundleContext().ungetService(reference);
    }

}
