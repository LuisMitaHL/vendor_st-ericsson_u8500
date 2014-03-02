package com.stericsson.sdk.equipment.u8500.test;

import junit.framework.TestCase;

import org.junit.Test;
import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;

import com.stericsson.sdk.backend.IBackendService;
import com.stericsson.sdk.common.configuration.IConfigurationService;
import com.stericsson.sdk.equipment.bootrom.BootRomTokens;
import com.stericsson.sdk.equipment.io.AbstractPort;
import com.stericsson.sdk.equipment.io.port.IPort;
import com.stericsson.sdk.equipment.io.port.IPortReceiver;
import com.stericsson.sdk.equipment.io.port.PortDeliveryState;
import com.stericsson.sdk.equipment.io.port.PortException;
import com.stericsson.sdk.loader.communication.types.SupportedCommandsType;

/**
 * 
 * @author xtomlju
 */
public class U8500EquipmentBootTaskTest extends TestCase {

    SupportedCommandsType stype = new SupportedCommandsType(0, 1);

    private static final String U8500_PROFILE = "TEST_U8500";

    /**
     * @throws Exception
     *             TBD
     */
    @Test
    public void testU8500EquipmentBoot() throws Exception {

        assertNotNull(BootRomTokens.getTokenDescription(new byte[] {
            (byte) 0x02, (byte) 0x00, (byte) 0x30, (byte) 0xA0}));

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
        IConfigurationService configService =
            (IConfigurationService) Activator.getBundleContext().getService(reference);
        assertNotNull(configService);

        IPort port = new AbstractPort("USB") {

            private int counter = 0;

            byte[] data =
                new byte[] {
                    0x05, 0x01, 0x05, 0x01, 0x00, 0x00, 0x00, 0x00, 0x13, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x00,
                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                    0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                    0x00,

                    (byte) 0x00, (byte) 0x00, (byte) 0x40, (byte) 0xA0,

                    (byte) 0x02, (byte) 0x00, (byte) 0x30, (byte) 0xA0,

                    (byte) 0x00, (byte) 0x00, (byte) 0x30, (byte) 0xA0,

                    (byte) 0x01, (byte) 0x00, (byte) 0x30, (byte) 0xA0};

            boolean once = true;

            @Override
            public int write(byte[] buffer, int offset, int length) throws PortException {
                return length;
            }

            @Override
            public int read(byte[] buffer, int offset, int length) throws PortException {

                try {
                    System.arraycopy(data, counter, buffer, offset, length);
                    counter += length;
                } catch (Exception e) {

                    if (once) {
                        e.printStackTrace();
                        System.out.println("****  counter + offset: " + (counter + offset) + " length: " + length);
                        once = false;
                    }
                }

                return length;
            }

            public int read(byte[] buffer, int offset, int length, int timeout) throws PortException {
                return read(buffer, offset, length);
            }

            public int write(byte[] buffer, int offset, int length, int timeout) throws PortException {
                // TODO Auto-generated method stub
                return 0;
            }
        };

        try {

            Activator.getLoaderCommunicationMock().setResult(stype);

            service.deliverPort(port, PortDeliveryState.SERVICE);
        } catch (PortException e) {
            e.printStackTrace();
            fail(e.getMessage());
        }
    }
}
