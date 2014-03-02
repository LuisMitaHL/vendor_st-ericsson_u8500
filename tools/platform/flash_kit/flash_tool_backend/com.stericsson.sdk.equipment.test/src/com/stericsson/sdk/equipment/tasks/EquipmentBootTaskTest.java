package com.stericsson.sdk.equipment.tasks;

import static org.junit.Assert.*;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.Hashtable;

import junit.framework.TestCase;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import com.stericsson.sdk.equipment.AbstractLoaderCommunicationEquipment;
import com.stericsson.sdk.equipment.EquipmentProperty;
import com.stericsson.sdk.equipment.EquipmentType;
import com.stericsson.sdk.equipment.IEquipmentProfile;
import com.stericsson.sdk.equipment.io.AbstractPort;
import com.stericsson.sdk.equipment.io.port.PortException;

/**
 * Test class for {@link EquipmentBootTask} class.
 * 
 * @author pkutac01
 * 
 */
public class EquipmentBootTaskTest extends TestCase {

    private class TestPort extends AbstractPort {

        private static final int MAX_READ_ATTEMPTS = 8;

        private byte[] dataToRead = null;

        private int dataToReadIndex = 0;

        private int maxReadAttempts = MAX_READ_ATTEMPTS;

        private int readAttempts = 0;

        public TestPort(String identifier) {
            super(identifier);
        }

        public int read(byte[] buffer, int offset, int length, int timeout) throws PortException {
            if (maxReadAttempts >= 0 && readAttempts > maxReadAttempts) {
                readAttempts = 0;
                throw new PortException("Port timeout.");
            } else {
                readAttempts++;
            }

            if (dataToRead == null) {
                return -1;
            } else {
                int count = 0;
                for (int i = dataToReadIndex; i < dataToRead.length && count < length; i++) {
                    buffer[offset + count] = dataToRead[i];
                    count++;
                }
                dataToReadIndex += count;
                return count;
            }
        }

        public int write(byte[] buffer, int offset, int length, int timeout) throws PortException {
            if (offset + length > buffer.length) {
                return -1;
            }
            return length;
        }

        public void setDataToRead(byte[] pData) {
            dataToRead = pData;
            dataToReadIndex = 0;
            readAttempts = 0;
        }

        public void setMaxReadAttempts(int pMaxReadAttempts) {
            maxReadAttempts = pMaxReadAttempts;
        }

    }

    private static final byte[] ASIC_ID_START = {
        0x05, 0x01, 0x05, 0x01};

    private static final byte[] ASIC_ID_START_V2 = {
        0x05, 0x01, 0x0B, 0x01};

    private static final byte[] ASIC_ID_8500_A0 =
        {
            0x06, 0x01, 0x05, 0x01, 0x00, (byte) 0x85, 0x00, (byte) 0xA0, 0x12, 0x05, 0x00, 0x01, 0x02, 0x03, 0x04,
            0x13, 0x02, 0x00, 0x00, 0x14, 0x05, 0x00, 0x01, 0x02, 0x03, 0x04, 0x15, 0x09, 0x00, 0x01, 0x02, 0x03, 0x04,
            0x05, 0x06, 0x07, 0x08, 0x00, 0x01, 0x00};

    private static final byte[] ASIC_ID_7400_A0 =
        {
            0x06, 0x01, 0x05, 0x01, 0x00, 0x74, 0x00, (byte) 0xA0, 0x12, 0x05, 0x00, 0x01, 0x02, 0x03, 0x04, 0x13,
            0x02, 0x00, 0x00, 0x14, 0x05, 0x00, 0x01, 0x02, 0x03, 0x04, 0x15, 0x09, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
            0x06, 0x07, 0x08, 0x00, 0x01, 0x00};

    private static final byte[] ASIC_ID_7400_B0 =
        {
            0x05, 0x01, 0x0B, 0x01, 0x00, 0x74, 0x00, (byte) 0xB0, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x12, 0x05,
            0x00, 0x01, 0x02, 0x03, 0x04, 0x13, 0x02, 0x00, 0x01, 0x14, 0x05, 0x00, 0x01, 0x02, 0x03, 0x04, 0x15, 0x09,
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};

    private static final byte[] ASIC_ID_INVALID_NUMBER_OF_SUB_BLOCKS = {
        0x7F, 0x01, 0x05, 0x01};

    private static final byte[] ASIC_ID_UNSUPPORTED_CHIP_ID = {
        0x01, 0x01, 0x05, 0x01, 0x00, 0x7F, 0x7F, (byte) 0xA0};

    private static final String UART_BAUD_RATE_PROPERTY_KEY = "UARTBaudRate";

    private static final int UART_BAUD_RATE_PROPERTY_DEFAULT = 115200;

    private Class<EquipmentBootTask> equipmentBootTaskClass = null;

    private Method synchronizeASICIDMethod = null;

    private Method parseASICIDMethod = null;

    private Method changeUARTBaudRateInROMMethod = null;

    private TestPort testPort = null;

    private AbstractLoaderCommunicationEquipment testEquipment = null;

    private EquipmentBootTask equipmentBootTask = null;

    /**
     * 
     * @throws Exception
     *             Exception.
     */
    @Before
    public void setUp() throws Exception {
        equipmentBootTaskClass = EquipmentBootTask.class;
        synchronizeASICIDMethod = null;
        parseASICIDMethod = null;
        changeUARTBaudRateInROMMethod = null;
        try {
            synchronizeASICIDMethod =
                equipmentBootTaskClass.getDeclaredMethod("synchronizeASICID", AbstractPort.class, byte[].class);
            synchronizeASICIDMethod.setAccessible(true);
            parseASICIDMethod = equipmentBootTaskClass.getDeclaredMethod("parseASICID", AbstractPort.class);
            parseASICIDMethod.setAccessible(true);
            changeUARTBaudRateInROMMethod =
                equipmentBootTaskClass.getDeclaredMethod("changeUARTBaudRateInROM", AbstractPort.class);
            changeUARTBaudRateInROMMethod.setAccessible(true);
        } catch (SecurityException e) {
            e.printStackTrace();
            fail();
        } catch (NoSuchMethodException e) {
            e.printStackTrace();
            fail();
        }

        testPort = new TestPort("TestPort");

        testEquipment = new AbstractLoaderCommunicationEquipment(testPort, null) {

            public EquipmentType getEquipmentType() {
                return null;
            }

            @Override
            public String toString() {
                return null;
            }
        };

        equipmentBootTask = new EquipmentBootTask(testEquipment);
    }

    /**
     * 
     * @throws Exception
     *             Exception.
     */
    @After
    public void tearDown() throws Exception {
    }

    /**
     * 
     */
    @Test
    public void testSynchronizeASICIDWithInvalidParameter() {
        byte[] buffer = new byte[ASIC_ID_START.length];

        try {
            int read = (Integer) synchronizeASICIDMethod.invoke(equipmentBootTask, null, buffer);

            fail("Should not get here, read " + read + " bytes.");
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
            fail();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
            fail();
        } catch (InvocationTargetException e) {
            System.out.println(e.getMessage());
        }

        try {
            int read = (Integer) synchronizeASICIDMethod.invoke(equipmentBootTask, testPort, null);

            fail("Should not get here, read " + read + " bytes.");
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
            fail();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
            fail();
        } catch (InvocationTargetException e) {
            System.out.println(e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testSynchronizeASICIDWithInvalidData() {
        byte[] buffer = new byte[ASIC_ID_START.length];

        try {
            testPort.setDataToRead(new byte[] {
                0x01, 0x02, 0x03, 0x04});
            int read = (Integer) synchronizeASICIDMethod.invoke(equipmentBootTask, testPort, buffer);

            fail("Should not get here, read " + read + " bytes.");
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
            fail();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
            fail();
        } catch (InvocationTargetException e) {
            System.out.println(e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testSynchronizeASICIDWithInitialGarbageData() {
        byte[] buffer = new byte[ASIC_ID_START.length];

        try {
            testPort.setDataToRead(new byte[] {
                (byte) 0xFF, (byte) 0xFE, 0x05, 0x01, 0x05, 0x01, 0x02, 0x04});
            int read = (Integer) synchronizeASICIDMethod.invoke(equipmentBootTask, testPort, buffer);

            assertEquals(ASIC_ID_START.length, read);
            assertArrayEquals(ASIC_ID_START, buffer);
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
            fail();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
            fail();
        } catch (InvocationTargetException e) {
            e.printStackTrace();
            fail();
        }
    }

    /**
     * 
     */
    @Test
    public void testSynchronizeASICIDWithMissingFirstByte() {
        byte[] buffer = new byte[ASIC_ID_START_V2.length];

        try {
            testPort.setDataToRead(new byte[] {
                0x01, 0x0B, 0x01, 0x02, 0x04});
            int read = (Integer) synchronizeASICIDMethod.invoke(equipmentBootTask, testPort, buffer);

            assertEquals(ASIC_ID_START_V2.length, read);
            assertArrayEquals(ASIC_ID_START_V2, buffer);
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
            fail();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
            fail();
        } catch (InvocationTargetException e) {
            e.printStackTrace();
            fail();
        }
    }

    /**
     * 
     */
    @Test
    public void testParseASICIDWithSubBlockID8500A0() {
        try {
            testPort.setDataToRead(ASIC_ID_8500_A0);
            testPort.setMaxReadAttempts(-1);
            parseASICIDMethod.invoke(equipmentBootTask, testPort);
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
            fail();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
            fail();
        } catch (InvocationTargetException e) {
            e.printStackTrace();
            fail();
        }
    }

    /**
     * 
     */
    @Test
    public void testParseASICIDWithSubBlockID7400A0() {
        try {
            testPort.setDataToRead(ASIC_ID_7400_A0);
            testPort.setMaxReadAttempts(-1);
            parseASICIDMethod.invoke(equipmentBootTask, testPort);
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
            fail();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
            fail();
        } catch (InvocationTargetException e) {
            e.printStackTrace();
            fail();
        }
    }

    /**
     * 
     */
    @Test
    public void testParseASICIDWithSubBlockID7400B0() {
        try {
            testPort.setDataToRead(ASIC_ID_7400_B0);
            testPort.setMaxReadAttempts(-1);
            parseASICIDMethod.invoke(equipmentBootTask, testPort);
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
            fail();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
            fail();
        } catch (InvocationTargetException e) {
            e.printStackTrace();
            fail();
        }
    }

    /**
     * 
     */
    @Test
    public void testParseASICIDWithInvalidSubBlocksNumber() {
        try {
            testPort.setDataToRead(ASIC_ID_INVALID_NUMBER_OF_SUB_BLOCKS);
            testPort.setMaxReadAttempts(-1);
            parseASICIDMethod.invoke(equipmentBootTask, testPort);

            fail("Should not get here.");
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
            fail();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
            fail();
        } catch (InvocationTargetException e) {
            System.out.println(e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testParseASICIDWithUnsupportedChipID() {
        try {
            testPort.setDataToRead(ASIC_ID_UNSUPPORTED_CHIP_ID);
            testPort.setMaxReadAttempts(-1);
            parseASICIDMethod.invoke(equipmentBootTask, testPort);

            fail("Should not get here.");
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
            fail();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
            fail();
        } catch (InvocationTargetException e) {
            System.out.println(e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testChangeUARTBaudRateInROMWithUSBPort() {
        try {
            testPort = new TestPort("USB0");
            changeUARTBaudRateInROMMethod.invoke(equipmentBootTask, testPort);
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
            fail();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
            fail();
        } catch (InvocationTargetException e) {
            e.printStackTrace();
            fail();
        }
    }

    /**
     * 
     */
    @Test
    public void testChangeUARTBaudRateInROMPropertyIsNullOrFalse() {
        try {
            // Change Baud Rate in ROM property is null.
            changeUARTBaudRateInROMMethod.invoke(equipmentBootTask, testPort);

            // Change Baud Rate in ROM property is false.
            testEquipment.setProperty(EquipmentBootTaskConstants.PROP_CHANGE_BAUD_RATE_IN_ROM.getPropertyName(),
                EquipmentBootTaskConstants.PROP_CHANGE_BAUD_RATE_IN_ROM.getPropertyName(), "FALSE", false);

            changeUARTBaudRateInROMMethod.invoke(equipmentBootTask, testPort);
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
            fail();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
            fail();
        } catch (InvocationTargetException e) {
            e.printStackTrace();
            fail();
        }
    }

    /**
     * 
     */
    @Test
    public void testChangeUARTBaudRateInROMPropertyIsTrueWithMissingBaudRateProperty() {
        try {
            IEquipmentProfile equipmentProfile = new IEquipmentProfile() {

                public Hashtable<String, EquipmentProperty> getSupportedSecurityProperties() {
                    return new Hashtable<String, EquipmentProperty>();
                }

                public String getSofwarePath(String type) {
                    return null;
                }

                public String getProperty(String key) {
                    return null;
                }

                public String getPlatformFilter() {
                    return null;
                }

                public String getAlias() {
                    return null;
                }
            };

            testEquipment = new AbstractLoaderCommunicationEquipment(testPort, equipmentProfile) {

                public EquipmentType getEquipmentType() {
                    return null;
                }

                @Override
                public String toString() {
                    return null;
                }
            };

            // Change Baud Rate in ROM property is true.
            testEquipment.setProperty(EquipmentBootTaskConstants.PROP_CHANGE_BAUD_RATE_IN_ROM.getPropertyName(),
                EquipmentBootTaskConstants.PROP_CHANGE_BAUD_RATE_IN_ROM.getPropertyName(), "TRUE", false);

            equipmentBootTask = new EquipmentBootTask(testEquipment);

            changeUARTBaudRateInROMMethod.invoke(equipmentBootTask, testPort);
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
            fail();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
            fail();
        } catch (InvocationTargetException e) {
            e.printStackTrace();
            fail();
        }
    }

    /**
     * 
     */
    @Test
    public void testChangeUARTBaudRateInROMPropertyIsTrueWithDefaultBaudRateProperty() {
        try {
            IEquipmentProfile equipmentProfile = new IEquipmentProfile() {

                public Hashtable<String, EquipmentProperty> getSupportedSecurityProperties() {
                    return new Hashtable<String, EquipmentProperty>();
                }

                public String getSofwarePath(String type) {
                    return null;
                }

                public String getProperty(String key) {
                    if (EquipmentBootTaskTest.UART_BAUD_RATE_PROPERTY_KEY.equalsIgnoreCase(key)) {
                        return String.valueOf(UART_BAUD_RATE_PROPERTY_DEFAULT);
                    }
                    return null;
                }

                public String getPlatformFilter() {
                    return null;
                }

                public String getAlias() {
                    return null;
                }
            };

            testEquipment = new AbstractLoaderCommunicationEquipment(testPort, equipmentProfile) {

                public EquipmentType getEquipmentType() {
                    return null;
                }

                @Override
                public String toString() {
                    return null;
                }
            };

            // Change Baud Rate in ROM property is true.
            testEquipment.setProperty(EquipmentBootTaskConstants.PROP_CHANGE_BAUD_RATE_IN_ROM.getPropertyName(),
                EquipmentBootTaskConstants.PROP_CHANGE_BAUD_RATE_IN_ROM.getPropertyName(), "TRUE", false);

            equipmentBootTask = new EquipmentBootTask(testEquipment);

            changeUARTBaudRateInROMMethod.invoke(equipmentBootTask, testPort);
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
            fail();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
            fail();
        } catch (InvocationTargetException e) {
            e.printStackTrace();
            fail();
        }
    }

    /**
     * 
     */
    @Test
    public void testChangeUARTBaudRateInROMPropertyIsTrueWithDefinedBaudRateProperty() {
        try {
            IEquipmentProfile equipmentProfile = new IEquipmentProfile() {

                public Hashtable<String, EquipmentProperty> getSupportedSecurityProperties() {
                    return new Hashtable<String, EquipmentProperty>();
                }

                public String getSofwarePath(String type) {
                    return null;
                }

                public String getProperty(String key) {
                    if (EquipmentBootTaskTest.UART_BAUD_RATE_PROPERTY_KEY.equalsIgnoreCase(key)) {
                        return String.valueOf(460800);
                    }
                    return null;
                }

                public String getPlatformFilter() {
                    return null;
                }

                public String getAlias() {
                    return null;
                }
            };

            testEquipment = new AbstractLoaderCommunicationEquipment(testPort, equipmentProfile) {

                public EquipmentType getEquipmentType() {
                    return null;
                }

                @Override
                public String toString() {
                    return null;
                }
            };

            // Change Baud Rate in ROM property is true.
            testEquipment.setProperty(EquipmentBootTaskConstants.PROP_CHANGE_BAUD_RATE_IN_ROM.getPropertyName(),
                EquipmentBootTaskConstants.PROP_CHANGE_BAUD_RATE_IN_ROM.getPropertyName(), "TRUE", false);

            equipmentBootTask = new EquipmentBootTask(testEquipment);

            testPort.setDataToRead(TokenType.CHANGE_BAUD_RATE_ACK_2_0.getTokenValue());
            testPort.setMaxReadAttempts(-1);
            changeUARTBaudRateInROMMethod.invoke(equipmentBootTask, testPort);

            testPort.setDataToRead(TokenType.CHANGE_BAUD_RATE_ACK_2_0_INCORRECT.getTokenValue());
            testPort.setMaxReadAttempts(-1);
            changeUARTBaudRateInROMMethod.invoke(equipmentBootTask, testPort);

            testPort.setDataToRead(new byte[] {
                0x0, 0x0, 0x0, 0x0});
            testPort.setMaxReadAttempts(-1);
            changeUARTBaudRateInROMMethod.invoke(equipmentBootTask, testPort);

            testPort.setDataToRead(new byte[] {});
            testPort.setMaxReadAttempts(-1);
            changeUARTBaudRateInROMMethod.invoke(equipmentBootTask, testPort);
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
            fail();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
            fail();
        } catch (InvocationTargetException e) {
            e.printStackTrace();
            fail();
        }
    }

}
