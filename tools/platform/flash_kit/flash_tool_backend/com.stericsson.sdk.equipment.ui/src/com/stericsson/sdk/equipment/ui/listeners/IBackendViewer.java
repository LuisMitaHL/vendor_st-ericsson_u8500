package com.stericsson.sdk.equipment.ui.listeners;

import com.stericsson.sdk.common.configuration.IConfigurationRecord;
import com.stericsson.sdk.common.configuration.IConfigurationService;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.IEquipment;
import com.stericsson.sdk.equipment.IEquipmentTask;

/**
 * Interface for a class that wants to display states and changes of the flash tool backend
 * 
 * @author xolabju
 * 
 */
public interface IBackendViewer {

    /**
     * Called when an equipment is connected
     * 
     * @param equipment
     *            the connected equipment
     */
    void equipmentConnected(IEquipment equipment);

    /**
     * Called when an equipment is disconnected
     * 
     * @param equipment
     *            the disconnected equipment
     */
    void equipmentDisconnected(IEquipment equipment);

    /**
     * Called when a connected equipment is changed (e.g. state or profile changed)
     * 
     * @param equipment
     *            the changed equipment
     */
    void equipmentChanged(IEquipment equipment);

    /**
     * Called when a message is sent from task running on a specific equipment
     * 
     * @param equipment
     *            the equipment
     * @param message
     *            the message
     */
    void equipmentMessage(IEquipment equipment, String message);

    /**
     * Gets the selected equipment, or null if this no equipment is selected or if the viewer
     * doesn't support selecting
     * 
     * @return the selected equipment
     */
    IEquipment getSelectedEquipment();

    /**
     * Called when a configuration record has been changed or made available.
     * 
     * @param service
     *            Configuration service
     * @param oldRecord
     *            Old configuration record (value) or null if record is made available
     * @param newRecord
     *            New configuration record (value)
     */
    void configurationChanged(IConfigurationService service, IConfigurationRecord oldRecord,
        IConfigurationRecord newRecord);

    /**
     * Called when a task is complete
     * 
     * @param task
     *            the task
     * 
     * @param result
     *            the result
     * 
     * @param duration
     *            the task duration
     */
    void equipmentTaskDone(IEquipmentTask task, EquipmentTaskResult result, long duration);
}
