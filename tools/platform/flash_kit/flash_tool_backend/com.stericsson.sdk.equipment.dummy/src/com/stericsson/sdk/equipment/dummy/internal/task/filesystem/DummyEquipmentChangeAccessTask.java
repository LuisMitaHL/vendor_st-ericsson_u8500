package com.stericsson.sdk.equipment.dummy.internal.task.filesystem;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.dummy.internal.DummyEquipment;
import com.stericsson.sdk.loader.communication.LCDefinitions;

/**
 * This task change Access of file.
 *
 * @author Vit Sykala <vit.sykala@tieto.com>
 */
public class DummyEquipmentChangeAccessTask extends DummyEquipmentFileSystemTask{

    /**
     * @param equipment
     *            Equipment for which this task is created.
     */
    public DummyEquipmentChangeAccessTask(DummyEquipment equipment) {
        super(equipment,LCDefinitions.METHOD_NAME_FS_CHANGE_ACCESS, CommandName.FILE_SYSTEM_CHANGE_ACCESS);
    }

    /**
    * {@inheritDoc}
    */
    @Override
    public void setArguments(String[] pArguments) {
        super.setArguments(pArguments);
        arguments[1]=Integer.valueOf(arguments[1].toString()).intValue();
    }
}
