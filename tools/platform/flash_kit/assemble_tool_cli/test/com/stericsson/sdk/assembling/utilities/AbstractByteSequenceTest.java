package com.stericsson.sdk.assembling.utilities;

import static org.junit.Assert.*;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.ReadableByteChannel;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

/**
 * 
 * @author xolabju
 * 
 */
public class AbstractByteSequenceTest {

    private TestSequence seq;

    private static final int FIELD_1 = 1;

    private static final int FIELD_2 = 2;

    private static final int SIZE_FIELD = 4;

    /**
     * 
     * @throws Exception
     *             e
     */
    @Before
    public void setUp() throws Exception {
        seq = new TestSequence();
    }

    /**
     * 
     * @throws Exception
     *             e
     */
    @After
    public void tearDown() throws Exception {
        seq = null;
    }

    /**
     * 
     */
    @Test
    public void testSetFields() {
        assertNotNull(seq);
        assertEquals(seq.getLength(), seq.getData().length);
        assertEquals(seq.getLength(), SIZE_FIELD * 2);
        assertEquals(0, seq.getField1());
        assertArrayEquals(new byte[] {
            0, 0, 0, 0}, seq.getField2());

        int i = 0xBABE;
        seq.setField1(i);
        assertEquals(i, seq.getField1());

        byte[] b = new byte[] {
            1, 2, 3, 4};
        seq.setField2(b);
        assertArrayEquals(b, seq.getField2());

    }

    /**
     * 
     */
    @Test
    public void testSetData() {
        byte[] newData = new byte[] {
            0, 1, 2, 3, 4, 5, 6, 7};
        try {
            seq.setData(newData);
            assertArrayEquals(newData, seq.getData());
        } catch (Exception e) {
            fail(e.getMessage());
        }

        newData = new byte[] {
            0};
        try {
            seq.setData(newData);
            fail("should not be allowed");
        } catch (Exception e) {
            // ignore
            e.getMessage();
        }

        try {
            seq.setData(null);
            fail("should not be allowed");
        } catch (Exception e) {
            // ignore
            e.getMessage();
        }
    }

    /**
     * 
     */
    @Test
    public void testRead() {
        final byte[] b = new byte[] {
            0, 1, 2, 3, 4, 5, 6, 7};

        ReadableByteChannel channel = new ReadableByteChannel() {

            int reads = 0;

            public boolean isOpen() {
                return true;
            }

            public void close() throws IOException {
                // ignore

            }

            public int read(ByteBuffer dst) throws IOException {
                dst.put(b, SIZE_FIELD * reads, SIZE_FIELD);
                reads++;
                return SIZE_FIELD;
            }
        };

        try {
            seq.read(channel);
        } catch (IOException e) {
            fail(e.getMessage());
        }
        assertArrayEquals(b, seq.getData());
    }

    private class TestSequence extends AbstractByteSequence {
        public TestSequence() {
            fields.put(FIELD_1, allocateField(SIZE_FIELD));
            fields.put(FIELD_2, allocateField(SIZE_FIELD));
        }

        public void setField1(int value) {
            fields.get(FIELD_1).putInt(0, value);
        }

        public int getField1() {
            return fields.get(FIELD_1).getInt(0);
        }

        public void setField2(byte[] value) {
            fields.get(FIELD_2).rewind();
            fields.get(FIELD_2).put(value, 0, value.length);
        }

        public byte[] getField2() {
            byte[] field2 = new byte[SIZE_FIELD];
            fields.get(FIELD_2).rewind();
            fields.get(FIELD_2).get(field2, 0, SIZE_FIELD);
            return field2;
        }
    }
}
