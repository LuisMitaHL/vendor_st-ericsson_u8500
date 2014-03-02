package com.stericsson.sdk.equipment;

/**
 * Implementation of EquipmentTaskResult.
 * A data holder for the task result.
 * @author xadazim
 * 
 */
public class EquipmentTaskResult {
    private int resultCode;

    private String resultMessage;

    private Object resultObject;

    private boolean fatal;

    private long duration;

    /**
     * Constructs task result.
     * 
     * @param pResultCode
     *            result code (most often LCD command exit code)
     * @param pResultMessage
     *            result message (often LCD error string in case of error)
     * @param pResultObject
     *            result object, non null for tasks in which resultCode and resultMessage are not
     *            enough
     * @param pFatal
     *            true if fatal error has occured during command execution, means that the ME is
     *            unable to communicate till a restart
     */
    public EquipmentTaskResult(int pResultCode, String pResultMessage, Object pResultObject, boolean pFatal) {
        resultCode = pResultCode;
        resultMessage = pResultMessage;
        resultObject = pResultObject;
        fatal = pFatal;
    }

    /**
     * Constructs task result.
     * 
     * @param pResultCode
     *            result code (most often LCD command exit code)
     * @param pResultMessage
     *            result message (often LCD error string in case of error)
     * @param pResultObject
     *            result object, non null for tasks in which resultCode and resultMessage are not
     *            enough
     * @param pFatal
     *            true if fatal error has occured during command execution, means that the ME is
     *            unable to communicate till a restart
     * @param pDuration duration of task
     */
    public EquipmentTaskResult(int pResultCode, String pResultMessage, Object pResultObject, boolean pFatal, long pDuration) {
        resultCode = pResultCode;
        resultMessage = pResultMessage;
        resultObject = pResultObject;
        fatal = pFatal;
        duration = pDuration;
    }

    /**
     * 
     * {@inheritDoc}
     */
    public int getResultCode() {
        return resultCode;
    }

    /**
     * 
     * {@inheritDoc}
     */
    public String getResultMessage() {
        return resultMessage;
    }

    /**
     * 
     * {@inheritDoc}
     */
    public Object getResultObject() {
        return resultObject;
    }

    /**
     * 
     * {@inheritDoc}
     */
    public boolean isFatal() {
        return fatal;
    }

    /**
     * Duration of task
     * @return duration
     */
    public long getDuration() {
        return duration;
    }

}
