package com.stericsson.sdk.equipment.io.port;

/**
 * @author rohacmic
 *
 */
public interface IPortProviderTester {

    /** */
    String DUMMY_SERIAL_PORT = "DummySerialPort";

    /** */
    String UART_PORT_RECIEVER_NAME = "uart.test";

    /** */
    String USB_PORT_RECIEVER_NAME = "usb.test";

    /**
     * @param vid TBD
     * @param pid TBD
     * @param readData TBD
     * @param writeData TBD
     */
    void simulateUSBDevice(int vid, int pid, byte[][] readData, byte[][] writeData);

    /**
     * @param readData TBD
     * @param writeData TBD
     */
    void simulateUARTDevice(byte[][] readData, byte[][] writeData);

}
