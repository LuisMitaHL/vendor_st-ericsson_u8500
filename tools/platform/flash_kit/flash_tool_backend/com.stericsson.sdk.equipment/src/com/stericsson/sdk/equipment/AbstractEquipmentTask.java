package com.stericsson.sdk.equipment;

import java.util.ArrayList;
import java.util.Dictionary;
import java.util.Hashtable;
import java.util.List;

import com.stericsson.sdk.equipment.io.port.IPort;
import com.stericsson.sdk.loader.communication.ILoaderCommunicationListener;

/**
 * @author xtomlju
 */
public abstract class AbstractEquipmentTask implements IEquipmentTask, ILoaderCommunicationListener {

    /**
     * string to end all command success
     */
    protected static final String COMPLETED = " completed.";

    private IEquipment mobileEquipment;

    /**
     * default result code
     */
    protected int resultCode = ERROR_NONE;

    /**
     * default result message
     */
    protected String resultMessage = "Command finished successfully";

    private List<IEquipmentTaskListener> taskListeners = new ArrayList<IEquipmentTaskListener>();

    /**
     * Constructor.
     * 
     * @param equipment
     *            Equipment object
     */
    public AbstractEquipmentTask(IEquipment equipment) {
        mobileEquipment = equipment;

        Dictionary<String, String> properties = new Hashtable<String, String>();
        properties.put(ILoaderCommunicationListener.INSTANCE_POINTER, mobileEquipment.getPort().getPortName());

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
    public void notifyTaskMessage(String message) {
        for (IEquipmentTaskListener listener : taskListeners) {
            if (listener != null) {
                listener.taskMessage(this, message);
            }
        }
    }

    /**
     * {@inheritDoc}
     * 
     * @see com.stericsson.sdk.loader.communication.ILoaderCommunicationListener#Failure(java.lang.String,
     *      java.lang.String, boolean)
     */
    public void loaderCommunicationMessage(String title, String message, boolean logOnly) {
        notifyTaskMessage(message);
    }

    /**
     * {@inheritDoc}
     * 
     * @see com.stericsson.sdk.loader.communication.ILoaderCommunicationListener#Progress(long,
     *      long)
     */
    public void loaderCommunicationProgress(long totalBytes, long transferredBytes) {
        notifyTaskProgress(totalBytes, transferredBytes);
    }

    /**
     * {@inheritDoc}
     * 
     * @see com.stericsson.sdk.loader.communication.ILoaderCommunicationListener#Progress(long,
     *      long)
     */
    public void loaderCommunicationError(IPort port, String message) {
        notifyTaskMessage(message);
    }

    /**
     * Add task listener {@inheritDoc}
     */
    public void addTaskListener(IEquipmentTaskListener listener) {
        taskListeners.add(listener);
    }
}
