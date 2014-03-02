package com.stericsson.sdk.equipment.tasks;

import java.io.IOException;

import org.apache.log4j.Logger;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.AbstractEquipmentTask;
import com.stericsson.sdk.equipment.AbstractLoaderCommunicationEquipment;
import com.stericsson.sdk.equipment.EquipmentModel;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.io.AbstractPort;
import com.stericsson.sdk.equipment.io.port.PortException;
import com.stericsson.sdk.equipment.io.port.PortUtilities;
import com.stericsson.sdk.loader.communication.LCDefinitions;

/**
 * Change baud rate for loader communication
 * 
 * @author xtomzap
 * 
 */
public class EquipmentChangeBaudRateTask extends AbstractEquipmentTask {

    /** Equipment instance */
    private final AbstractLoaderCommunicationEquipment mobileEquipment;

    /** Result message for this task */
    private String resultMessage;

    /** Result code for this task */
    private int resultCode;

    private static Logger logger = Logger.getLogger(EquipmentChangeBaudRateTask.class);

    /**
     * Constructor.
     * 
     * @param equipment
     *            Equipment interface
     */
    public EquipmentChangeBaudRateTask(AbstractLoaderCommunicationEquipment equipment) {
        super(equipment);

        // Store equipment object
        mobileEquipment = equipment;

        // Initialize task result with default information
        resultCode = ERROR_NONE;
        resultMessage = "No error";
    }

    /**
     * {@inheritDoc}
     */
    public void cancel() {
        // N/A
    }

    /**
     * {@inheritDoc}
     */
    public EquipmentTaskResult execute() {

        notifyTaskStart();

        try {
            AbstractPort port = (AbstractPort) mobileEquipment.getPort();

            // change baud rate if port is not USB and if baud rate has not been already changed by
            // ROM code
            if (!PortUtilities.isUSBPort(port) && !mobileEquipment.getChangeBaudRateROMAck()) {
                changeUARTBaudRate(port);
            }
            mobileEquipment.updateModel(EquipmentModel.COMMAND);

        } catch (IOException e) {
            resultCode = 1;
            resultMessage = e.getMessage();
        }

        return new EquipmentTaskResult(resultCode, resultMessage, null, (resultCode != 0));
    }

    /**
     * Changes UART port baud rate.
     * 
     * @param port
     *            Port to be changed.
     * @throws PortException
     *             Port exception.
     */
    private void changeUARTBaudRate(AbstractPort port) throws PortException {

        String uartSpeed = mobileEquipment.getProfile().getProperty(EquipmentBootTask.UART_BAUDRATE);
        if (uartSpeed == null) {
            return;
        }

        int speed = EquipmentBootTask.UART_BAUD_RATE_DEFAULT;
        try {
            speed = Integer.parseInt(uartSpeed.substring(uartSpeed.lastIndexOf("=") + 1));
        } catch (NumberFormatException e) {
            logger.error("UART baud rate speed specification in configuration file is invalid.", e);
        }
        if (speed == EquipmentBootTask.UART_BAUD_RATE_DEFAULT) {
            return;
        }

        notifyTaskMessage("Backend will switch uart speed to " + uartSpeed);
        logger.debug("calling change speed in loader");
        int result =
            (Integer) mobileEquipment.sendCommand(LCDefinitions.METHOD_NAME_SYSTEM_CHANGE_BAUDRATE, new Object[] {
                speed}, this);
        if (result != 0) {
            resultCode = result;
            resultMessage = mobileEquipment.getLoaderCommunicationService().getLoaderErrorDesc(result);
            logger.error("Speed change in loader not successfull. Will continue on default speed.");
        } else {
            logger.debug("Speed change in loader successfull. Will change speed in backend to " + uartSpeed);
            try {
                Thread.sleep(1000);
            } catch (InterruptedException e) {
                logger.debug("Interrupted while sleeping.");
            }
            port.setSpeed(uartSpeed);
            try {
                Thread.sleep(1000);
            } catch (InterruptedException e) {
                logger.debug("Interrupted while sleeping.");
            }
        }

        notifyTaskMessage("Uart speed is changed now");
    }

    /**
     * {@inheritDoc}
     */
    public String getId() {
        return CommandName.CHANGE_BAUD_RATE;
    }

    /**
     * {@inheritDoc}
     */
    public boolean isCancelable() {
        return false;
    }

    /**
     * {@inheritDoc}
     */
    public void setArguments(String[] arguments) {

    }

}
