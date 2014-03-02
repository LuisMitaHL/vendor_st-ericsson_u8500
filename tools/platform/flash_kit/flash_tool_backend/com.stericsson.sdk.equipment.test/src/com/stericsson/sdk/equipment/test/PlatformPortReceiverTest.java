package com.stericsson.sdk.equipment.test;

import java.util.Hashtable;

import junit.framework.TestCase;

import org.junit.Test;
import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;

import com.stericsson.sdk.equipment.IEquipment;
import com.stericsson.sdk.equipment.IEquipmentProfile;
import com.stericsson.sdk.equipment.IPlatform;
import com.stericsson.sdk.equipment.io.AbstractPort;
import com.stericsson.sdk.equipment.io.port.IPort;
import com.stericsson.sdk.equipment.io.port.IPortProvider;
import com.stericsson.sdk.equipment.io.port.IPortReceiver;
import com.stericsson.sdk.equipment.io.port.PortDeliveryState;
import com.stericsson.sdk.equipment.io.port.PortException;

/**
 * @author xtomlju
 */
public class PlatformPortReceiverTest extends TestCase implements IPortProvider, IPlatform {

    private static final String TEST_PORT_NAME = "TEST";

    /**
     * @throws PortException
     *             TBD
     * @throws Exception
     *             TBD
     */
    @Test
    public void testDeliverPort() throws PortException, Exception {

        Hashtable<String, String> properties = new Hashtable<String, String>();
        properties.put("platform", "test");
        TestFragmentActivator.getBundleContext().registerService(IPlatform.class.getName(), this, properties);

        final byte[] byteArray = new byte[] {
            1, 2, 3, 4, 5};

        IPort port = new AbstractPort(TEST_PORT_NAME) {

            public int write(byte[] buffer, int offset, int length) {
                return -1;
            }

            public int read(byte[] buffer, int offset, int length) {
                if (length > byteArray.length) {
                    length = byteArray.length;
                }
                System.arraycopy(byteArray, 0, buffer, offset, length);
                return length;
            }

            public int read(byte[] buffer, int offset, int length, int timeout) throws PortException {
                // TODO Auto-generated method stub
                return 0;
            }

            public int write(byte[] buffer, int offset, int length, int timeout) throws PortException {
                // TODO Auto-generated method stub
                return 0;
            }

        };

        ServiceReference[] references = null;
        try {
            references = TestFragmentActivator.getBundleContext().getServiceReferences(IPortReceiver.class.getName(), null);
        } catch (InvalidSyntaxException e) {
            throw new Exception(e.getMessage());
        }
        if (references == null || references.length < 1) {
            throw new Exception("Failed to get port receiver!");
        }
        IPortReceiver receiver = (IPortReceiver) TestFragmentActivator.getBundleContext().getService(references[0]);
        assertNotNull(receiver);
        receiver.deliverPort(port, PortDeliveryState.SERVICE);
    }

    /**
     * {@inheritDoc}
     */
    public String getIdentifier() {
        return "PlatformPortReceiverTestProvider";
    }

    /**
     * {@inheritDoc}
     */
    public boolean isTriggered() {
        return false;
    }

    /**
     * {@inheritDoc}
     */
    public void release() {
    }

    /**
     * {@inheritDoc}
     */
    public void trigger() throws PortException {
    }

    /**
     * {@inheritDoc}
     */
    public void createEquipment(IPort port, IEquipmentProfile profile, boolean bootME) {
        System.out.println("Equipment created");
    }

    /**
     * {@inheritDoc}
     */
    public void destroyEquipment(IEquipment equipment) {
        // TODO Auto-generated method stub
    }

    /**
     * {@inheritDoc}
     */
    public void registerEquipmentProperties(IEquipment equipment) {
        // TODO Auto-generated method stub

    }
}
