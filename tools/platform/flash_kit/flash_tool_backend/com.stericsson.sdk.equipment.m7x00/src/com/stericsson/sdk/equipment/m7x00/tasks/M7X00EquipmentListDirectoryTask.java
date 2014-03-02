package com.stericsson.sdk.equipment.m7x00.tasks;

import java.util.Date;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.AbstractLCEquipmentTask;
import com.stericsson.sdk.equipment.AbstractLoaderCommunicationEquipment;
import com.stericsson.sdk.loader.communication.LCDefinitions;
import com.stericsson.sdk.loader.communication.types.ListDirectoryType;
import com.stericsson.sdk.loader.communication.types.ModeType;
import com.stericsson.sdk.loader.communication.types.subtypes.Entry;

/**
 * This task List files in selected directory of device.
 * 
 * @author Vit Sykala <vit.sykala@tieto.com>
 */
public class M7X00EquipmentListDirectoryTask extends AbstractLCEquipmentTask {

    /**
     * @param equipment
     *            Equipment for which this task is created.
     */
    public M7X00EquipmentListDirectoryTask(AbstractLoaderCommunicationEquipment equipment) {
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
                    resultMessage +=
                        "\n" + parseType(dir.getMode().getType()) + "\t"
                            + parsePermision(dir.getMode().getAccessPermissionOwner())
                            + parsePermision(dir.getMode().getAccessPermissionGroup())
                            + parsePermision(dir.getMode().getAccessPermissionOthers()) + "\t" + dir.getSize() + "\t"
                            + new Date(dir.getTime()).toString() + "\t" + dir.getName();
                }
            } else {
                resultMessage =
                    ((AbstractLoaderCommunicationEquipment) getEquipment()).getLoaderCommunicationService()
                        .getLoaderErrorDesc((int) resultCode);
            }
        } else {
            super.updateResult(resultObj);
        }
    }

    private String parseType(String type) {
        if (type.equalsIgnoreCase(ModeType.DIRECTORY_FILE_STR)) {
            return "Dir";
        } else {
            return "File";
        }
    }

    private String parsePermision(int perm) {
        switch (perm) {
            case 0:
                return "---";
            case 1:
                return "--x";
            case 2:
                return "-w-";
            case 3:
                return "-wx";
            case 4:
                return "r--";
            case 5:
                return "r-x";
            case 6:
                return "rw-";
            case 7:
                return "rwx";
            default:
                return "---";
        }

    }
}
