package com.stericsson.sdk.equipment.u8500.warm.test;

import junit.framework.TestCase;

import org.junit.Test;
import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;

import com.stericsson.sdk.backend.IBackendService;
import com.stericsson.sdk.common.configuration.IConfigurationService;
import com.stericsson.sdk.equipment.io.port.IPort;
import com.stericsson.sdk.equipment.io.port.IPortReceiver;
import com.stericsson.sdk.equipment.io.port.PortDeliveryState;
import com.stericsson.sdk.equipment.io.port.PortException;

/**
 * 
 * @author esrimpa
 */
public class U8500WarmEquipmentBootTaskTest extends TestCase {

    // SupportedCommandsType stype = new SupportedCommandsType(0, 1);

    private static final String U8500_PROFILE = "TEST_U8500";

    /**
     * @throws Exception
     *             TBD
     */
    @Test
    public void testU8500EquipmentBoot() throws Exception {

        ServiceReference[] references = null;
        try {
            references = Activator.getBundleContext().getAllServiceReferences(IBackendService.class.getName(), null);
        } catch (InvalidSyntaxException e) {
            throw new Exception(e.getMessage());
        }

        if (references == null || references.length == 0) {
            throw new Exception("Cannot find IBackendService.");

        }
        IBackendService backendService = (IBackendService) Activator.getBundleContext().getService(references[0]);

        backendService.setActiveProfile(U8500_PROFILE);

        ServiceReference reference = Activator.getBundleContext().getServiceReference(IPortReceiver.class.getName());
        assertNotNull(reference);

        IPortReceiver service = (IPortReceiver) Activator.getBundleContext().getService(reference);
        assertNotNull(service);

        reference = Activator.getBundleContext().getServiceReference(IConfigurationService.class.getName());
        assertNotNull(reference);

        IPort port = Activator.getPort();

        try {
            service.deliverPort(port, PortDeliveryState.WARM);
        } catch (PortException e) {
            e.printStackTrace();
            fail(e.getMessage());
        }
    }
}
