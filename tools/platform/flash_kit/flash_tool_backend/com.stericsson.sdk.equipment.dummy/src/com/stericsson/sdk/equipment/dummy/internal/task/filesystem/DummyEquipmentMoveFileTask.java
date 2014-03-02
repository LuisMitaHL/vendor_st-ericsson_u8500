package com.stericsson.sdk.equipment.dummy.internal.task.filesystem;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.dummy.internal.DummyEquipment;
import com.stericsson.sdk.loader.communication.LCDefinitions;

/**
 * This task move file from device to device.
 *
 * @author Vit Sykala <vit.sykala@tieto.com>
 */
public class DummyEquipmentMoveFileTask extends DummyEquipmentFileSystemTask{

    /**
     * @param equipment
     *            Equipment for which this task is created.
     */
    public DummyEquipmentMoveFileTask(DummyEquipment equipment) {
        super(equipment,LCDefinitions.METHOD_NAME_FS_MOVE_FILE, CommandName.FILE_SYSTEM_MOVE_FILE);
    }
}
