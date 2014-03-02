package com.stericsson.sdk.loader.communication.test;

import static org.junit.Assert.assertArrayEquals;
import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.loader.communication.types.AuthenticationType;
import com.stericsson.sdk.loader.communication.types.OTPReadBitsType;
import com.stericsson.sdk.loader.communication.types.PSReadGlobalDataUnitType;
import com.stericsson.sdk.loader.communication.types.SecurityPropertiesType;
import com.stericsson.sdk.loader.communication.types.SupportedCommandsType;
import com.stericsson.sdk.loader.communication.types.TimeoutType;
import com.stericsson.sdk.loader.communication.types.VolumePropertiesType;

/**
 * @author xdancho
 */
public class DataTypesTest2 extends TestCase {

    static final int FILE_TYPE_MASK = 0xF000; // 0170000;

    static final int ACCESS_OWNER_MASK = 0x01C0;

    static final int ACCESS_GROUP_MASK = 0x0038;

    static final int ACCESS_OTHERS_MASK = 0x0007;

    static final int UNIX_DOMAIN_SOCKET = 0xC000; // 0140000;

    static final int SYMBOLIC_LINK = 0xA000; // 0120000;

    static final int REGULAR_FILE = 0x8000; // 0100000;

    static final int BLOCK_DEVICE = 0x6000; // 0060000;

    static final int DIRECTORY_FILE = 0x4000; // 0040000;

    static final int CHARACTER_DEVICE = 0x2000; // 0020000;

    static final int NAMED_PIPE = 0x1000; // 0010000;

    /**
     * test OTPReadBitsType
     */
    @Test
    public void testOTPReadBitsType() {

        byte[] data = {
            0x01, 0x02, 0x03};
        byte[] lock = {
            0x00, 0x01, 0x01};

        OTPReadBitsType t = new OTPReadBitsType(0, data, lock);

        assertArrayEquals(data, t.getDataBits());
        assertArrayEquals(lock, t.getLockStatusBits());

    }

    /**
     * test PSReadGlobalDataUnitType
     */
    @Test
    public void testPSReadGlobalDataUnitType() {

        byte[] data = {
            0x01, 0x02, 0x03};
        PSReadGlobalDataUnitType t = new PSReadGlobalDataUnitType(0, data);
        assertArrayEquals(data, t.getData());

    }

    /**
     * test SecurityPropertiesType
     */
    @Test
    public void testSecurityPropertiesType() {
        byte[] data = {
            0x01, 0x02, 0x03};
        SecurityPropertiesType t = new SecurityPropertiesType(0, data);
        assertArrayEquals(data, t.getBuffer());

    }

    /**
     * test SupportedComandsType
     */
    @Test
    public void testSupportedComandsType() {
        SupportedCommandsType t = new SupportedCommandsType(0, 1);

        assertEquals(0, t.getSupportedCommands().size());

        t.addSupportedCommand(1, 0, 2);
        t.addSupportedCommand(2, 1, 3);

        assertEquals(0, t.getSupportedCommands().get(0).getGroup());
        assertEquals(1, t.getSupportedCommands().get(0).getCommand());
        assertEquals(2, t.getSupportedCommands().get(0).getPermitted());

        assertEquals(1, t.getSupportedCommands().get(1).getGroup());
        assertEquals(2, t.getSupportedCommands().get(1).getCommand());
        assertEquals(3, t.getSupportedCommands().get(1).getPermitted());

    }

    /**
     * test VolumePropertiesType
     */
    @Test
    public void testVolumePropertiesType() {

        String type = "type";
        int typeSize = 4;
        int size = 100;
        int free = 50;

        VolumePropertiesType t = new VolumePropertiesType(0, type, typeSize, size, free);

        assertEquals(type, t.getFsType());
        assertEquals(size, t.getSize());
        assertEquals(free, t.getFree());

        t = new VolumePropertiesType(0, type, 0, size, free);

        assertEquals("", t.getFsType());

    }

    /**
     * test initTimeoutType
     */
    @Test
    public void testTimeoutType() {

        TimeoutType tt = new TimeoutType(0, 1, 2, 3, 4);

        assertEquals(1, tt.getCommandAck());
        assertEquals(2, tt.getBulkReadReq());
        assertEquals(3, tt.getBulkReadAck());
        assertEquals(4, tt.getBulkDR());

        tt.setCommandAck(11);
        tt.setBulkReadReq(22);
        tt.setBulkReadAck(33);
        tt.setBulkDR(44);

        assertEquals(11, tt.getCommandAck());
        assertEquals(22, tt.getBulkReadReq());
        assertEquals(33, tt.getBulkReadAck());
        assertEquals(44, tt.getBulkDR());

    }

    /**
     * test AuthenticationType
     */
    @Test
    public void testAuthenticationType() {
        byte[] cert = new byte[] {
            (byte) 0xBA, (byte) 0xBE};
        // for simlock keys
        AuthenticationType at1 = new AuthenticationType(0);
        assertNotNull(at1);

        // for certs
        AuthenticationType at2 = new AuthenticationType(0, 2, cert);

        assertArrayEquals(cert, at2.getChallengeData());

    }

}
