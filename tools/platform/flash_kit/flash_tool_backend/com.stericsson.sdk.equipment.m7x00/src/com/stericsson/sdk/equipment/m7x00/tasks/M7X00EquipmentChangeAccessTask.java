package com.stericsson.sdk.equipment.m7x00.tasks;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.AbstractLCEquipmentTask;
import com.stericsson.sdk.equipment.AbstractLoaderCommunicationEquipment;
import com.stericsson.sdk.loader.communication.LCDefinitions;

/**
 * This task change Access of file.
 *
 * @author Vit Sykala <vit.sykala@tieto.com>
 */
public class M7X00EquipmentChangeAccessTask extends AbstractLCEquipmentTask{

    /**
     * @param equipment
     *            Equipment for which this task is created.
     */
    public M7X00EquipmentChangeAccessTask(AbstractLoaderCommunicationEquipment equipment) {
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
