package com.stericsson.sdk.equipment.dummy.internal.task;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.AbstractEquipmentTask;
import com.stericsson.sdk.equipment.EquipmentModel;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.dummy.internal.DummyEquipment;
import com.stericsson.sdk.equipment.dummy.internal.DummyEquipmentFlashModel;

/**
 * @author xolabju
 * 
 */
public class DummyEquipmentCollectDataTask extends AbstractEquipmentTask {

    /** Equipment instance */
    private DummyEquipment mobileEquipment;

    /** Result message for this task */
    private String resultMessage;

    /** Result code for this task */
    private int resultCode;

    private int type;

    /**
     * @param pEquipment
     *            mobile equipment
     */
    public DummyEquipmentCollectDataTask(DummyEquipment pEquipment) {
        super(pEquipment);

        // Store U8500 equipment object
        mobileEquipment = pEquipment;
        mobileEquipment.updateModel(EquipmentModel.FLASH);

        // Initialize task result with default information
        resultCode = ERROR_NONE;
        resultMessage = CommandName.getCLISyntax(getId()) + COMPLETED;
    }

    /**
     * {@inheritDoc}
     */
    public void cancel() {
        // N/A
    }

    /**
     * {@inheritDoc}
     */
    public EquipmentTaskResult execute() {
        notifyTaskStart();
        notifyTaskMessage("Attempting to collect data from ME. Type = " + type);
        DummyEquipmentFlashModel model = (DummyEquipmentFlashModel) mobileEquipment.getModel(EquipmentModel.FLASH);
        resultMessage = model.getReport();
        return new EquipmentTaskResult(resultCode, resultMessage, null, false);
    }

    /**
     * {@inheritDoc}
     */
    public String getId() {
        return CommandName.SYSTEM_COLLECT_DATA.name();
    }

    /**
     * {@inheritDoc}
     */
    public boolean isCancelable() {
        return false;
    }

    /**
     * {@inheritDoc}
     */
    public void setArguments(String[] pArguments) {
        type = Integer.parseInt(pArguments[2]);
    }

    /**
     * {@inheritDoc}
     */
    public String toString() {
        return getId() + "@" + mobileEquipment.toString();
    }
}
