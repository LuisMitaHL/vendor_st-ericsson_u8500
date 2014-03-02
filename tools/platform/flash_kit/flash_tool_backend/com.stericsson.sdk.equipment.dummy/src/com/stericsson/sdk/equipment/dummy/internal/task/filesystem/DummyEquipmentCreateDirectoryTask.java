package com.stericsson.sdk.equipment.dummy.internal.task.filesystem;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.dummy.internal.DummyEquipment;
import com.stericsson.sdk.loader.communication.LCDefinitions;

/**
 * This task create directories in device file system.
 *
 * @author Vit Sykala <vit.sykala@tieto.com>
 */
public class DummyEquipmentCreateDirectoryTask extends DummyEquipmentFileSystemTask{

    /**
     * @param equipment
     *            Equipment for which this task is created.
     */
    public DummyEquipmentCreateDirectoryTask(DummyEquipment equipment) {
        super(equipment,LCDefinitions.METHOD_NAME_FS_CREATE_DIRECTORY, CommandName.FILE_SYSTEM_CREATE_DIRECTORY);
    }
}
