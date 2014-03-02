package com.stericsson.sdk.equipment.dummy.internal.task.filesystem;

import java.sql.Time;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.dummy.internal.DummyEquipment;
import com.stericsson.sdk.loader.communication.LCDefinitions;
import com.stericsson.sdk.loader.communication.types.FSPropertiesType;

/**
 * This task read file properties from device.
 * 
 * @author Vit Sykala <vit.sykala@tieto.com>
 */
public class DummyEquipmentPropertiesTask extends DummyEquipmentFileSystemTask {

    /**
     * @param equipment
     *            Equipment for which this task is created.
     */
    public DummyEquipmentPropertiesTask(DummyEquipment equipment) {
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

    /**
     * @param property
     * @return string for result from this task
     */
    private String getResultString(FSPropertiesType pProperty) {
        StringBuffer result = new StringBuffer();
        result.append("\nMode:                ");
        result.append(pProperty.getMode().getType());
        result.append("\nSize:                ");
        result.append(pProperty.getSize());
        result.append("\nModificationTime:    ");
        result.append(new Time(pProperty.getModificationTime()));
        result.append("\nAccessTime:          ");
        result.append(new Time(pProperty.getAccessTime()));
        result.append("\nCreateTime:          ");
        result.append(new Time(pProperty.getCreateTime()));
        return result.toString();
    }
}
