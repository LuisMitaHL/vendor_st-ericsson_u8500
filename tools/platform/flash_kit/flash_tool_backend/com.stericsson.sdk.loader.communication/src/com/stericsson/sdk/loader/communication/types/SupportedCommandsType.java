package com.stericsson.sdk.loader.communication.types;

import java.util.ArrayList;
import java.util.List;

import com.stericsson.sdk.loader.communication.types.subtypes.SupportedCommand;

/**
 * @author xdancho
 * 
 */
public class SupportedCommandsType extends LoaderCommunicationType {

    int commandCount = 0;

    List<SupportedCommand> supportedCommandsList = new ArrayList<SupportedCommand>();

    /**
     * @param status
     *            result
     * @param cmdCount
     *            command count
     */
    public SupportedCommandsType(int status, int cmdCount) {
        super(status);
        this.commandCount = cmdCount;
    }

    /**
     * 
     * @param command
     *            cmd
     * @param group
     *            cmd group
     * @param permitted
     *            is permitted
     */
    public void addSupportedCommand(int command, int group, int permitted) {
        supportedCommandsList.add(new SupportedCommand(group, command, permitted));

    }

    /**
     * @return list of supported commands
     */
    public List<SupportedCommand> getSupportedCommands() {
        return supportedCommandsList;
    }

}
