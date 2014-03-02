package com.stericsson.sdk.equipment;

import org.apache.log4j.Logger;

import com.stericsson.sdk.brp.CommandName;

/**
 * Abstract LC equipment task.
 * 
 * @author Vit Sykala <vit.sykala@tieto.com>
 */
public abstract class AbstractLCEquipmentTask extends AbstractEquipmentTask {

    private static Logger logger = Logger.getLogger(AbstractLCEquipmentTask.class.getName());

    /**
     * Task result code.
     */
    protected int resultCode;

    /**
     * Task result message.
     */
    protected String resultMessage;

    /**
     * Task result object.
     */
    protected Object resultObject;

    /**
     * LC method arguments.
     */
    protected Object[] arguments;

    /**
     * Method name from {@link LCDefinitions}.
     */
    protected final String methodName;

    private final CommandName command;

    /**
     * Constructor.
     * 
     * @param equipment
     *            Equipment instance.
     * @param pMethodName
     *            Method name from {@link LCDefinitions}.
     * @param pCommand
     *            Command from {@link CommandName}.
     */
    public AbstractLCEquipmentTask(AbstractLoaderCommunicationEquipment equipment, String pMethodName,
        CommandName pCommand) {
        super(equipment);
        command = pCommand;
        methodName = pMethodName;
        resultCode = ERROR_NONE;
        resultMessage =
            ((AbstractLoaderCommunicationEquipment) getEquipment()).getLoaderCommunicationService().getLoaderErrorDesc(
                resultCode);
        resultObject = null;
    }

    /**
     * {@inheritDoc}
     */
    public void cancel() {
        throw new RuntimeException("Not supported");
    }

    /**
     * {@inheritDoc}
     */
    public EquipmentTaskResult execute() {
        if (resultCode == ERROR_NONE) {
            try {
                notifyTaskStart();
                resultObject =
                    ((AbstractLoaderCommunicationEquipment) getEquipment()).sendCommand(methodName, arguments, this);
                updateResult(resultObject);
            } catch (Exception e) {
                logger.error(e.getMessage(), e);
                resultCode = ERROR;
                resultMessage = "Exception: " + e.getMessage();
            }
        }
        return new EquipmentTaskResult(resultCode, resultMessage, resultObject, false);
    }

    /**
     * Updates task result code and message.
     * 
     * @param resultObj
     *            Task result object.
     */
    protected void updateResult(Object resultObj) {
        if (resultObj instanceof Integer) {
            resultCode = (Integer) resultObj;
            resultMessage =
                ((AbstractLoaderCommunicationEquipment) getEquipment()).getLoaderCommunicationService()
                    .getLoaderErrorDesc(resultCode);
        }
    }

    /**
     * {@inheritDoc}
     */
    public String getId() {
        return command.name();
    }

    /**
     * {@inheritDoc}
     */
    public boolean isCancelable() {
        return false;
    }

    /**
     * Returns task result code.
     * 
     * @return Task result code.
     */
    public int getResultCode() {
        return resultCode;
    }

    /**
     * Returns task result message.
     * 
     * @return Task result message.
     */
    public String getResultMessage() {
        return resultMessage;
    }

    /**
     * Returns task result object.
     * 
     * @return Task result object.
     */
    public Object getResultObject() {
        return resultObject;
    }

    /**
     * Sets LC method arguments from given command arguments.
     * 
     * @param pArguments
     *            Command arguments.
     */
    public void setArguments(String[] pArguments) {
        arguments = new Object[pArguments.length - 2];
        System.arraycopy(pArguments, 2, arguments, 0, pArguments.length - 2);
    }

    /**
     * Returns array of command method arguments.
     * 
     * @return Array of command method arguments.
     */
    public Object[] getArguments() {
        return arguments;
    }

    /**
     * Returns {@link LCDefinitions} method name.
     * 
     * @return Method name.
     */
    public String getMethodName() {
        return methodName;
    }

    /**
     * {@inheritDoc}
     */
    public String toString() {
        return getId() + "@" + getEquipment();
    }
}
