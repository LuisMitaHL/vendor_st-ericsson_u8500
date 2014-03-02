package com.stericsson.sdk.equipment.m7x00.tasks;

import java.sql.Time;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.AbstractLCEquipmentTask;
import com.stericsson.sdk.equipment.AbstractLoaderCommunicationEquipment;
import com.stericsson.sdk.loader.communication.LCDefinitions;
import com.stericsson.sdk.loader.communication.types.FSPropertiesType;

/**
 * This task read file properties from device.
 * 
 * @author Vit Sykala <vit.sykala@tieto.com>
 */
public class M7X00EquipmentPropertiesTask extends AbstractLCEquipmentTask {

    /**
     * @param equipment
     *            Equipment for which this task is created.
     */
    public M7X00EquipmentPropertiesTask(AbstractLoaderCommunicationEquipment equipment) {
        super(equipment, LCDefinitions.METHOD_NAME_FS_PROPERTIES, CommandName.FILE_SYSTEM_PROPERTIES);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void updateResult(Object resultObj) {
        if (resultObj instanceof FSPropertiesType) {
            FSPropertiesType property = (FSPropertiesType) resultObj;
            resultCode = property.getStatus();
            if (resultCode == 0) {
                resultMessage = getResultString(property);
            } else {
                resultMessage =
                    ((AbstractLoaderCommunicationEquipment) getEquipment()).getLoaderCommunicationService()
                        .getLoaderErrorDesc(resultCode);
            }
        } else {
            super.updateResult(resultObj);
        }
    }

    /**
     * @param property
     * @return string for result from this task
     */
    private String getResultString(FSPropertiesType pProperty) {
        StringBuffer result = new StringBuffer();
        result.append("Type:     ");
        result.append(pProperty.getMode().getType());
        result.append("\nSize:     ");
        result.append(pProperty.getSize() + " B");
        result.append("\nCreated:  ");
        result.append(new Time(pProperty.getCreateTime()));
        result.append("\nModified: ");
        result.append(new Time(pProperty.getModificationTime()));
        result.append("\nAccessed: ");
        result.append(new Time(pProperty.getAccessTime()));
        return result.toString();
    }
}
