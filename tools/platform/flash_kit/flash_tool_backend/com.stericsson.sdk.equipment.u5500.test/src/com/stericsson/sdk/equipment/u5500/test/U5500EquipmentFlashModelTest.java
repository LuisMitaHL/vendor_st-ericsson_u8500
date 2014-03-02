package com.stericsson.sdk.equipment.u5500.test;

import java.util.ArrayList;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.equipment.tasks.EquipmentFlashModel;
import com.stericsson.sdk.equipment.u5500.internal.U5500EquipmentFlashModel;
import com.stericsson.sdk.loader.communication.types.ListDevicesType;
import com.stericsson.sdk.loader.communication.types.subtypes.Device;


/**
 * @author Xxvs0002
 *
 */
public class U5500EquipmentFlashModelTest extends TestCase{

    private EquipmentFlashModel equipmentFlashModel;
    private U5500EquipmentFlashModel u5500equipmentFlashModel;

    private static final String TYPE1 = "u8500";
    private static final String TYPE2 = "u5500";

    private static final String PATH1 = "path1";
    private static final String PATH2 = "path2";
    private static final String PATH3 = "path3";

    private static final long BLOCK_SIZE1 = 120L;
    private static final long BLOCK_SIZE2 = 1200L;
    private static final long BLOCK_SIZE3 = 12000L;

    private static final long START1 = 100L;
    private static final long START2 = 1000L;
    private static final long START3 = 10000L;

    private static final long LENGTH1 = 900L;
    private static final long LENGTH2 = 9000L;
    private static final long LENGTH3 = 90000L;

    private static final String ERROR_DEVICE = "error";
    private static final String TYPE_ERROR = "Unknown";
    private static final long ERROR_CODE = -1;

    private static final String REPORT = "report";

    /**
     * 
     */
    @Test
    public void testEquipmentFlashModel(){
        ListDevicesType listDevicesType = new ListDevicesType(0, 4);
        ArrayList<Device> deviceList = new ArrayList<Device>();

        deviceList.add(new Device(PATH1, 5, TYPE1, 5, BLOCK_SIZE1, START1, LENGTH1));
        deviceList.add(new Device(PATH2, 5, TYPE1, 5, BLOCK_SIZE2, START2, LENGTH2));
        deviceList.add(new Device(PATH3, 5, TYPE2, 5, BLOCK_SIZE3, START3, LENGTH3));

        listDevicesType.addDevice(PATH1, 5, TYPE1, 5, BLOCK_SIZE1, START1, LENGTH1);
        listDevicesType.addDevice(PATH2, 5, TYPE1, 5, BLOCK_SIZE2, START2, LENGTH2);
        listDevicesType.addDevice(PATH3, 5, TYPE2, 5, BLOCK_SIZE3, START3, LENGTH3);

        assertEquals(deviceList.size(), listDevicesType.getDevices().size());

        equipmentFlashModel = new EquipmentFlashModel(listDevicesType, REPORT);

        assertEquals(3, equipmentFlashModel.getFlashDeviceNames(null).length);
        assertEquals(2, equipmentFlashModel.getFlashDeviceNames(TYPE1).length);

        assertEquals(LENGTH1, equipmentFlashModel.getLength(PATH1));
        assertEquals(ERROR_CODE, equipmentFlashModel.getLength(ERROR_DEVICE));

        assertEquals(START3, equipmentFlashModel.getStart(PATH3));
        assertEquals(ERROR_CODE, equipmentFlashModel.getStart(ERROR_DEVICE));

        assertEquals(TYPE1, equipmentFlashModel.getType(PATH2));
        assertEquals(TYPE_ERROR, equipmentFlashModel.getType(ERROR_DEVICE));

        assertEquals(BLOCK_SIZE3, equipmentFlashModel.getBlockSize(PATH3));
        assertEquals(ERROR_CODE, equipmentFlashModel.getBlockSize(ERROR_DEVICE));

        assertEquals(REPORT, equipmentFlashModel.getReport());
    }

    /**
     * 
     */
    @Test
    public void testU5500EquipmentFlashModel(){
        ListDevicesType listDevicesType = new ListDevicesType(0, 4);
        ArrayList<Device> deviceList = new ArrayList<Device>();

        deviceList.add(new Device(PATH1, 5, TYPE1, 5, BLOCK_SIZE1, START1, LENGTH1));
        deviceList.add(new Device(PATH2, 5, TYPE1, 5, BLOCK_SIZE2, START2, LENGTH2));
        deviceList.add(new Device(PATH3, 5, TYPE2, 5, BLOCK_SIZE3, START3, LENGTH3));

        listDevicesType.addDevice(PATH1, 5, TYPE1, 5, BLOCK_SIZE1, START1, LENGTH1);
        listDevicesType.addDevice(PATH2, 5, TYPE1, 5, BLOCK_SIZE2, START2, LENGTH2);
        listDevicesType.addDevice(PATH3, 5, TYPE2, 5, BLOCK_SIZE3, START3, LENGTH3);

        assertEquals(deviceList.size(), listDevicesType.getDevices().size());

        u5500equipmentFlashModel = new U5500EquipmentFlashModel(listDevicesType);

        assertEquals(3, u5500equipmentFlashModel.getFlashDeviceNames(null).length);
        assertEquals(2, u5500equipmentFlashModel.getFlashDeviceNames(TYPE1).length);

        assertEquals(LENGTH1, u5500equipmentFlashModel.getLength(PATH1));
        assertEquals(ERROR_CODE, u5500equipmentFlashModel.getLength(ERROR_DEVICE));

        assertEquals(START3, u5500equipmentFlashModel.getStart(PATH3));
        assertEquals(ERROR_CODE, u5500equipmentFlashModel.getStart(ERROR_DEVICE));

        assertEquals(TYPE1, u5500equipmentFlashModel.getType(PATH2));
        assertEquals(TYPE_ERROR, u5500equipmentFlashModel.getType(ERROR_DEVICE));

        assertEquals(BLOCK_SIZE3, u5500equipmentFlashModel.getBlockSize(PATH3));
        assertEquals(ERROR_CODE, u5500equipmentFlashModel.getBlockSize(ERROR_DEVICE));

        assertEquals(null, u5500equipmentFlashModel.getReport());
    }
}
