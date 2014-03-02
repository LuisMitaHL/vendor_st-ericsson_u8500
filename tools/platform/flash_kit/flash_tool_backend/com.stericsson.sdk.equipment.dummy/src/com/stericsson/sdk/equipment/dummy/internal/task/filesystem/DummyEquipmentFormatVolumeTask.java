package com.stericsson.sdk.equipment.dummy.internal.task.filesystem;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.dummy.internal.DummyEquipment;
import com.stericsson.sdk.loader.communication.LCDefinitions;

/**
 * This task format volume in device.
 *
 * @author Vit Sykala <vit.sykala@tieto.com>
 */
public class DummyEquipmentFormatVolumeTask extends DummyEquipmentFileSystemTask {

    /**
     * @param equipment
     *            Equipment for which this task is created.
     */
    public DummyEquipmentFormatVolumeTask(DummyEquipment equipment) {
        super(equipment,LCDefinitions.METHOD_NAME_FS_FORMAT_VOLUME, CommandName.FILE_SYSTEM_FORMAT_VOLUME);
    }

}
