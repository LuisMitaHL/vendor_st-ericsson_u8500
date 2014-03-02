package com.stericsson.sdk.equipment.io.uart.internal;

import junit.framework.TestCase;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import com.stericsson.sdk.equipment.io.uart.internal.SerialPortConfiguration.BAUD_RATE;
import com.stericsson.sdk.equipment.io.uart.internal.SerialPortConfiguration.DATA_BITS;
import com.stericsson.sdk.equipment.io.uart.internal.SerialPortConfiguration.DTR_CIRCUIT;
import com.stericsson.sdk.equipment.io.uart.internal.SerialPortConfiguration.ON_OFF;
import com.stericsson.sdk.equipment.io.uart.internal.SerialPortConfiguration.PARITY;
import com.stericsson.sdk.equipment.io.uart.internal.SerialPortConfiguration.RTS_CIRCUIT;
import com.stericsson.sdk.equipment.io.uart.internal.SerialPortConfiguration.STOP_BITS;

/**
 * @author Vit Sykala <vit.sykala@tieto.com>
 */
public class SerialPortConfigurationTest extends TestCase {

    private SerialPortConfiguration tested;

    private SerialPortConfiguration other;

    /**
     * @throws java.lang.Exception .
     */
    @Before
    public void setUp() throws Exception {
        tested = new SerialPortConfiguration();
        other = new SerialPortConfiguration();
    }

    /**
     * @throws java.lang.Exception .
     */
    @After
    public void tearDown() throws Exception {
    }

    /**
     * Test method for
     * {@link com.stericsson.sdk.equipment.io.uart.internal.SerialPortConfiguration#equals(java.lang.Object)}
     * and {@link com.stericsson.sdk.equipment.io.uart.internal.SerialPortConfiguration#hashCode()}.
     */
    @Test
    public void testEqualsHash() {
        assertTrue(tested.equals(other));
        assertTrue(tested.equals(tested));
        assertFalse(tested.equals("string"));
        tested.baudRate = null;
        tested.ctsHandshake = null;
        tested.dataBits = null;
        tested.dsr = null;
        tested.dsrHandshake = null;
        tested.dtr = null;
        tested.parity = null;
        tested.rts = null;
        tested.stopBits = null;
        tested.timeout = null;
        tested.xOnOff = null;

        try {
            tested.hashCode();
        } catch (RuntimeException e) {
            assertTrue(true);
        }
    }

    /**
     * Test method for
     * {@link com.stericsson.sdk.equipment.io.uart.internal.SerialPortConfiguration#equals(java.lang.Object)}
     * and {@link com.stericsson.sdk.equipment.io.uart.internal.SerialPortConfiguration#hashCode()}.
     */
    @Test
    public void testEqualsHashNull() {
        assertFalse(tested.equals(null));
    }

    /**
     * Test method for
     * {@link com.stericsson.sdk.equipment.io.uart.internal.SerialPortConfiguration#equals(java.lang.Object)}
     * and {@link com.stericsson.sdk.equipment.io.uart.internal.SerialPortConfiguration#hashCode()}.
     */
    @Test
    public void testEqualsHashBaudRate() {
        tested.baudRate = BAUD_RATE.BR_110;
        other.baudRate = BAUD_RATE.BR_115200;
        assertFalse(tested.equals(other));
    }

    /**
     * Test method for
     * {@link com.stericsson.sdk.equipment.io.uart.internal.SerialPortConfiguration#equals(java.lang.Object)}
     * and {@link com.stericsson.sdk.equipment.io.uart.internal.SerialPortConfiguration#hashCode()}.
     */
    @Test
    public void testEqualsHashCtsHandshake() {
        tested.ctsHandshake = ON_OFF.OFF;
        other.ctsHandshake = ON_OFF.ON;
        assertFalse(tested.equals(other));
    }

    /**
     * Test method for
     * {@link com.stericsson.sdk.equipment.io.uart.internal.SerialPortConfiguration#equals(java.lang.Object)}
     * and {@link com.stericsson.sdk.equipment.io.uart.internal.SerialPortConfiguration#hashCode()}.
     */
    @Test
    public void testEqualsHashDataBits() {
        tested.dataBits = DATA_BITS.DATA_BITS_5;
        other.dataBits = DATA_BITS.DATA_BITS_8;
        assertFalse(tested.equals(other));
    }

    /**
     * Test method for
     * {@link com.stericsson.sdk.equipment.io.uart.internal.SerialPortConfiguration#equals(java.lang.Object)}
     * and {@link com.stericsson.sdk.equipment.io.uart.internal.SerialPortConfiguration#hashCode()}.
     */
    @Test
    public void testEqualsHashBaudDsr() {
        tested.dsr = ON_OFF.OFF;
        other.dsr = ON_OFF.ON;
        assertFalse(tested.equals(other));
    }

    /**
     * Test method for
     * {@link com.stericsson.sdk.equipment.io.uart.internal.SerialPortConfiguration#equals(java.lang.Object)}
     * and {@link com.stericsson.sdk.equipment.io.uart.internal.SerialPortConfiguration#hashCode()}.
     */
    @Test
    public void testEqualsHashDsrHandshake() {
        tested.dsrHandshake = ON_OFF.OFF;
        other.dsrHandshake = ON_OFF.ON;
        assertFalse(tested.equals(other));
    }

    /**
     * Test method for
     * {@link com.stericsson.sdk.equipment.io.uart.internal.SerialPortConfiguration#equals(java.lang.Object)}
     * and {@link com.stericsson.sdk.equipment.io.uart.internal.SerialPortConfiguration#hashCode()}.
     */
    @Test
    public void testEqualsDtr() {
        tested.dtr = DTR_CIRCUIT.DTR_OFF;
        other.dtr = DTR_CIRCUIT.DTR_ON;
        assertFalse(tested.equals(other));
    }

    /**
     * Test method for
     * {@link com.stericsson.sdk.equipment.io.uart.internal.SerialPortConfiguration#equals(java.lang.Object)}
     * and {@link com.stericsson.sdk.equipment.io.uart.internal.SerialPortConfiguration#hashCode()}.
     */
    @Test
    public void testEqualsHashParity() {
        tested.parity = PARITY.PAR_EVEN;
        other.parity = PARITY.PAR_MARK;
        assertFalse(tested.equals(other));
    }

    /**
     * Test method for
     * {@link com.stericsson.sdk.equipment.io.uart.internal.SerialPortConfiguration#equals(java.lang.Object)}
     * and {@link com.stericsson.sdk.equipment.io.uart.internal.SerialPortConfiguration#hashCode()}.
     */
    @Test
    public void testEqualsHashRts() {
        tested.rts = RTS_CIRCUIT.RTS_ON;
        other.rts = RTS_CIRCUIT.RTS_OFF;
        assertFalse(tested.equals(other));
    }

    /**
     * Test method for
     * {@link com.stericsson.sdk.equipment.io.uart.internal.SerialPortConfiguration#equals(java.lang.Object)}
     * and {@link com.stericsson.sdk.equipment.io.uart.internal.SerialPortConfiguration#hashCode()}.
     */
    @Test
    public void testEqualsHashStopBits() {
        tested.stopBits = STOP_BITS.STOP_BITS_1;
        other.stopBits = STOP_BITS.STOP_BITS_2;
        assertFalse(tested.equals(other));
    }

    /**
     * Test method for
     * {@link com.stericsson.sdk.equipment.io.uart.internal.SerialPortConfiguration#equals(java.lang.Object)}
     * and {@link com.stericsson.sdk.equipment.io.uart.internal.SerialPortConfiguration#hashCode()}.
     */
    @Test
    public void testEqualsHashTimeout() {
        tested.timeout = ON_OFF.ON;
        other.timeout = ON_OFF.OFF;
        assertFalse(tested.equals(other));
    }

    /**
     * Test method for
     * {@link com.stericsson.sdk.equipment.io.uart.internal.SerialPortConfiguration#equals(java.lang.Object)}
     * and {@link com.stericsson.sdk.equipment.io.uart.internal.SerialPortConfiguration#hashCode()}.
     */
    @Test
    public void testEqualsHashXOnOff() {
        tested.xOnOff = ON_OFF.ON;
        other.xOnOff = ON_OFF.OFF;
        assertFalse(tested.equals(other));
    }

    /**
     * Test get by string methods in inner class.
     */
    @Test
    public void testGetByString() {
        assertTrue(BAUD_RATE.getByString("11") == null);
        assertTrue(BAUD_RATE.getByString("xxx") == null);

        assertTrue(DATA_BITS.getByString("11") == null);
        assertTrue(DATA_BITS.getByString("xxx") == null);

        assertTrue(PARITY.getByString("11") == null);
        assertTrue(PARITY.getByString("xxx") == null);

        assertTrue(STOP_BITS.getByString("11") == null);
        assertTrue(STOP_BITS.getByString("2") == STOP_BITS.STOP_BITS_2);
        assertTrue(STOP_BITS.getByString("xxx") == null);
    }

}
