package com.stericsson.sdk.assembling.internal;

import static org.junit.Assert.assertArrayEquals;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

import org.junit.Test;

/**
 * @author xtomlju
 */
public class MESPConfigurationRecordTest {

    /**
     * @author xtomlju
     * @throws Exception
     *             tbd
     */
    @Test
    public void testParse() throws Exception {
        MESPConfigurationRecord record = null;
        // MESPConfigurationException ce = null;

        try {
            record = MESPConfigurationRecord.parse("?");
            fail("MESPConfigurationException should be thrown");
        } catch (MESPConfigurationException e) {
            assertEquals("Configuration line must start with a name", e.getMessage());
        }

        try {
            record = MESPConfigurationRecord.parse("Name ?");
            fail("MESPConfigurationException should be thrown");
        } catch (MESPConfigurationException e) {
            assertEquals("Missing value name", e.getMessage());
        }

        try {
            record = MESPConfigurationRecord.parse("Name id?");
            fail("MESPConfigurationException should be thrown");
        } catch (MESPConfigurationException e) {
            assertEquals("Missing '=' delimeter", e.getMessage());
        }

        testParseCtd(record);
    }

    private void testParseCtd(MESPConfigurationRecord record) throws Exception {

        try {
            record = MESPConfigurationRecord.parse("Name id=");
            fail("MESPConfigurationException should be thrown");
        } catch (MESPConfigurationException e) {
            assertEquals("Missing value", e.getMessage());
        }

        try {

            record = MESPConfigurationRecord.parse("Name id=\"1\"");
        } catch (MESPConfigurationException e) {
            fail(e.getMessage());
        }

        assertNotNull(record);
        assertEquals("Name id=\"1\"", record.toString());

        try {
            record = MESPConfigurationRecord.parse("Name ids[]={\"1\"}");
        } catch (MESPConfigurationException e) {
            fail(e.getMessage());
        }

        record = new MESPConfigurationRecord();
        record.setName("Name");
        record.setArrayValue("array", new String[] {
            "1", "3", "2"});

        assertEquals("Name", record.getName());
        assertNotNull(record.getArrayValue("array"));
        assertTrue(record.getArrayValue("array").length == 3);
        assertEquals("Name array={\"1\" \"3\" \"2\"}", record.toString());
    }

    /**
     * 
     * @throws Exception
     *             TBD
     */
    @Test
    public void testSetValue() throws Exception {
        MESPConfigurationRecord mesp = new MESPConfigurationRecord();
        mesp.setValue("key1", "value1");
        String value = mesp.getValue("key1");
        if (value == null) {
            throw new Exception("Prevent needs this :/");
        }
        assertTrue(value.equals("value1"));
        mesp.setValue("key1", "value1_new");
        value = mesp.getValue("key1");
        if (value == null) {
            throw new Exception("Prevent needs this :/");
        }
        assertTrue(value.equals("value1_new"));

        String[] values = new String[] {
            "value1", "value2", "value3"};
        mesp.setArrayValue("arrayValue", values);
        assertArrayEquals(mesp.getArrayValue("arrayValue"), values);
        String[] values2 = new String[] {
            "value1_new", "value2_new", "value3_new"};
        mesp.setArrayValue("arrayValue", values2);
        assertArrayEquals(mesp.getArrayValue("arrayValue"), values2);
    }

    /**
     * 
     */
    @Test
    public void testSetArrayValue() {
        MESPConfigurationRecord mesp = new MESPConfigurationRecord();
        mesp.setArrayValue("arrayValue1", new String[] {});
        mesp.setArrayValue("arrayValue2", null);
        assertNotNull(mesp.toString());
    }

    /**
     * 
     */
    @Test
    public void testParseArrayValueNoLeftBrace() {

        MESPConfigurationTokenizer tokenizer = new MESPConfigurationTokenizer("123}");
        try {
            MESPConfigurationRecord.parseArrayValue(tokenizer, new MESPConfigurationRecord(), "Test");
        } catch (MESPConfigurationException e) {
            assertEquals("Missing '{' delimeter", e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testParseArrayValueNoIdentifier() {

        MESPConfigurationTokenizer tokenizer = new MESPConfigurationTokenizer("{}");
        try {
            MESPConfigurationRecord.parseArrayValue(tokenizer, new MESPConfigurationRecord(), "Test");
        } catch (MESPConfigurationException e) {
            assertEquals("Missing identifier", e.getMessage());
        }
    }

    // @Test
    // public void testParseArrayValueNoRightBrace() {
    //
    // MESPConfigurationTokenizer tokenizer = new MESPConfigurationTokenizer("{12");
    // try {
    // MESPConfigurationRecord.parseArrayValue(tokenizer, new MESPConfigurationRecord(), "Test");
    // } catch (MESPConfigurationException e) {
    // assertEquals("Missing '}' delimeter", e.getMessage());
    // }
    // }

    /**
     * 
     */
    @Test
    public void testIsValueNameSetNullParameter() {
        MESPConfigurationRecord record = new MESPConfigurationRecord();
        assertFalse(record.isValueNameSet(null));
    }

    /**
     * 
     */
    @Test
    public void testIsValueNameSet() {
        MESPConfigurationRecord record = new MESPConfigurationRecord();
        record.setValue("Test-Name", "Test-Value");
        record.setValue("Test-Name2", "Test-Value2");
        record.setValue("Test-Name3", "Test-Value3");
        assertTrue(record.isValueNameSet("Test-Name3"));
    }

    /**
     * 
     */
    @Test
    public void testIsValueNameSetNoValue() {
        MESPConfigurationRecord record = new MESPConfigurationRecord();
        record.setValue("Test-Name", "Test-Value");
        record.setValue("Test-Name2", "Test-Value2");
        record.setValue("Test-Name3", "Test-Value3");

        assertFalse(record.isValueNameSet("Test-Name4"));
    }

    /**
     * 
     */
    @Test
    public void testGetValue() {
        MESPConfigurationRecord record = MESPConfigurationRecord.newRecord();
        String value = record.getValue("name");
        assertNull(value);

        record.setValue("name", "value");
        value = record.getValue("test");
        assertNull(value);
    }

    /**
     * 
     */
    @Test
    public void testGetArrayValue() {
        MESPConfigurationRecord record = MESPConfigurationRecord.newRecord();
        String[] value = record.getArrayValue("name");
        assertNull(value);

        record.setArrayValue("name", new String[] {"value"});
        value = record.getArrayValue("test");
        assertNull(value);
    }

    /**
     * 
     */
    @Test
    public void testDeepCopy() {
        MESPConfigurationRecord record = MESPConfigurationRecord.newRecord();
        record.setName("test");
        MESPConfigurationRecord deepCopy = record.deepCopy();
        assertNotNull(deepCopy);

        record = MESPConfigurationRecord.newRecord();
        record.setName("test");
        record.setValue("name1", "value1");
        record.setArrayValue("name2", new String[] {
            "value2a", "value2b"});
        deepCopy = record.deepCopy();

        assertNotNull(deepCopy);
        assertEquals(record.getName(), deepCopy.getName());
        assertEquals(record.getValue("name1"), deepCopy.getValue("name1"));
        assertArrayEquals(record.getArrayValue("name2"), deepCopy.getArrayValue("name2"));
    }

}
