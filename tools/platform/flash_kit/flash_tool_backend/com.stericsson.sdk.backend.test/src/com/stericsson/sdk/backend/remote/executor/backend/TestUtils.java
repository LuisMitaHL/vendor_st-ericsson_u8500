/**
 * 
 */
package com.stericsson.sdk.backend.remote.executor.backend;

import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;

import com.stericsson.sdk.backend.IBackendService;
import com.stericsson.sdk.equipment.Activator;
import com.stericsson.sdk.equipment.IEquipmentProfile;
import com.stericsson.sdk.equipment.IEquipmentProfileManagerService;
import com.stericsson.sdk.equipment.io.AbstractPort;
import com.stericsson.sdk.equipment.io.port.IPort;
import com.stericsson.sdk.equipment.io.port.PortException;

import junit.framework.TestCase;

/**
 * @author cizovhel
 * 
 */
public class TestUtils extends TestCase {
    /** TEST_U8500 profile */
    public static final String TEST_U8500 = "TEST_U8500";

    /** TEST profile */
    public static final String TEST = "TEST";

    /** TEST_U8500 equipment ID */
    public static final String PORT = "TEST";

    /**
     * Sets active profile for testing
     * 
     * @param requiredProfileAlias
     *            profile alias
     * @throws Exception
     *             TBD
     * */
    public void setActiveProfile(String requiredProfileAlias) throws Exception {
        IEquipmentProfileManagerService profileManagerService = getProfileManagerService();

        if (profileManagerService != null) {
            IEquipmentProfile requiredProfile = profileManagerService.getProfile(requiredProfileAlias, true);
            if (requiredProfile == null) {
                throw new Exception("Cannot set profile " + requiredProfileAlias
                    + " as active! Profile does not exist in available profiles list.");
            }

            IBackendService backendService = getBackendService();
            if (backendService != null) {
                backendService.setActiveProfile(requiredProfileAlias);
            }
        }
    }

    /**
     * Creates abstract port for testing
     * 
     * @param pEquipmentId
     *            equipment id
     * @return created abstract port
     * */
    public IPort createAbstractPort(String pEquipmentId) {
        IPort port = new AbstractPort(pEquipmentId) {

            private int counter = 0;

            @Override
            public int write(byte[] buffer, int offset, int length) throws PortException {
                return length;
            }

            @Override
            public int read(byte[] buffer, int offset, int length) throws PortException {
                counter++;
                int result = 0;
                if (counter == 1) {
                    System.arraycopy(new byte[] {
                        0x05, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00}, 0, buffer, offset, 65);
                    result = 65;
                } else if (counter == 2) {
                    System.arraycopy(new byte[] {
                        (byte) 0x00, (byte) 0x00, (byte) 0x40, (byte) 0xA0}, 0, buffer, offset, 4);
                    result = 4;
                } else if (counter == 3) {
                    System.arraycopy(new byte[] {
                        (byte) 0x02, (byte) 0x00, (byte) 0x30, (byte) 0xA0}, 0, buffer, offset, 4);
                    result = 0;
                } else if (counter == 4) {
                    result = 4;
                    System.arraycopy(new byte[] {
                        (byte) 0x00, (byte) 0x00, (byte) 0x30, (byte) 0xA0}, 0, buffer, offset, 4);
                } else if (counter == 5) {
                    System.arraycopy(new byte[] {
                        (byte) 0x01, (byte) 0x00, (byte) 0x30, (byte) 0xA0}, 0, buffer, offset, 4);
                    result = 4;
                } else {
                    fail("Counter: " + counter + ", blen: " + buffer.length);
                }

                return result;
            }

            public int read(byte[] buffer, int offset, int length, int timeout) throws PortException {
                return read(buffer, offset, length);
            }

            public int write(byte[] buffer, int offset, int length, int timeout) throws PortException {
                // TODO Auto-generated method stub
                return 0;
            }
        };
        return port;
    }

    private IEquipmentProfileManagerService getProfileManagerService() throws Exception {
        ServiceReference[] references = null;

        try {
            references =
                Activator.getBundleContext().getAllServiceReferences(IEquipmentProfileManagerService.class.getName(),
                    "(type=profile)");
        } catch (InvalidSyntaxException e) {
            throw new Exception(e.getMessage());
        }

        if (references == null || references.length == 0) {
            throw new Exception("Cannot find IEquipmentProfileManagerService.");

        }
        IEquipmentProfileManagerService profileManagerService =
            (IEquipmentProfileManagerService) Activator.getBundleContext().getService(references[0]);

        return profileManagerService;
    }

    private IBackendService getBackendService() throws Exception {
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
        return backendService;
    }

}
