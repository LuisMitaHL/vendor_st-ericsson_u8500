package com.stericsson.sdk.equipment;

import java.util.ArrayList;
import java.util.List;

import com.stericsson.sdk.equipment.io.port.IPort;
import com.stericsson.sdk.ftp.communication.IFTPCommunicatorListener;

/**
 * Abstract class for warm equipment task. Warm equipment is an equipment which is booted and not in
 * flash-mode.
 * 
 * @author eolabor
 */
public abstract class AbstractWarmEquipmentTask implements IEquipmentTask, IFTPCommunicatorListener {

    /**
     * String to end all command success.
     */
    protected static final String COMPLETED = " completed.";

    private IEquipment mobileEquipment;

    private List<IEquipmentTaskListener> taskListeners = new ArrayList<IEquipmentTaskListener>();

    /**
     * Constructor.
     * 
     * @param equipment
     *            Equipment object
     */
    public AbstractWarmEquipmentTask(IEquipment equipment) {
        mobileEquipment = equipment;

    }

    /**
     * {@inheritDoc}
     */
    public IEquipment getEquipment() {
        return mobileEquipment;
    }

    /**
     * Notify all equipment task listeners about task start.
     */
    protected void notifyTaskStart() {
        for (IEquipmentTaskListener listener : taskListeners) {
            if (listener != null) {
                listener.taskStart(this);
            }
        }
    }

    /**
     * {@inheritDoc} Notify all equipment task listeners about task progress.
     */
    protected void notifyTaskProgress(long totalBytes, long transferredBytes) {
        for (IEquipmentTaskListener listener : taskListeners) {
            if (listener != null) {
                listener.taskProgress(this, totalBytes, (transferredBytes));
            }
        }
    }

    /**
     * Notify all equipment task listeners about task message.
     * 
     * @param message
     *            Message from task
     */
    protected void notifyTaskMessage(String message) {
        for (IEquipmentTaskListener listener : taskListeners) {
            if (listener != null) {
                listener.taskMessage(this, message);
            }
        }
    }

    /**
     * {@inheritDoc}
     * 
     */
    public void error(IPort port, String message) {
        // N/A
    }

    /**
     * {@inheritDoc}
     * 
     */
    public void taskProgress(long totalBytes, long transferredBytes) {
        notifyTaskProgress(totalBytes, transferredBytes);
    }

    /**
     * Add task listener
     * {@inheritDoc}
     */
    public void addTaskListener(IEquipmentTaskListener listener) {
        taskListeners.add(listener);
    }

    /**
     * Remove task listener
     * {@inheritDoc}
     */
    public void removeTaskListener(IEquipmentTaskListener listener) {
        taskListeners.remove(listener);
    }

}
