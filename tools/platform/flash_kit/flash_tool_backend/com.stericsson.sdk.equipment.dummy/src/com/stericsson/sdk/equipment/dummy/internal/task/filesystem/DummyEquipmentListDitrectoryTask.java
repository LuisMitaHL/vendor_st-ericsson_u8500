package com.stericsson.sdk.equipment.dummy.internal.task.filesystem;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.dummy.internal.DummyEquipment;
import com.stericsson.sdk.loader.communication.LCDefinitions;
import com.stericsson.sdk.loader.communication.types.ListDirectoryType;
import com.stericsson.sdk.loader.communication.types.subtypes.Entry;

/**
 * This task List files in selected directory of device.
 * 
 * @author Vit Sykala <vit.sykala@tieto.com>
 */
public class DummyEquipmentListDitrectoryTask extends DummyEquipmentFileSystemTask {

    /**
     * @param equipment
     *            Equipment for which this task is created.
     */
    public DummyEquipmentListDitrectoryTask(DummyEquipment equipment) {
        super(equipment, LCDefinitions.METHOD_NAME_FS_LIST_DIRECTORY, CommandName.FILE_SYSTEM_LIST_DIRECTORY);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void updateResult(Object resultObj) {
        if (resultObj instanceof ListDirectoryType) {
            ListDirectoryType directoryList = (ListDirectoryType) resultObj;
            resultCode = directoryList.getStatus();
            if (resultCode == 0) {
                resultMessage = "";
                for (Entry dir : directoryList.getDirectorys()) {
                    resultMessage += "\n" + dir.getName();
                }
            } else {
                try {
                    resultMessage = mobileEquipment.getMock().getLoaderErrorDesc(resultCode);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        } else {
            super.updateResult(resultObj);
        }
    }
}
