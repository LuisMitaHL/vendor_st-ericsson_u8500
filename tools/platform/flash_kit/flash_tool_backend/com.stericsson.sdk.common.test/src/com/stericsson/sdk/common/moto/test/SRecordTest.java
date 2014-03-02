package com.stericsson.sdk.common.moto.test;

import static org.junit.Assert.*;

import java.io.UnsupportedEncodingException;
import java.text.ParseException;
import java.util.Arrays;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.common.moto.SRecord;

/**
 * @author xtomlju
 */
public class SRecordTest extends TestCase {

    /**
     * 
     */
    @Test
    public void testSetLine() {
        SRecord rec = null;
        ParseException pe = null;
        try {
            pe = null;
            rec = null;
            rec = new SRecord("AB");
        } catch (ParseException e) {
            pe = e;
        }
        assertTrue(rec == null);
        assertTrue(pe != null);

        try {
            pe = null;
            rec = null;
            rec = new SRecord("ST");
        } catch (ParseException e) {
            pe = e;
        }

        assertTrue(rec == null);
        assertTrue(pe != null);

        try {
            pe = null;
            rec = null;
            rec = new SRecord("S3154842F02000000000000000000000E0FFFFFFEF4143");
        } catch (ParseException e) {
            pe = e;
        }
        assertTrue(rec != null);
        assertTrue(pe == null);
    }

    /**
     * 
     */
    @Test
    public void testSetLineInvalidInput() {
        SRecord rec = null;
        ParseException pe = null;

        try {
            pe = null;
            rec = null;
            rec = new SRecord("S9030000FC");
        } catch (ParseException e) {
            pe = e;
        }
        assertTrue(rec != null);
        assertTrue(pe == null);

        try {
            pe = null;
            rec = null;
            rec = new SRecord("S9");
        } catch (ParseException e) {
            pe = e;
        }

        assertTrue(rec != null);
        assertTrue(pe == null);
        assertArrayEquals(rec.getData(), new byte[0]);

        try {
            pe = null;
            rec = null;
            rec = new SRecord("S9030000FC00");
        } catch (ParseException e) {
            pe = e;
        }
        assertTrue(rec != null);
        assertTrue(pe == null);

        try {
            pe = null;
            rec = null;
            rec = new SRecord("S300");
        } catch (ParseException e) {
            pe = e;
        }
        assertTrue(rec == null);
        assertTrue(pe != null);

        try {
            pe = null;
            rec = null;
            rec = new SRecord("S90*0000FC");
        } catch (ParseException e) {
            pe = e;
        }
        assertTrue(rec != null);
        assertTrue(pe == null);

        try {
            pe = null;
            rec = null;
            rec = new SRecord("S9030*00FC");
        } catch (ParseException e) {
            pe = e;
        }
        assertTrue(rec != null);
        assertTrue(pe == null);

        try {
            pe = null;
            rec = null;
            rec = new SRecord("S3154842F0200*000000000000000000E0FFFFFFEF4143");
        } catch (ParseException e) {
            pe = e;
        }
        assertTrue(rec == null);
        assertTrue(pe != null);

        try {
            pe = null;
            rec = null;
            // record with incorrect checksum
            rec = new SRecord("S3154842F02000000000000000000000E0FFFFFFEF4140");
        } catch (ParseException e) {
            pe = e;
        }
        assertTrue(rec != null);
        assertTrue(pe == null);
    }

    /**
     * 
     */
    @Test
    public void testIsStartRecord() {
        SRecord rec = null;
        ParseException pe = null;

        try {
            pe = null;
            rec = null;
            rec = new SRecord("S00600004844521B");
        } catch (ParseException e) {
            pe = e;
        }
        assertTrue(rec != null);
        assertTrue(pe == null);
        assertTrue(rec.isStartRecord());
        assertFalse(rec.isEndRecord());
    }

    /**
     * 
     */
    @Test
    public void testIsEndRecord() {
        SRecord rec = null;
        ParseException pe = null;

        try {
            pe = null;
            rec = null;
            rec = new SRecord("S9030000FC");
        } catch (ParseException e) {
            pe = e;
        }
        assertTrue(rec != null);
        assertTrue(pe == null);
        assertFalse(rec.isStartRecord());
        assertTrue(rec.isEndRecord());
    }

    /**
     * 
     */
    @Test
    public void testToString() {
        SRecord rec0 = null;
        SRecord rec1 = null;

        try {
            String str = "Hello world..";
            rec0 = new SRecord(1, 0, str.getBytes("UTF-8"));
            for (int i = 1; i <= 3; i++) {
                rec1 = new SRecord(rec0.toString(i));
                String tmp = new String(rec1.getData(), "UTF-8");
                assertTrue("Different string are not acceptable!", str.equals(tmp));
            }
        } catch (Exception e) {
            fail(e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testToStringInvalidInput() {
        try {
            SRecord record = new SRecord("S9030000FC");

            byte[] data = new byte[256];
            Arrays.fill(data, (byte) 0);
            record.setData(data);
            String result = record.toString(0);

            fail("Should not get here (expected exception, received '" + result + "').");
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }

        try {
            SRecord record = new SRecord("S9030000FC");

            record.setAddress(0xFFFFFFFFFFL);
            String result = record.toString(0);

            fail("Should not get here (expected exception, received '" + result + "').");
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testToUserFriendlyString() {
        SRecord rec = null;
        try {
            rec = new SRecord("S3154842F02000000000000000000000E0FFFFFFEF4143");
        } catch (ParseException e) {
            fail(e.getMessage());
        }
        System.out.println(rec.toUserFriendlyString());
    }

    /**
     * 
     */
    @Test
    public void testAdjustToRange() {
        String str = "Hello world..";
        int type = 1;
        long address = 0;
        byte[] bytes = null;

        try {
            bytes = str.getBytes("UTF-8");

            SRecord rec0 = new SRecord(type, address, bytes);

            assertEquals(address, rec0.getAddress());
            assertEquals(str, new String(rec0.getData(), "UTF-8"));

            rec0.adjustToRange(6, 10); // move start address to 6 and end address to 10
            assertTrue(rec0.adjustToRange(6, 10));

            assertEquals("world", new String(rec0.getData(), "UTF-8"));

            assertTrue(rec0.adjustToRange(6, 8));

            assertEquals("wor", new String(rec0.getData(), "UTF-8"));
        } catch (UnsupportedEncodingException e) {
            fail(e.getMessage());
            return;
        }
    }

    /**
     * 
     */
    @Test
    public void testAdjustToRangeInvalidInput() {
        String string = "ABCDEF";
        int type = 1;
        long address = 2;
        byte[] data = null;

        try {
            data = string.getBytes("UTF-8");

            SRecord record = new SRecord(type, address, data);

            assertEquals(type, record.getType());
            assertEquals(address, record.getAddress());
            assertEquals(string, new String(record.getData(), "UTF-8"));

            assertFalse(record.adjustToRange(8, 4));
            assertFalse(record.adjustToRange(0, 1));
            assertFalse(record.adjustToRange(8, 16));
        } catch (UnsupportedEncodingException e) {
            fail(e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testEndAndStartRecords() {
        SRecord endRecord = SRecord.getEndRecord();
        SRecord startRecord = SRecord.getStartRecord();
        assertNotNull(endRecord);
        assertNotNull(startRecord);
        assertFalse(endRecord.equals(startRecord));
        SRecord startRecord2 = SRecord.getStartRecord();
        assertTrue((startRecord.getType()==startRecord2.getType())
            &&(startRecord.getAddress()-startRecord2.getAddress()==0));
        String endRecordLower = endRecord.toString();
        SRecord.setUpperCase(true);
        String endRecordUpper = endRecord.toString();
        assertFalse(endRecordLower.equals(endRecordUpper));
    }

    /**
     * 
     */
    @Test
    public void testConstructors() {
        SRecord record = new SRecord();
        assertNotNull(record);

        String line = "S3154842F02000000000000000000000E0FFFFFFEF4143";
        try {
            record = new SRecord(line);
            assertNotNull(record);
        } catch (ParseException e) {
            e.printStackTrace();
            fail();
        }

        long address = 1;
        byte[] data = new byte[] {
            0};
        record = new SRecord(address, data);
        assertNotNull(record);
        assertEquals(address, record.getAddress());
        assertArrayEquals(data, record.getData());

        int type = 1;
        record = new SRecord(type, address, data);
        assertNotNull(record);
        assertEquals(type, record.getType());
        assertEquals(address, record.getAddress());
        assertArrayEquals(data, record.getData());

        type = -1;
        try {
            record = new SRecord(type, address, data);
        } catch (IllegalArgumentException e) {
            System.out.println(e.getMessage());
        }

        type = Integer.MAX_VALUE;
        try {
            record = new SRecord(type, address, data);
            fail("Should not get here.");
        } catch (IllegalArgumentException e) {
            System.out.println(e.getMessage());
        }

        type = 1;
        address = -1;
        try {
            record = new SRecord(type, address, data);
            fail("Should not get here.");
        } catch (IllegalArgumentException e) {
            System.out.println(e.getMessage());
        }

        type = 1;
        address = 1;
        data = null;
        try {
            record = new SRecord(type, address, data);
            fail("Should not get here.");
        } catch (IllegalArgumentException e) {
            System.out.println(e.getMessage());
        }

        data = new byte[] {};
        try {
            record = new SRecord(type, address, data);
            fail("Should not get here.");
        } catch (IllegalArgumentException e) {
            System.out.println(e.getMessage());
        }
    }

}
