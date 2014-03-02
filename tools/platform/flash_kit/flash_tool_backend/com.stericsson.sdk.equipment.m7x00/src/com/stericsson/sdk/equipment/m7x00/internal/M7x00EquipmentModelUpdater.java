package com.stericsson.sdk.equipment.m7x00.internal;

import org.apache.log4j.Logger;

import com.stericsson.sdk.equipment.EquipmentModel;
import com.stericsson.sdk.equipment.EquipmentState;
import com.stericsson.sdk.equipment.internal.EquipmentCommandModel;
import com.stericsson.sdk.equipment.tasks.EquipmentFlashModel;
import com.stericsson.sdk.loader.communication.LCCommandMappings;
import com.stericsson.sdk.loader.communication.LCDefinitions;
import com.stericsson.sdk.loader.communication.types.ListDevicesType;
import com.stericsson.sdk.loader.communication.types.SupportedCommandsType;
import com.stericsson.sdk.loader.communication.types.SystemCollectDataType;
import com.stericsson.sdk.loader.communication.types.subtypes.SupportedCommand;

/**
 * @author xtomzap
 *
 */
public class M7x00EquipmentModelUpdater {

    /** */
    private static Logger logger = Logger.getLogger(M7x00EquipmentModelUpdater.class);

    private M7x00Equipment equipment;

    /**
     * @param pM7x00Equipment mobile equipment
     */
    public M7x00EquipmentModelUpdater(M7x00Equipment pM7x00Equipment) {
        equipment = pM7x00Equipment;
    }

    /**
     * {@inheritDoc}
     */
    public void updateModel(EquipmentModel model) {
        if (model == EquipmentModel.COMMAND) {
            updateCommandModel();
        } else if (model == EquipmentModel.FLASH) {
            updateFlashModel();
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
            if (supportedCommands != null && supportedCommands.getStatus() == 0) {
                new CommandModelHelper().updateModel(supportedCommands);
            } else {
                equipment.setStatus(EquipmentState.ERROR);
            }
        } catch (Exception e) {
            logger.error(e.getMessage(), e);
        }
    }

    private class CommandModelHelper {
        public void updateModel(SupportedCommandsType supportedCommands) {
            equipment.setModel(EquipmentModel.COMMAND, new EquipmentCommandModel(supportedCommands));
            for (SupportedCommand command : supportedCommands.getSupportedCommands()) {
                if (command.getCommand() < 100) {
                    logger.debug("The loader supports: "
                        + LCCommandMappings.getCommandString(command.getGroup(), command.getCommand()));
                }
            }
        }
    }

    /**
     * still similar like u8500
     */
    private void updateFlashModel() {
        try {
            ListDevicesType deviceList =
                (ListDevicesType) equipment.sendCommand(LCDefinitions.METHOD_NAME_FLASH_LIST_DEVICES, new Object[0],
                    equipment);
            if (deviceList.getStatus() == 0) {
                SystemCollectDataType result2 =
                    (SystemCollectDataType) equipment.sendCommand(LCDefinitions.METHOD_NAME_SYSTEM_COLLECT_DATA,
                        new Object[] {
                            2}, equipment);
                if (result2.getStatus() == 0) {
                    equipment.setModel(EquipmentModel.FLASH, new EquipmentFlashModel(deviceList, result2.getCollectedData()));
                } else {
                    equipment.setModel(EquipmentModel.FLASH, new EquipmentFlashModel(deviceList, null));
                }
            } else {
                equipment.setStatus(EquipmentState.ERROR);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
