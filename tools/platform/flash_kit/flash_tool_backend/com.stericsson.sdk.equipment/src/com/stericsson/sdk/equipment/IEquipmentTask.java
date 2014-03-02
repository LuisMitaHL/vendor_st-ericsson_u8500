package com.stericsson.sdk.equipment;

/**
 * @author xtomlju
 */
public interface IEquipmentTask {

    /**
     * Key that will be used when registering a service with a specific port, i.e. USB1
     */
    String PORT_PROPERTY = IPlatform.PORT_PROPERTY;

    /**
     * Key that will be used when registering a service with a specific platform type, i.e. U8500
     * 
     * @see CommandName
     */
    String TYPE_PROPERTY = "type";

    /** */
    int ERROR_NONE = 0;

    /** */
    int ERROR = 1;

    /**
     * @return TBD
     */
    EquipmentTaskResult execute();

    /**
     * Check if task can be canceled.
     * 
     * @return True if task can be canceled
     */
    boolean isCancelable();

    /**
     * Cancel equipment task.
     */
    void cancel();

    /**
     * Get associated equipment.
     * 
     * @return Equipment interface
     */
    IEquipment getEquipment();

    /**
     * Get the unique identifier for this task
     * 
     * @return the task identifier
     */
    String getId();

    /**
     * 
     * @param arguments
     *            TBD
     */
    void setArguments(String[] arguments);

    /**
     * Add task listener
     * @param listener task listener
     */
    void addTaskListener(IEquipmentTaskListener listener);
}
