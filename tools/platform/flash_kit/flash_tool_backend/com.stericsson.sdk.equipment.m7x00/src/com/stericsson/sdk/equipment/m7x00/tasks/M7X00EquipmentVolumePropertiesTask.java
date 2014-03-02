package com.stericsson.sdk.equipment.m7x00.tasks;

import java.util.Locale;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.AbstractLCEquipmentTask;
import com.stericsson.sdk.equipment.AbstractLoaderCommunicationEquipment;
import com.stericsson.sdk.loader.communication.LCDefinitions;
import com.stericsson.sdk.loader.communication.types.VolumePropertiesType;

/**
 * This task read volume properties from device.
 * 
 * @author Vit Sykala <vit.sykala@tieto.com>
 */
public class M7X00EquipmentVolumePropertiesTask extends AbstractLCEquipmentTask {

    /**
     * @param equipment
     *            Equipment for which this task is created.
     */
    public M7X00EquipmentVolumePropertiesTask(AbstractLoaderCommunicationEquipment equipment) {
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
                double sizeMB = ((double) volumeProperty.getSize()) / 1048576;
                double freeMB = ((double) volumeProperty.getFree()) / 1048576;

                StringBuffer result = new StringBuffer();
                result.append("Type: " + volumeProperty.getFsType());
                result.append(String.format(Locale.ENGLISH, "\nSize: %1.1f MB (" + volumeProperty.getSize() + " B)",
                    sizeMB));
                result.append(String.format(Locale.ENGLISH, "\nFree: %1.1f MB (" + volumeProperty.getFree() + " B)",
                    freeMB));
                resultMessage = result.toString();
            } else {
                resultMessage =
                    ((AbstractLoaderCommunicationEquipment) getEquipment()).getLoaderCommunicationService()
                        .getLoaderErrorDesc(resultCode);
            }
        } else {
            super.updateResult(resultObj);
        }
    }
}
