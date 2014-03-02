package com.stericsson.sdk.equipment.u5500.test;

import junit.framework.TestCase;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.tasks.EquipmentSendRawDataTask;
import com.stericsson.sdk.equipment.u5500.internal.U5500Equipment;

/**
 * @author xdancho
 *
 */
public class U5500EquipmentSendRawDataTaskTest extends TestCase {

    EquipmentSendRawDataTask sendRawDataTask = null;

    boolean wait = true;

    static final int RESULT_SUCCESS = 0;

    static final int RESULT_FAIL = 5;

    static final String RESULT_MSG = " completed.";

    static final String DATA = "CAFEBABE";

    static final String[] ARGUMENTS = new String[] {
        "SEND_RAW_DATA", "<equipment-id>", DATA};

    /**
     * test send raw data
     */
    public void testU8500SendRawData() {

        U5500Equipment eq = new U5500Equipment(Activator.getPort(), null);
        sendRawDataTask = new EquipmentSendRawDataTask(eq);

        sendRawDataTask.setArguments(ARGUMENTS);
        EquipmentTaskResult result = sendRawDataTask.execute();
        assertEquals(RESULT_SUCCESS, result.getResultCode());
        assertEquals(CommandName.getCLISyntax(sendRawDataTask.getId()) + RESULT_MSG, result.getResultMessage());

        assertNull(result.getResultObject());

        assertFalse(sendRawDataTask.isCancelable());

        assertEquals(CommandName.SYSTEM_SEND_RAW_DATA.name(), sendRawDataTask.getId());

        assertEquals(sendRawDataTask.getId() + "@" + eq.toString(), sendRawDataTask.toString());

        assertFalse(result.isFatal());

    }

}
