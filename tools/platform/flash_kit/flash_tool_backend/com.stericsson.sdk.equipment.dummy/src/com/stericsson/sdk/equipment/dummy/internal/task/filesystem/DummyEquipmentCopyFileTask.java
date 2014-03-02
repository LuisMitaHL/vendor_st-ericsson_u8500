package com.stericsson.sdk.equipment.dummy.internal.task.filesystem;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.dummy.internal.DummyEquipment;
import com.stericsson.sdk.loader.communication.LCDefinitions;

/**
 * This task copy files between device and computer or from phone to phone.
 *
 * @author Vit Sykala <vit.sykala@tieto.com>
 */
public class DummyEquipmentCopyFileTask extends DummyEquipmentFileSystemTask {

    private static final String PC = "PC:";

    /**
     * @param equipment
     *            Equipment for which this task is created.
     */
    public DummyEquipmentCopyFileTask(DummyEquipment equipment) {
        super(equipment, LCDefinitions.METHOD_NAME_FS_COPY_FILE, CommandName.FILE_SYSTEM_COPY_FILE);
    }

    /**
     * From 2 arguments makes 4 arguments which are necessary for method fileSystemCopyFile.
     * @param pArguments .
     * @see LoaderCommunicationInterface
     */
    @Override
    public void setArguments(String[] pArguments) {
        String src = pArguments[2];
        String dest = pArguments[3];
        boolean srcIsPC = src.startsWith(PC);
        if (srcIsPC) {
            src = src.substring(PC.length());
        }
        boolean destIsPC = dest.startsWith(PC);
        if (destIsPC) {
            dest = dest.substring(PC.length());
        }
        super.arguments = new Object[] {
            src, dest, srcIsPC, destIsPC};
    }

}
