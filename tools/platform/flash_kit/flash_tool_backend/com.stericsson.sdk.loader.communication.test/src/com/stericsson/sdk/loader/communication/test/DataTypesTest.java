package com.stericsson.sdk.loader.communication.test;

import static org.junit.Assert.assertArrayEquals;
import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.loader.communication.types.DomainType;
import com.stericsson.sdk.loader.communication.types.FSPropertiesType;
import com.stericsson.sdk.loader.communication.types.ListDevicesType;
import com.stericsson.sdk.loader.communication.types.ListDirectoryType;
import com.stericsson.sdk.loader.communication.types.LoaderStartupType;
import com.stericsson.sdk.loader.communication.types.ModeType;
import com.stericsson.sdk.loader.communication.types.OTPReadBitsType;

/**
 * @author xdancho
 */
public class DataTypesTest extends TestCase {

    private static final int CREATE_TIME = 1000;

    private static final int ACCESS_TIME = 2000;

    private static final int MOD_TIME = 3000;

    private static final long SIZE = 4000;

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
     * test domain type
     */
    @Test
    public void testDomainType() {
        DomainType t = new DomainType(0, 1);
        assertEquals(0, t.getStatus());
        assertEquals(1, t.getWrittenDomain());

    }

    /**
     * test properties type
     */
    @Test
    public void testFSPropertiesType() {

        int mode = DIRECTORY_FILE + ACCESS_GROUP_MASK;

        FSPropertiesType t = new FSPropertiesType(0, mode, SIZE, MOD_TIME, ACCESS_TIME, CREATE_TIME);

        assertEquals(0, t.getStatus());
        assertEquals(mode, t.getMode().getModeValue());
        assertEquals(SIZE, t.getSize());
        assertEquals(CREATE_TIME, t.getCreateTime());
        assertEquals(ACCESS_TIME, t.getAccessTime());
        assertEquals(MOD_TIME, t.getModificationTime());

        // modeType fail
        t = new FSPropertiesType(0, 0, SIZE, MOD_TIME, ACCESS_TIME, CREATE_TIME);

        assertNull(t.getMode());

    }

    /**
     * test ListDevicesType
     */
    @Test
    public void testListDevicesType() {

        String path = "path";
        int pathSize = 4;
        int corruptPathSize = 5;
        String type = "type";
        int typeSize = 4;
        int corruptTypeSize = 0;
        String path2 = "path2";
        int pathSize2 = 5;
        String type2 = "type2";
        int typeSize2 = 5;
        int start = 0;
        int length = 100;
        int blockSize = 10;

        ListDevicesType t = new ListDevicesType(0, 3);
        assertEquals(0, t.getDevices().size());
        t.addDevice(path, pathSize, type, typeSize, blockSize, start, length);
        assertEquals(path, t.getDevices().get(0).getPath());
        assertEquals(type, t.getDevices().get(0).getType());
        assertEquals(blockSize, t.getDevices().get(0).getBlockSize());
        assertEquals(start, t.getDevices().get(0).getStart());
        assertEquals(length, t.getDevices().get(0).getLength());

        t.addDevice(path2, pathSize2, type2, typeSize2, blockSize, start, length);
        assertEquals(path2, t.getDevices().get(1).getPath());
        assertEquals(type2, t.getDevices().get(1).getType());
        assertEquals(blockSize, t.getDevices().get(1).getBlockSize());
        assertEquals(start, t.getDevices().get(1).getStart());
        assertEquals(length, t.getDevices().get(1).getLength());

        t.addDevice(path, corruptPathSize, type, corruptTypeSize, blockSize, start, length);
        assertEquals("", t.getDevices().get(2).getPath());
        assertEquals("", t.getDevices().get(2).getType());
        assertEquals(blockSize, t.getDevices().get(2).getBlockSize());
        assertEquals(start, t.getDevices().get(2).getStart());
        assertEquals(length, t.getDevices().get(2).getLength());

    }

    /**
     * test ListDirectoryType
     */
    @Test
    public void testListDirectoryType() {

        String name = "name";
        int nameSize = 4;
        String name2 = "name2";
        int nameSize2 = 5;
        int corruptSize = 0;
        int size = 100;
        int mode = DIRECTORY_FILE + ACCESS_GROUP_MASK;

        ListDirectoryType t = new ListDirectoryType(0, 4);

        t.addDirectory(name, nameSize, size, mode, ACCESS_TIME);
        t.addDirectory(name2, nameSize2, size, mode, ACCESS_TIME);
        t.addDirectory(name, corruptSize, size, mode, ACCESS_TIME);
        // modeType fail
        t.addDirectory(name, nameSize, size, 0, ACCESS_TIME);

        assertNull(t.getDirectorys().get(3).getMode());

        assertEquals(name, t.getDirectorys().get(0).getName());
        assertEquals(size, t.getDirectorys().get(0).getSize());
        assertEquals(mode, t.getDirectorys().get(0).getMode().getModeValue());
        assertEquals(ACCESS_TIME, t.getDirectorys().get(0).getTime());

        assertEquals(name2, t.getDirectorys().get(1).getName());
        assertEquals(size, t.getDirectorys().get(1).getSize());
        assertEquals(mode, t.getDirectorys().get(1).getMode().getModeValue());
        assertEquals(ACCESS_TIME, t.getDirectorys().get(1).getTime());

        assertEquals("", t.getDirectorys().get(2).getName());
        assertEquals(size, t.getDirectorys().get(2).getSize());
        assertEquals(mode, t.getDirectorys().get(2).getMode().getModeValue());
        assertEquals(ACCESS_TIME, t.getDirectorys().get(2).getTime());
    }

    /**
     * test loaderStartupType
     */
    @Test
    public void testLoaderStartupType() {

        String version = "version";
        String protocol = "protocol";
        int verSize = 7;
        int protSize = 8;
        int corruptSize = 9;

        LoaderStartupType t = new LoaderStartupType(0, version, corruptSize, protocol, protSize);
        assertNotSame(version, t.getLoaderVersion());
        assertEquals("", t.getLoaderVersion());

        assertEquals(protocol, t.getProtocolVersion());

        t = new LoaderStartupType(0, version, verSize, protocol, corruptSize);
        assertEquals(version, t.getLoaderVersion());
        assertNotSame(protocol, t.getProtocolVersion());
        assertEquals("", t.getProtocolVersion());

        t = new LoaderStartupType(0, version, verSize, protocol, protSize);
        assertEquals(version, t.getLoaderVersion());
        assertEquals(protocol, t.getProtocolVersion());

    }

    /**
     * test ModeType
     */
    @Test
    public void testModeType() {
        ModeType t = null;
        Exception ex = null;
        int accessRWX = 7; // 111
        int accessRX = 5; // 101
        int accessR = 4; // 100
        int accessAllRX = 365; // 101101101
        int accessAllR = 292; // 100100100

        try {
            t = new ModeType(REGULAR_FILE + ACCESS_GROUP_MASK + ACCESS_OWNER_MASK + ACCESS_OTHERS_MASK);
        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }

        assertEquals(accessRWX, t.getAccessPermissionGroup());
        assertEquals(accessRWX, t.getAccessPermissionOwner());
        assertEquals(accessRWX, t.getAccessPermissionOthers());
        assertEquals(ModeType.REGULAR_FILE_STR, t.getType());

        try {
            t = new ModeType(0);
        } catch (Exception e) {
            ex = e;
        }

        assertNotNull(ex);

        try {
            t = new ModeType(UNIX_DOMAIN_SOCKET);

            assertEquals(ModeType.UNIX_DOMAIN_SOCKET_STR, t.getType());

            t = new ModeType(SYMBOLIC_LINK);

            assertEquals(ModeType.SYMBOLIC_LINK_STR, t.getType());

            t = new ModeType(BLOCK_DEVICE);

            assertEquals(ModeType.BLOCK_DEVICE_STR, t.getType());

            t = new ModeType(DIRECTORY_FILE);

            assertEquals(ModeType.DIRECTORY_FILE_STR, t.getType());

            t = new ModeType(CHARACTER_DEVICE);

            assertEquals(ModeType.CHARACTER_DEVICE_STR, t.getType());

            t = new ModeType(NAMED_PIPE);

            assertEquals(ModeType.NAMED_PIPE_STR, t.getType());

            t = new ModeType(SYMBOLIC_LINK + accessAllRX);
        } catch (Exception e) {
            // TODO Auto-generated catch block
            fail("Exception when creating ModeType");
        }

        assertEquals(accessRX, t.getAccessPermissionGroup());
        assertEquals(accessRX, t.getAccessPermissionOthers());
        assertEquals(accessRX, t.getAccessPermissionOwner());

        try {
            t = new ModeType(SYMBOLIC_LINK + accessAllR);
        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }

        assertEquals(accessR, t.getAccessPermissionGroup());
        assertEquals(accessR, t.getAccessPermissionOthers());
        assertEquals(accessR, t.getAccessPermissionOwner());

    }

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

}
