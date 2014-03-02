package com.stericsson.sdk.equipment.tasks;

import static org.junit.Assert.assertArrayEquals;

import java.io.File;
import java.io.FileInputStream;
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
import com.stericsson.sdk.loader.communication.types.OTPReadBitsType;

/**
 * 
 * @author pkutac01
 * 
 */
public class EquipmentReadOTPTaskTest extends TestCase {

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
    public void testUpdateResult() {
        EquipmentReadOTPTask task = new EquipmentReadOTPTask(equipment);

        try {
            File tempFile = File.createTempFile("otpdata", null);

            String[] arguments = new String[] {
                task.getId(), task.getEquipment().getPort().getPortName(), tempFile.getAbsolutePath()};
            task.setArguments(arguments);

            byte[] resultData = new byte[] {
                0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0};
            byte[] swapResultData = new byte[] {
                0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
            Object result = new OTPReadBitsType(0, resultData, null);
            task.updateResult(result);

            assertEquals(resultData.length, tempFile.length());

            byte[] data = new byte[(int) tempFile.length()];
            FileInputStream fis = null;
            try {
                fis = new FileInputStream(tempFile);
                int read = fis.read(data);
                if (read != data.length) {
                    throw new IOException("Read " + read + " bytes instead of " + data.length + " bytes from file '"
                        + tempFile.getAbsolutePath() + "'.");
                }
            } catch (IOException e) {
                e.printStackTrace();
                fail(e.getMessage());
            } finally {
                if (fis != null) {
                    try {
                        fis.close();
                    } catch (IOException e) {
                        e.printStackTrace();
                        fail(e.getMessage());
                    }
                }
            }

            assertArrayEquals(swapResultData, data);

            tempFile.delete();
        } catch (IOException e) {
            e.printStackTrace();
            fail(e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testSetInvalidArguments() {
        EquipmentReadOTPTask task = new EquipmentReadOTPTask(equipment);

        assertEquals(IEquipmentTask.ERROR_NONE, task.getResultCode());

        String[] arguments = new String[] {
            task.getId(), task.getEquipment().getPort().getPortName(), null};
        task.setArguments(arguments);

        byte[] resultData = new byte[] {
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0};
        Object result = new OTPReadBitsType(0, resultData, null);
        task.updateResult(result);

        assertEquals(IEquipmentTask.ERROR, task.getResultCode());
    }

}
