package com.stericsson.sdk.equipment.tasks;

import static org.junit.Assert.assertArrayEquals;
import junit.framework.TestCase;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import com.stericsson.sdk.brp.util.Base64;
import com.stericsson.sdk.equipment.AbstractLoaderCommunicationEquipment;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.EquipmentType;
import com.stericsson.sdk.equipment.test.TestFragmentActivator;
import com.stericsson.sdk.loader.communication.ILoaderCommunicationService;
import com.stericsson.sdk.loader.communication.internal.LoaderCommunicationMock;

/**
 * 
 * @author pkutac01
 * 
 */
public class EquipmentBurnOTPDataTaskTest extends TestCase {

    private static final int ERROR_NONE = 0;

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
     * Test method for {@link EquipmentBurnOTPDataTask#setArguments(String[])}
     */
    @Test
    public void testSetArguments() {
        EquipmentBurnOTPDataTask task = new EquipmentBurnOTPDataTask(equipment);

        byte[] data =
            new byte[] {
                0x40, 0x0, 0x0, 0x9, 0x0, 0x0, 0x0, 0x0, 0x40, 0x20, 0x0, 0x5, 0x0, 0x0, 0x0, 0x5, 0x40, 0x20, 0x0,
                0xA, 0x0, 0x0, 0x0, 0x5};
        String[] arguments = new String[] {
            task.getId(), task.getEquipment().getPort().getPortName(), Base64.encode(data)};
        task.setArguments(arguments);

        byte[] methodData = new byte[] {
            0x05, 0x0, 0x0, 0x40, 0x5, 0x0, 0x0, 0x0, 0x0A, 0x0, 0x0, 0x40, 0x5, 0x0, 0x0, 0x0};
        Object[] methodArguments = task.getArguments();

        assertEquals(0, methodArguments[0]);
        assertEquals(0x05 * 64, methodArguments[1]);
        assertEquals(methodData.length * 8, methodArguments[2]);
        assertArrayEquals(methodData, (byte[]) methodArguments[3]);
    }

    /**
     * Test method for {@link EquipmentBurnOTPDataTask#execute()}
     */
    @Test
    public void testExecute() {
        EquipmentBurnOTPDataTask task = new EquipmentBurnOTPDataTask(equipment);

        byte[] data = new byte[] {
            0x40, 0x0, 0x0, 0x9, 0x0, 0x0, 0x0, 0x0, 0x40, 0x0, 0x0, 0xA, 0x0, 0x0, 0x0, 0x5};
        String[] arguments = new String[] {
            task.getId(), task.getEquipment().getPort().getPortName(), Base64.encode(data)};
        task.setArguments(arguments);

        Object[] methodArguments = task.getArguments();

        assertArrayEquals(new byte[] {}, (byte[]) methodArguments[3]);

        EquipmentTaskResult result = task.execute();
        assertEquals(ERROR_NONE, result.getResultCode());
    }
}
