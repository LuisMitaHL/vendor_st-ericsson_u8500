package com.stericsson.sdk.common.configuration.test;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;

import junit.framework.TestCase;

import org.junit.Before;
import org.junit.Test;

import com.stericsson.sdk.common.configuration.mesp.MESPConfiguration;
import com.stericsson.sdk.common.configuration.mesp.MESPConfigurationRecord;

/**
 * @author xtomlju
 */
public class MESPConfigurationTest extends TestCase {

    /** */
    private MESPConfiguration configuration;

    /**
     * @throws Exception
     *             TBD
     */
    @Before
    public void setUp() throws Exception {
        configuration = new MESPConfiguration();
    }

    private static final String TEST_FILE_FILENAME = "records.txt";

    private void createTestConfiguration() {

        MESPConfigurationRecord record;

        record = new MESPConfigurationRecord();
        record.setName("Record1");
        record.setValue("Value1", "AABBCCDD");
        record.setValue("Value2", "DDCCBBAA");

        configuration.addConfigurationRecord(record);

        record = new MESPConfigurationRecord();
        record.setName("Record2");

        record.setValue("Value1", "EEFFGGHH");
        record.setValue("Value2", "HHGGFFEE");

        configuration.addConfigurationRecord(record);

        record = new MESPConfigurationRecord();
        record.setName("Record3");

        record.setArrayValue("Value1[]", new String[] {
            "EEFFGGHH", "IIJJKKLL"});
        record.setArrayValue("Value2[]", new String[] {
            "HHGGFFEE", "LLKKJJII"});

        configuration.addConfigurationRecord(record);

    }

    private void destroyTestFile() {
        new File(TEST_FILE_FILENAME).delete();
        assertFalse((new File(TEST_FILE_FILENAME)).exists());
    }

    /**
     * 
     */
    @Test
    public void testReadFile() {
        createTestConfiguration();

        try {
            MESPConfiguration.write(new File(TEST_FILE_FILENAME), configuration);
        } catch (Exception e) {
            fail(e.getMessage());
        }

        MESPConfiguration readConfiguration = null;

        try {
            readConfiguration = MESPConfiguration.read(new File(TEST_FILE_FILENAME));
        } catch (Exception e) {
            fail(e.getMessage());
        }

        assertTrue(readConfiguration != null);
        assertTrue(readConfiguration.getAllRecords().length == 3);

        destroyTestFile();
    }

    /**
     * 
     */
    @Test
    public void testReadInputStream() {
        createTestConfiguration();

        try {
            MESPConfiguration.write(new File(TEST_FILE_FILENAME), configuration);
        } catch (Exception e) {
            fail(e.getMessage());
        }

        MESPConfiguration readConfiguration = null;
        FileInputStream input = null;

        try {
            input = new FileInputStream(TEST_FILE_FILENAME);
            readConfiguration = MESPConfiguration.read(input);
        } catch (Exception e) {
            fail(e.getMessage());
        } finally {
            try {
                input.close();
            } catch (IOException e) {
                fail(e.getMessage());
            }
        }

        assertTrue(readConfiguration != null);
        assertTrue(readConfiguration.getAllRecords().length == 3);

        destroyTestFile();
    }

    /**
     * 
     */
    @Test
    public void testWriteFileConfiguration() {
        createTestConfiguration();

        try {
            MESPConfiguration.write(new File(TEST_FILE_FILENAME), configuration);
        } catch (Exception e) {
            fail(e.getMessage());
        }

        destroyTestFile();
    }

    /**
     * 
     */
    @Test
    public void testWriteOutputStreamConfiguration() {
        createTestConfiguration();

        FileOutputStream output = null;

        try {
            output = new FileOutputStream(TEST_FILE_FILENAME);
            MESPConfiguration.write(output, configuration);
        } catch (Exception e) {
            fail(e.getMessage());
        } finally {
            try {
                if (output != null) {
                    output.close();
                }
            } catch (IOException e) {
                fail(e.getMessage());
            }
        }

        destroyTestFile();
    }

    /**
     * 
     */
    @Test
    public void testConfiguration() {
        assertTrue(configuration.getAllRecords() != null);
        assertTrue(configuration.getAllRecords().length == 0);
        assertTrue(configuration.getRecords(null) != null);
        assertTrue(configuration.getRecords(null).length == 0);
        assertTrue(configuration.getRecords(new String[] {}) != null);
        assertTrue(configuration.getRecords(new String[] {}).length == 0);
        assertTrue(configuration.getRecords(new String[] {
            "Test"}) != null);
        assertTrue(configuration.getRecords(new String[] {
            "Test"}).length == 0);
    }

    /**
     * 
     */
    @Test
    public void testAddConfigurationRecord() {
        MESPConfigurationRecord record1 = new MESPConfigurationRecord();
        record1.setName("Record1");
        configuration.addConfigurationRecord(record1);
        assertTrue(configuration.getAllRecords() != null);
        assertTrue(configuration.getAllRecords().length == 1);
        MESPConfigurationRecord record2 = new MESPConfigurationRecord();
        record1.setName("Record2");
        configuration.addConfigurationRecord(record2);
        assertTrue(configuration.getAllRecords() != null);
        assertTrue(configuration.getAllRecords().length == 2);
    }

    /**
     * 
     */
    @Test
    public void testGetRecords() {
        MESPConfigurationRecord record1 = new MESPConfigurationRecord();
        record1.setName("Record1");
        configuration.addConfigurationRecord(record1);
        MESPConfigurationRecord record2 = new MESPConfigurationRecord();
        record2.setName("Record2");
        configuration.addConfigurationRecord(record2);

        assertTrue(configuration.getRecords(null) != null);
        assertTrue(configuration.getRecords(null).length == 0);
        assertTrue(configuration.getRecords(new String[] {
            "Record1"}) != null);
        assertTrue(configuration.getRecords(new String[] {
            "Record1"}).length == 1);
        assertTrue(configuration.getRecords(new String[] {
            "Record1"})[0] != null);
        assertTrue(configuration.getRecords(new String[] {
            "Record1"})[0].getName().equalsIgnoreCase("record1"));
        assertTrue(!configuration.getRecords(new String[] {
            "Record1"})[0].getName().equalsIgnoreCase("record"));
    }

    /**
     * 
     */
    @Test
    public void testGetRecordByValue() {
        MESPConfigurationRecord record1 = new MESPConfigurationRecord();
        record1.setName("Record1");
        record1.setValue("Record1", "Test");
        configuration.addConfigurationRecord(record1);
        MESPConfigurationRecord record2 = new MESPConfigurationRecord();
        record2.setName("Record2");
        configuration.addConfigurationRecord(record2);

        assertTrue(configuration.getRecordByValue("Record1", "Test").length == 1);
    }

    /**
     * 
     */
    @Test
    public void testGetAllRecords() {
        MESPConfigurationRecord record1 = new MESPConfigurationRecord();
        record1.setName("Record1");
        configuration.addConfigurationRecord(record1);
        MESPConfigurationRecord record2 = new MESPConfigurationRecord();
        record2.setName("Record2");
        configuration.addConfigurationRecord(record2);
        MESPConfigurationRecord record3 = new MESPConfigurationRecord();
        record3.setName("Record3");
        configuration.addConfigurationRecord(record3);
        MESPConfigurationRecord record4 = new MESPConfigurationRecord();
        record4.setName("Record4");
        configuration.addConfigurationRecord(record4);

        assertTrue(configuration.getAllRecords() != null);
        assertTrue(configuration.getAllRecords().length == 4);
    }

    /**
     * 
     */
    @Test
    public void testRecordWithArrayValue() {
        MESPConfigurationRecord record1 = new MESPConfigurationRecord();
        record1.setName("Record1");
        record1.setArrayValue("Value1[]", new String[] {
            "A", "B", "C"});

        String[] arrayValue = record1.getArrayValue("Value1[]");
        if (arrayValue == null) {
            fail();
        } else {
            assertTrue(arrayValue.length == 3);
        }
    }
}
