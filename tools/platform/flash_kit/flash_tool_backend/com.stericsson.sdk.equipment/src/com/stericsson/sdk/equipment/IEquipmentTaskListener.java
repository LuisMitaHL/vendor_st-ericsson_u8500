package com.stericsson.sdk.equipment;

/**
 * @author xtomlju
 */
public interface IEquipmentTaskListener {

    /**
     * @param task
     *            TBD
     */
    void taskStart(IEquipmentTask task);

    /**
     * {@inheritDoc}
     * 
     * @param task
     *            TBD
     */
    void taskProgress(IEquipmentTask task, long totalBytes, long transferredBytes);

    /**
     * @param task
     *            TBD
     * @param message
     *            TBD
     */
    void taskMessage(IEquipmentTask task, String message);

}
