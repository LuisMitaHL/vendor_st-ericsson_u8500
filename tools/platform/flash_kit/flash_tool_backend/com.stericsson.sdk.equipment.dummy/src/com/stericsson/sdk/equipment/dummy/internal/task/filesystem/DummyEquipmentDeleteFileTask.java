package com.stericsson.sdk.equipment.dummy.internal.task.filesystem;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.dummy.internal.DummyEquipment;
import com.stericsson.sdk.loader.communication.LCDefinitions;

/**
 * This task delete file from device.
 *
 * @author Vit Sykala <vit.sykala@tieto.com>
 */
public class DummyEquipmentDeleteFileTask extends DummyEquipmentFileSystemTask{

    /**
     * @param equipment
     *            Equipment for which this task is created.
     */
    public DummyEquipmentDeleteFileTask(DummyEquipment equipment) {
        super(equipment,LCDefinitions.METHOD_NAME_FS_DELETE_FILE, CommandName.FILE_SYSTEM_DELETE_FILE);
    }
}
