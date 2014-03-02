package com.stericsson.sdk.equipment.dummy.internal.task.filesystem;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.dummy.internal.DummyEquipment;
import com.stericsson.sdk.loader.communication.LCDefinitions;
import com.stericsson.sdk.loader.communication.types.VolumePropertiesType;

/**
 * This task read volume properties from device.
 * 
 * @author Vit Sykala <vit.sykala@tieto.com>
 */
public class DummyEquipmentVolumePropertiesTask extends DummyEquipmentFileSystemTask {

    /**
     * @param equipment
     *            Equipment for which this task is created.
     */
    public DummyEquipmentVolumePropertiesTask(DummyEquipment equipment) {
        super(equipment, LCDefinitions.METHOD_NAME_FS_VOLUME_PROPERTIES, CommandName.FILE_SYSTEM_VOLUME_PROPERTIES);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void updateResult(Object resultObj) {
        if (resultObj instanceof VolumePropertiesType) {
            VolumePropertiesType volumeProperty = (VolumePropertiesType) resultObj;
            resultCode = volumeProperty.getStatus();
            if (resultCode == 0) {
                resultMessage = volumeProperty.getFsType();
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
