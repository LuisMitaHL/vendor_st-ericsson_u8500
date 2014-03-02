package com.stericsson.sdk.equipment.dummy.internal;

import org.apache.log4j.Logger;

import com.stericsson.sdk.equipment.EquipmentModel;
import com.stericsson.sdk.equipment.EquipmentState;
import com.stericsson.sdk.loader.communication.LCCommandMappings;
import com.stericsson.sdk.loader.communication.LCDefinitions;
import com.stericsson.sdk.loader.communication.types.ListDevicesType;
import com.stericsson.sdk.loader.communication.types.SupportedCommandsType;
import com.stericsson.sdk.loader.communication.types.subtypes.SupportedCommand;

/**
 * @author xtomzap
 *
 */
public class DummyEquipmentModelUpdater {

    private static Logger logger = Logger.getLogger(DummyEquipmentModelUpdater.class);

    private DummyEquipment equipment;

    /**
     * @param pEquipment mobile equipment
     */
    public DummyEquipmentModelUpdater(DummyEquipment pEquipment) {
        equipment = pEquipment;
    }

    /**
     * @param model equipment model
     */
    public void updateModel(EquipmentModel model) {
        if (model == EquipmentModel.COMMAND) {
            updateCommandModel();
        } else if (model == EquipmentModel.FLASH) {
            updateFlashModel();
        }
    }

    /**
     * 
     */
    private void updateFlashModel() {

        try {
            ListDevicesType deviceList =
                (ListDevicesType) equipment.sendCommand(LCDefinitions.METHOD_NAME_FLASH_LIST_DEVICES, new Object[0], equipment);
            if (deviceList.getStatus() == 0) {

                deviceList.addDevice(0, "/flash0/", "/flash0/".length(), "Partition", "Partition".length(), 512, 0,
                    Long.MAX_VALUE);

                equipment.setModel(EquipmentModel.FLASH, new DummyEquipmentFlashModel(deviceList));
            } else {
                equipment.setStatus(EquipmentState.ERROR);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * Update what task services should be supported by this equipment.
     */
    private void updateCommandModel() {

        try {
            SupportedCommandsType supportedCommands =
                (SupportedCommandsType) equipment.sendCommand(LCDefinitions.METHOD_NAME_SYSTEM_SUPPORTED_COMMANDS, new Object[0],
                    equipment);
            if (supportedCommands.getStatus() == 0) {
                equipment.setModel(EquipmentModel.COMMAND, new DummyEquipmentCommandModel(supportedCommands));
                for (SupportedCommand command : supportedCommands.getSupportedCommands()) {
                    if (command.getCommand() < 100) {
                        logger.debug("The loader supports: "
                            + LCCommandMappings.getCommandString(command.getGroup(), command.getCommand()));
                    }
                }
            } else {
                equipment.setStatus(EquipmentState.ERROR);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

}
