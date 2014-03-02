package com.stericsson.sdk.equipment;

import junit.framework.TestCase;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.test.TestFragmentActivator;
import com.stericsson.sdk.loader.communication.LCDefinitions;

/**
 * @author Vit Sykala <vit.sykala@tieto.com>
 */
public class AbstractLCEquipmentTaskTest extends TestCase {

    private static final Object RESULT_SUCCESS = Integer.valueOf(0);

    private static final Object RESULT_FAIL = Integer.valueOf(1);

    AbstractLCEquipmentTask task;

    private AbstractLoaderCommunicationEquipment equipment;

    private static final String FUNCTION_NAME = LCDefinitions.METHOD_NAME_FS_DELETE_FILE;

    private static final CommandName COMMAND = CommandName.FILE_SYSTEM_DELETE_FILE;

    private static final String FILENAME = "file.txt";

    /**
     * {@inheritDoc}
     */
    @Before
    public void setUp() {
        equipment = new AbstractLoaderCommunicationEquipment(TestFragmentActivator.getPort(), null) {
            @Override
            public String toString() {
                return null;
            }

            public EquipmentType getEquipmentType() {
                return null;
            }
        };
        task = new AbstractLCEquipmentTask(equipment, FUNCTION_NAME, COMMAND) {
        };
        task.setArguments(new String[] {
            null, null, FILENAME});
    }

    /**
     * {@inheritDoc}
     */
    @After
    public void tearDown() {
    }

    /**
     * 
     */
    @Test(expected = RuntimeException.class)
    public void testCancel() {
        task.cancel();
    }

    /**
     * 
     */
    @Test
    public void testExecute() {
        TestFragmentActivator.getLoaderCommunicationMock().setResult(RESULT_SUCCESS);
        task.execute();
        assertEquals(RESULT_SUCCESS, task.resultCode);
        assertEquals(RESULT_SUCCESS, task.resultObject);
    }

    /**
     * 
     */
    @Test
    public void testUpdateResult() {
        task.updateResult(RESULT_FAIL);
        assertEquals(RESULT_FAIL, task.resultCode);
        String expectedMessage = equipment.getLoaderCommunicationService().getLoaderErrorDesc((Integer) RESULT_FAIL);
        assertEquals(expectedMessage, task.resultMessage);
    }

    /**
     * 
     */
    @Test
    public void testUpdateResultBadResultType() {
        task.updateResult("string");
    }

    /**
     * 
     */
    @Test
    public void testIsFatal() {
        EquipmentTaskResult taskResult = new EquipmentTaskResult(0, null, null, false);
        assertEquals(false, taskResult.isFatal());
    }

    /**
     * 
     */
    @Test
    public void testIsCancelable() {
        assertEquals(false, task.isCancelable());
    }

    /**
     * 
     */
    @Test
    public void testGetId() {
        assertEquals(COMMAND.name(), task.getId());
    }

    /**
     * 
     */
    @Test
    public void testToString() {
        assertEquals(COMMAND.name() + "@m7x00@USB2", task.toString());
    }
}
