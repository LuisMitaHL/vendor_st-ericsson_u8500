package com.stericsson.sdk.equipment.tasks;

import junit.framework.TestCase;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import com.stericsson.sdk.brp.util.Base64;
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
public class EquipmentReadOTPDataTaskTest extends TestCase {

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
        EquipmentReadOTPDataTask task = new EquipmentReadOTPDataTask(equipment);
        byte[] data =
            new byte[] {
                0x0, 0x0, 0x0, 0x4, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x6,
                0x0, 0x0, 0x0, 0x0};
        String[] arguments = new String[] {
            task.getId(), task.getEquipment().getPort().getPortName(), Base64.encode(data)};
        task.setArguments(arguments);

        byte[] resultData =
            new byte[] {
                0x4, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0,
                0x0, 0x0, 0x0, 0x0, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x3, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
                0x6, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x5, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
        Object result = new OTPReadBitsType(0, resultData, null);
        task.updateResult(result);

        assertEquals(Base64.encode(data), task.getResultMessage());

        result = new OTPReadBitsType(1, null, null);
        task.updateResult(result);

        assertEquals(IEquipmentTask.ERROR, task.getResultCode());

        result = Integer.valueOf(1);
        task.updateResult(result);

        assertEquals(IEquipmentTask.ERROR, task.getResultCode());
    }

    /**
     * 
     */
    @Test
    public void testUpdateResultWithInvalidIndex() {
        EquipmentReadOTPDataTask task = new EquipmentReadOTPDataTask(equipment);

        byte[] data =
            new byte[] {
                0x0, 0x0, 0x0, 0x4, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x6,
                0x0, 0x0, 0x0, 0x0};
        String[] arguments = new String[] {
            task.getId(), task.getEquipment().getPort().getPortName(), Base64.encode(data)};
        task.setArguments(arguments);

        byte[] resultData =
            new byte[] {
                0x4, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x7F, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0,
                0x0, 0x0, 0x0, 0x0, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x3, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
                0x6, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x5, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
        Object result = new OTPReadBitsType(0, resultData, null);

        assertEquals(IEquipmentTask.ERROR_NONE, task.getResultCode());

        task.updateResult(result);

        assertEquals(IEquipmentTask.ERROR, task.getResultCode());
    }

    /**
     * 
     */
    @Test
    public void testSetArguments() {
        EquipmentReadOTPDataTask task = new EquipmentReadOTPDataTask(equipment);

        byte[] data =
            new byte[] {
                0x0, 0x0, 0x0, 0x4, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x6,
                0x0, 0x0, 0x0, 0x0};
        String[] arguments = new String[] {
            task.getId(), task.getEquipment().getPort().getPortName(), Base64.encode(data)};
        task.setArguments(arguments);

        Object[] methodArguments = task.getArguments();

        assertEquals(0, methodArguments[0]);
        assertEquals(2 * 64, methodArguments[1]);
        assertEquals((6 - 2 + 1) * 64, methodArguments[2]);
    }

}
