package com.stericsson.sdk.equipment.dummy.internal.task.otp;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.dummy.internal.AbstractDummyEquipmentTask;
import com.stericsson.sdk.equipment.dummy.internal.DummyEquipment;

/**
 * Task for burning OTP data provided in specified file to specified ME.
 * 
 * @author vsykal01
 * 
 */
public class DummyEquipmentBurnOTPTask extends AbstractDummyEquipmentTask {

    /**
     * @param equipment
     *            Equipment for which this task is.
     */
    public DummyEquipmentBurnOTPTask(DummyEquipment equipment) {
        super(equipment, CommandName.OTP_BURN_OTP);
    }

    /**
     * {@inheritDoc}
     */
    public EquipmentTaskResult execute() {
        //throw new RuntimeException("Not yet implemented");
        return new EquipmentTaskResult(0, "", null, false);
    }
}
