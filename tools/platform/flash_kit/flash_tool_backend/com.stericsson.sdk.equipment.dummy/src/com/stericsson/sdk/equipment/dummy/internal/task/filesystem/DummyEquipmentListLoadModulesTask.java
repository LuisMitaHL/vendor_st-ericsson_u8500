/**
 * 
 */
package com.stericsson.sdk.equipment.dummy.internal.task.filesystem;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.dummy.internal.DummyEquipment;
import com.stericsson.sdk.loader.communication.LCDefinitions;

/**
 * Dummy listing of installed modules task.
 * 
 * @author mbocek01
 * 
 */
public class DummyEquipmentListLoadModulesTask extends DummyEquipmentFileSystemTask {

    /**
     * Constructs the list load modules task.
     * 
     * @param equipment
     *            equipment on which load modules shall be listed
     */
    public DummyEquipmentListLoadModulesTask(DummyEquipment equipment) {
        super(equipment, LCDefinitions.METHOD_NAME_FS_READ_LOAD_MODULE_MANIFESTS, CommandName.FILE_SYSTEM_LIST_MODULES);
    }

    /**
     * Dummy, only sets result. {@inheritDoc}
     */
    @Override
    public EquipmentTaskResult execute() {
        return new EquipmentTaskResult(0, "Module1.elf v1.0", null, false);
    }
}
