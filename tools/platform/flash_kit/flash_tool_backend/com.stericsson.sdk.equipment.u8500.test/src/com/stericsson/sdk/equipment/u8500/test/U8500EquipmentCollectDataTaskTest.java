package com.stericsson.sdk.equipment.u8500.test;

import java.io.UnsupportedEncodingException;

import junit.framework.TestCase;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.tasks.EquipmentCollectDataTask;
import com.stericsson.sdk.equipment.u8500.internal.U8500Equipment;
import com.stericsson.sdk.loader.communication.types.SystemCollectDataType;

/**
 * @author ezaptom
 * 
 */
public class U8500EquipmentCollectDataTaskTest extends TestCase {

    private EquipmentCollectDataTask collectDataTask;

    private static String[] pArguments = new String[] {
        "SYSTEM_COLLECT_DATA", "<equipment-id>", "2"};

    static final int RESULT_SUCCESS = 0;

    static final int RESULT_FAIL = 5;

    static final String STATUS_FROM_ME = "some status from me";

    static final String NO_STATUS_FROM_ME = "No flash report received from ME.";

    /** */
    public void testU8500CollectData() {
        U8500Equipment eq = new U8500Equipment(Activator.getPort(), null);
        collectDataTask = new EquipmentCollectDataTask(eq);
        byte[] statusBytes = null;
        try {
            statusBytes = STATUS_FROM_ME.getBytes("UTF-8");
        } catch (UnsupportedEncodingException e) {
            fail(e.getMessage());
            return;
        }
        Activator.getLoaderCommunicationMock().setResult(new SystemCollectDataType(RESULT_SUCCESS, statusBytes));
        collectDataTask.setArguments(pArguments);
        EquipmentTaskResult result =  collectDataTask.execute();
        assertEquals(RESULT_SUCCESS, result.getResultCode());
        assertEquals(STATUS_FROM_ME, result.getResultMessage());

        Activator.getLoaderCommunicationMock().setResult(new SystemCollectDataType(RESULT_SUCCESS, new byte[] {}));
        result =  collectDataTask.execute();
        assertEquals(RESULT_SUCCESS, result.getResultCode());
        assertEquals(NO_STATUS_FROM_ME, result.getResultMessage());

        Activator.getLoaderCommunicationMock().setResult(new SystemCollectDataType(RESULT_FAIL, new byte[] {}));
        result =  collectDataTask.execute();
        assertEquals(RESULT_FAIL, result.getResultCode());

        assertNull(result.getResultObject());

        assertFalse(collectDataTask.isCancelable());

        assertEquals(CommandName.SYSTEM_COLLECT_DATA.name(), collectDataTask.getId());

        assertEquals(CommandName.SYSTEM_COLLECT_DATA.name() + "@" + eq.toString(), collectDataTask.toString());

        assertFalse(result.isFatal());
    }
}
