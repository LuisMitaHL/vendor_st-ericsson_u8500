package com.stericsson.sdk.equipment.u8500.test;

import java.util.Hashtable;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.equipment.EquipmentProperty;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.IEquipmentProfile;
import com.stericsson.sdk.equipment.io.AbstractPort;
import com.stericsson.sdk.equipment.io.port.IPort;
import com.stericsson.sdk.equipment.io.port.PortException;
import com.stericsson.sdk.equipment.tasks.EquipmentChangeBaudRateTask;
import com.stericsson.sdk.equipment.u8500.internal.U8500Equipment;

/**
 * @author xtomzap
 *
 */
public class EquipmentChangeBaudRateTest extends TestCase {

    private EquipmentChangeBaudRateTask task;

    private static final int RESULT_SUCCESS = 0;

    /**
     * 
     */
    @Test
    public void testInit() {
        IPort port = createPort();
        IEquipmentProfile profile = createProfile();

        U8500Equipment eq = new U8500Equipment(port, null);
        eq.setProfile(profile);

        Activator.getLoaderCommunicationMock().setResult(RESULT_SUCCESS);

        task = new EquipmentChangeBaudRateTask(eq);
        task.setArguments(null);
        EquipmentTaskResult result = task.execute();

        assertEquals(RESULT_SUCCESS, result.getResultCode());
    }

    private IEquipmentProfile createProfile() {
        IEquipmentProfile profile = new IEquipmentProfile() {

            public Hashtable<String, EquipmentProperty> getSupportedSecurityProperties() {
                return new Hashtable<String, EquipmentProperty>();
            }

            public String getSofwarePath(String type) {
                return null;
            }

            public String getProperty(String key) {
                return "666";
            }

            public String getPlatformFilter() {
                return null;
            }

            public String getAlias() {
                return null;
            }
        };
        return profile;
    }

    private IPort createPort() {
        IPort port = new AbstractPort("UARTTEST") {

            public int write(byte[] buffer, int offset, int length, int timeout) throws PortException {
                return 0;
            }

            public int read(byte[] buffer, int offset, int length, int timeout) throws PortException {
                return 0;
            }
        };
        return port;
    }
}
