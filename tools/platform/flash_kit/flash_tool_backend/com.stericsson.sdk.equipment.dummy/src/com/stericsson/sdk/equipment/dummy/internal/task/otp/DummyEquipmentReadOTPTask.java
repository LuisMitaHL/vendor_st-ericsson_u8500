package com.stericsson.sdk.equipment.dummy.internal.task.otp;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.dummy.internal.AbstractDummyEquipmentTask;
import com.stericsson.sdk.equipment.dummy.internal.DummyEquipment;

/**
 * Task for reading OTP data from specified ME and writing it to specified file.
 * 
 * @author vsykal01
 * 
 */
public class DummyEquipmentReadOTPTask extends AbstractDummyEquipmentTask {

    /**
     * @param equipment
     *            Equipment for which this task is.
     */
    public DummyEquipmentReadOTPTask(DummyEquipment equipment) {
        super(equipment, CommandName.OTP_READ_OTP);
    }

    /**
     * {@inheritDoc}
     */
    public EquipmentTaskResult execute() {
        //throw new RuntimeException("Not yet implemented");
        return new EquipmentTaskResult(0, "", null, false);
    }
}
