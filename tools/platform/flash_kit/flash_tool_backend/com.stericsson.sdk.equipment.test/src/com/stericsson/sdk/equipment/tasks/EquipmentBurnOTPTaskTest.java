package com.stericsson.sdk.equipment.tasks;

import static org.junit.Assert.assertArrayEquals;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

import junit.framework.TestCase;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import com.stericsson.sdk.equipment.AbstractLoaderCommunicationEquipment;
import com.stericsson.sdk.equipment.EquipmentType;
import com.stericsson.sdk.equipment.IEquipmentTask;
import com.stericsson.sdk.equipment.test.TestFragmentActivator;
import com.stericsson.sdk.loader.communication.ILoaderCommunicationService;
import com.stericsson.sdk.loader.communication.internal.LoaderCommunicationMock;

/**
 * 
 * @author pkutac01
 * 
 */
public class EquipmentBurnOTPTaskTest extends TestCase {

    private AbstractLoaderCommunicationEquipment equipment;

    /**
     * 
     * @throws Exception
     *             Exception.
     */
    @Override
    @Before
    public void setUp() throws Exception {
        equipment = new AbstractLoaderCommunicationEquipment(TestFragmentActivator.getPort(), null) {

            public EquipmentType getEquipmentType() {
                return null;
            }

            @Override
            public String toString() {
                return null;
            }

            @Override
            public ILoaderCommunicationService getLoaderCommunicationService() {
                return new LoaderCommunicationMock();
            }
        };
    }

    /**
     * 
     * @throws Exception
     *             Exception.
     */
    @Override
    @After
    public void tearDown() throws Exception {
    }

    /**
     * 
     */
    @Test
    public void testSetArguments() {
        EquipmentBurnOTPTask task = new EquipmentBurnOTPTask(equipment);

        byte[] data = new byte[] {
            0x40, 0x0, 0x0, 0x9, 0x0, 0x0, 0x0, 0x0, 0x40, 0x20, 0x0, 0xA, 0x0, 0x0, 0x0, 0x5};
        FileOutputStream fos = null;
        try {
            File tempFile = File.createTempFile("otpdata", null);
            fos = new FileOutputStream(tempFile);
            fos.write(data);
            fos.flush();
            fos.close();

            String[] arguments = new String[] {
                task.getId(), task.getEquipment().getPort().getPortName(), tempFile.getAbsolutePath()};
            task.setArguments(arguments);

            byte[] methodData = new byte[] {
                0x0A, 0x0, 0x0, 0x40, 0x5, 0x0, 0x0, 0x0};
            Object[] methodArguments = task.getArguments();

            assertEquals(0, methodArguments[0]);
            assertEquals(0xA * 64, methodArguments[1]);
            assertEquals(methodData.length * 8, methodArguments[2]);
            assertArrayEquals(methodData, (byte[]) methodArguments[3]);

            tempFile.delete();
        } catch (IOException e) {
            e.printStackTrace();
            fail(e.getMessage());
        } finally {
            if (fos != null) {
                try {
                    fos.close();
                } catch (IOException e) {
                    e.printStackTrace();
                    fail(e.getMessage());
                }
            }
        }
    }

    /**
     * 
     */
    @Test
    public void testSetInvalidArguments() {
        EquipmentBurnOTPTask task = new EquipmentBurnOTPTask(equipment);

        assertEquals(IEquipmentTask.ERROR_NONE, task.getResultCode());

        String[] arguments = new String[] {
            task.getId(), task.getEquipment().getPort().getPortName(), "/invalid_file_path/"};
        task.setArguments(arguments);

        assertEquals(IEquipmentTask.ERROR, task.getResultCode());
    }

}
