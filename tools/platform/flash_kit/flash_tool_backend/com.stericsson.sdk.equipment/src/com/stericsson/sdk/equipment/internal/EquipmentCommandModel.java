package com.stericsson.sdk.equipment.internal;

import java.util.List;

import com.stericsson.sdk.equipment.model.IEquipmentCommandModel;
import com.stericsson.sdk.loader.communication.LCCommandMappings;
import com.stericsson.sdk.loader.communication.types.SupportedCommandsType;
import com.stericsson.sdk.loader.communication.types.subtypes.SupportedCommand;

/**
 * @author xtomlju
 */
public class EquipmentCommandModel implements IEquipmentCommandModel {

    /** */
    private List<SupportedCommand> commands;

    /**
     * Constructor
     *
     * @param supportedCommands
     *            Supported commands
     */
    public EquipmentCommandModel(SupportedCommandsType supportedCommands) {
        commands = supportedCommands.getSupportedCommands();
    }

    /**
     * {@inheritDoc}
     */
    public String[] getSupportedCommandNames() {

        int i = 0;
        String[] result = new String[commands.size()];

        for (SupportedCommand command : commands) {
            result[i++] = LCCommandMappings.getCommandString(command.getGroup(), command.getCommand());
        }

        return result;
    }

}
