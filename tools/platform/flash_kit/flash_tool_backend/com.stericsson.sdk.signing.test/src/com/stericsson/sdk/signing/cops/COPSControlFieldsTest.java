package com.stericsson.sdk.signing.cops;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.ReadableByteChannel;

import junit.framework.TestCase;

import org.junit.Test;

/**
 * 
 * @author xolabju
 * 
 */
public class COPSControlFieldsTest extends TestCase{

    /**
     * 
     */
    @Test
    public void testCreateControlFieldsFromChannel() {
        ReadableByteChannel channel = new ReadableByteChannel() {

            public boolean isOpen() {
                return true;
            }

            public void close() throws IOException {

            }

            public int read(ByteBuffer dst) throws IOException {
                dst.putInt(COPSControlFields.HID_BABE0200);
                return 4;
            }
        };
        try {
            COPSControlFields fields = new COPSControlFields(channel);
            assertNotNull(fields);
        } catch (IOException e) {
            fail(e.getMessage());
        }

        channel = new ReadableByteChannel() {

            public boolean isOpen() {
                return true;
            }

            public void close() throws IOException {

            }

            public int read(ByteBuffer dst) throws IOException {
                dst.putInt(COPSControlFields.HID_BABE0300);
                return 4;
            }
        };
        try {
            COPSControlFields fields = new COPSControlFields(channel);
            assertNotNull(fields);
        } catch (IOException e) {
            fail(e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testCreateControlFields() {
        COPSControlFields ccf = new COPSControlFields(COPSControlFields.HID_BABE0200);
        assertEquals(ccf.getHeaderID(), COPSControlFields.HID_BABE0200);
        assertEquals(84, ccf.getLength());
        assertEquals(0, ccf.getPayloadLength());
        ccf.setPayloadLength(20);
        assertEquals(20, ccf.getPayloadLength());

        ccf.setHeaderID(COPSControlFields.HID_BABE0300);
        assertEquals(ccf.getHeaderID(), COPSControlFields.HID_BABE0300);
        assertEquals(104, ccf.getLength());

        ccf = new COPSControlFields(COPSControlFields.HID_BABE0300);
        assertEquals(0, ccf.getPayloadLength());
        ccf.setPayloadLength(20);
        assertEquals(20, ccf.getPayloadLength());

    }

}
