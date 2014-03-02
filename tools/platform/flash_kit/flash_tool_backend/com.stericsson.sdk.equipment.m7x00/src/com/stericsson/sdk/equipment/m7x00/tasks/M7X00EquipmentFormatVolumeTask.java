package com.stericsson.sdk.equipment.m7x00.tasks;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.AbstractLCEquipmentTask;
import com.stericsson.sdk.equipment.AbstractLoaderCommunicationEquipment;
import com.stericsson.sdk.loader.communication.LCDefinitions;

/**
 * This task format volume in device.
 *
 * @author Vit Sykala <vit.sykala@tieto.com>
 */
public class M7X00EquipmentFormatVolumeTask extends AbstractLCEquipmentTask {

    /**
     * @param equipment
     *            Equipment for which this task is created.
     */
    public M7X00EquipmentFormatVolumeTask(AbstractLoaderCommunicationEquipment equipment) {
        super(equipment,LCDefinitions.METHOD_NAME_FS_FORMAT_VOLUME, CommandName.FILE_SYSTEM_FORMAT_VOLUME);
    }

}
