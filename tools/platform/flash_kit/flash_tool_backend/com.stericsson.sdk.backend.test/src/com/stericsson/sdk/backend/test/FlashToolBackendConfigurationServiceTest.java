package com.stericsson.sdk.backend.test;

import junit.framework.TestCase;

import org.junit.Test;
import org.osgi.framework.ServiceReference;

import com.stericsson.sdk.common.configuration.IConfigurationRecord;
import com.stericsson.sdk.common.configuration.IConfigurationService;
import com.stericsson.sdk.common.configuration.mesp.MESPConfigurationRecord;

/**
 * @author xtomlju
 */
public class FlashToolBackendConfigurationServiceTest extends TestCase {

    /**
     * @throws Exception
     *             TBD
     */
    @Test
    public void testAddRecord() throws Exception {

        ServiceReference reference =
            Activator.getBundleContext().getServiceReference(IConfigurationService.class.getName());
        if (reference == null) {
            throw new Exception("Should not get here!");
        }
        IConfigurationService service = (IConfigurationService) Activator.getBundleContext().getService(reference);
        if (service == null) {
            throw new Exception("Should not get here!");
        }

        IConfigurationRecord record = new MESPConfigurationRecord();
        if (record == null) {
            throw new Exception("Should not get here!");
        }
        record.setName("TestRecord");
        record.setValue("value", "test");

        service.addRecord(record);

        record = service.getRecord("TestRecord");
        if (record == null) {
            throw new Exception("Should not get here!");
        }
        assertEquals("TestRecord", record.getName());
        assertEquals("test", record.getValue("value"));

        Activator.getBundleContext().ungetService(reference);
    }

    /**
     * @throws Exception
     *             TBD
     */
    @Test
    public void testGetAllRecords() throws Exception {
        ServiceReference reference =
            Activator.getBundleContext().getServiceReference(IConfigurationService.class.getName());
        if (reference == null) {
            throw new Exception("Should not get here!");
        }
        IConfigurationService service = (IConfigurationService) Activator.getBundleContext().getService(reference);
        if (service == null) {
            throw new Exception("Should not get here!");
        }

        IConfigurationRecord[] records = service.getAllRecords();
        if (records == null) {
            throw new Exception("Should not get here!");
        }
        assertTrue(records.length > 0);

        for (IConfigurationRecord record : records) {
            System.out.println(record);
        }

        Activator.getBundleContext().ungetService(reference);
    }

    /**
     * @throws Exception
     *             TBD
     */
    @Test
    public void testGetRecords() throws Exception {
        ServiceReference reference =
            Activator.getBundleContext().getServiceReference(IConfigurationService.class.getName());
        if (reference == null) {
            throw new Exception("Should not get here!");
        }
        IConfigurationService service = (IConfigurationService) Activator.getBundleContext().getService(reference);
        if (service == null) {
            throw new Exception("Should not get here!");
        }

        IConfigurationRecord[] records = service.getRecords(new String[] {
            "Record1", "Record2"});
        if (records == null) {
            throw new Exception("Should not get here!");
        }
        assertTrue("Expected 4 records. Invalid test configuration?", records.length == 4);

        Activator.getBundleContext().ungetService(reference);
    }

    /**
     * @throws Exception
     *             TBD
     */
    @Test
    public void testGetUSBRecords() throws Exception {
        ServiceReference reference =
            Activator.getBundleContext().getServiceReference(IConfigurationService.class.getName());
        if (reference == null) {
            throw new Exception("Should not get here!");
        }
        IConfigurationService service = (IConfigurationService) Activator.getBundleContext().getService(reference);
        if (service == null) {
            throw new Exception("Should not get here!");
        }

        IConfigurationRecord[] records = service.getRecords(new String[] {
            "USB"});
        if (records == null) {
            throw new Exception("Should not get here!");
        }
        assertTrue(records.length == 1);

        Activator.getBundleContext().ungetService(reference);
    }

    /**
     * @throws Exception
     *             TBD
     */
    @Test
    public void testGetUARTRecords() throws Exception {
        ServiceReference reference =
            Activator.getBundleContext().getServiceReference(IConfigurationService.class.getName());
        if (reference == null) {
            throw new Exception("Should not get here!");
        }
        IConfigurationService service = (IConfigurationService) Activator.getBundleContext().getService(reference);
        if (service == null) {
            throw new Exception("Should not get here!");
        }

        IConfigurationRecord[] records = service.getRecords(new String[] {
            "UART"});
        if (records == null) {
            throw new Exception("Should not get here!");
        }
        assertTrue(records.length == 1);

        Activator.getBundleContext().ungetService(reference);
    }

    /**
     * @throws Exception
     *             TBD
     * 
     */
    @Test
    public void testSetRecords() throws Exception {
        ServiceReference reference =
            Activator.getBundleContext().getServiceReference(IConfigurationService.class.getName());
        if (reference == null) {
            throw new Exception("Should not get here!");
        }
        IConfigurationService service = (IConfigurationService) Activator.getBundleContext().getService(reference);
        if (service == null) {
            throw new Exception("Should not get here!");
        }

        IConfigurationRecord[] records = service.getAllRecords();
        if (records == null) {
            throw new Exception("Should not get here!");
        }
        service.setRecords(records);

        Activator.getBundleContext().ungetService(reference);
    }

    /**
     * 
     */
    @Test
    public void testWriteFile() {
        // fail("Not yet implemented");
    }
}
