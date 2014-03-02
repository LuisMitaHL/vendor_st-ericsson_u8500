package com.stericsson.sdk.common.configuration.test;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.common.configuration.mesp.MESPConfigurationException;
import com.stericsson.sdk.common.configuration.mesp.MESPConfigurationRecord;

/**
 * @author xtomlju
 */
public class MESPConfigurationRecordTest extends TestCase {

    /**
     * @throws Exception
     *             TBD
     * 
     */
    @Test
    public void testParse() throws Exception {
        MESPConfigurationRecord record = null;
        MESPConfigurationException ce;

        try {
            ce = null;
            record = MESPConfigurationRecord.parse("?");
        } catch (MESPConfigurationException e) {
            ce = e;
        }

        if (ce == null) {
            throw new Exception("Should not get here.");
        }
        assertEquals("Configuration line must start with a name", ce.getMessage());

        try {
            ce = null;
            record = MESPConfigurationRecord.parse("Name ?");
        } catch (MESPConfigurationException e) {
            ce = e;
        }

        if (ce == null) {
            throw new Exception("Should not get here.");
        }
        assertEquals("Missing value name", ce.getMessage());

        try {
            ce = null;
            record = MESPConfigurationRecord.parse("Name id?");
        } catch (MESPConfigurationException e) {
            ce = e;
        }

        if (ce == null) {
            throw new Exception("Should not get here.");
        }
        assertEquals("Missing '=' delimeter", ce.getMessage());

        try {
            ce = null;
            record = MESPConfigurationRecord.parse("Name id=");
        } catch (MESPConfigurationException e) {
            ce = e;
        }

        if (ce == null) {
            throw new Exception("Should not get here.");
        }
        assertEquals("Missing value", ce.getMessage());

        try {
            ce = null;
            record = MESPConfigurationRecord.parse("Name id=\"1\"");
        } catch (MESPConfigurationException e) {
            ce = e;
        }

        if (ce != null) {
            throw new Exception("Should not get here.");
        }
        assertTrue(record != null);
        assertEquals("Name id=\"1\"", record.toString());

        try {
            ce = null;
            record = MESPConfigurationRecord.parse("Name ids[]={\"1\"}");
        } catch (MESPConfigurationException e) {
            fail(e.getMessage());
        }

        record = new MESPConfigurationRecord();
        record.setName("Name");
        record.setArrayValue("array", new String[] {
            "1", "3", "2"});

        assertEquals("Name", record.getName());
        assertTrue(record.getArrayValue("array").length == 3);
        assertEquals("Name array={\"1\" \"3\" \"2\"}", record.toString());
    }

}
